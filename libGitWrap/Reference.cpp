/*
 * MacGitver
 * Copyright (C) 2012-2013 Sascha Cunz <sascha@babbelbox.org>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License (Version 2) as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "GitWrapPrivate.hpp"
#include "ReferencePrivate.hpp"
#include "ObjectCommit.hpp"
#include "ObjectId.hpp"
#include "Repository.hpp"
#include "Reference.hpp"

namespace Git
{

    namespace Internal
    {

        ReferencePrivate::ReferencePrivate( const GitPtr< RepositoryPrivate >& repo,
                                            git_reference* ref )
            : RepoObject( repo )
            , mRef( ref )
        {
            Q_ASSERT( ref );
        }

        ReferencePrivate::~ReferencePrivate()
        {
            git_reference_free( mRef );
        }

    }

    Reference::Reference()
    {
    }

    Reference::Reference( Internal::ReferencePrivate* _d )
        : d( _d )
    {
    }

    Reference::Reference( const Reference& other )
        : d( other.d )
    {
    }

    Reference::~Reference()
    {
    }


    Reference& Reference::operator=( const Reference& other )
    {
        d = other.d;
        return * this;
    }

    bool Reference::operator==(const Reference &other) const
    {
        if( d && other.d )
            return git_reference_cmp( d->mRef, other.d->mRef ) == 0;

        return false;
    }

    bool Reference::operator!=(const Reference &other) const
    {
        return !( *this == other );
    }

    bool Reference::isValid() const
    {
        return d;
    }

    QString Reference::name() const
    {
        if( !isValid() )
        {
            GitWrap::lastResult().setInvalidObject();
            return QString();
        }

        return QString::fromUtf8( git_reference_name( d->mRef ) );
    }

    QString Reference::shorthand() const
    {
        if ( !isValid() )
        {
            GitWrap::lastResult().setInvalidObject();
            return QString();
        }

        return QString::fromUtf8( git_reference_shorthand( d->mRef ) );
    }

    Reference::Type Reference::type( Result& result ) const
    {
        if( !result )
        {
            return Invalid;
        }

        if( !isValid() )
        {
            result.setInvalidObject();
            return Invalid;
        }

        switch( git_reference_type( d->mRef ) )
        {
        case GIT_REF_SYMBOLIC:  return Symbolic;
        case GIT_REF_OID:       return Direct;
        default:
        case GIT_REF_INVALID:   return Invalid;
        }
    }

    ObjectId Reference::objectId( Result& result ) const
    {
        if( !result )
        {
            return ObjectId();
        }

        if( !type( result ) == Direct )	// Does the isValid() check for us, no need to repeat it
        {
            return ObjectId();
        }

        return ObjectId::fromRaw( git_reference_target( d->mRef )->id );
    }

    QString Reference::target( Result& result ) const
    {
        if( !result )
        {
            return QString();
        }

        if( !type( result ) == Symbolic )   // Does the isValid() check for us, no need to repeat it
        {
            return QString();
        }
        return QString::fromUtf8( git_reference_symbolic_target( d->mRef ) );
    }

    Repository Reference::repository( Result& result ) const
    {
        if( !result )
        {
            return Repository();
        }

        if( !d )
        {
            return Repository();
        }

        return Repository( d->repo() );
    }

    Reference Reference::resolved( Result& result ) const
    {
        if( !result )
        {
            return Reference();
        }

        if( !d )
        {
            result.setInvalidObject();
            return Reference();
        }

        git_reference* ref;
        result = git_reference_resolve( &ref, d->mRef );
        if( !result )
        {
            return Reference();
        }

        return new Internal::ReferencePrivate( d->repo(), ref );
    }

    ObjectId Reference::resolveToObjectId( Result& result ) const
    {
        Reference resolvedRef = resolved( result );
        if( !result )
        {
            return ObjectId();
        }
        return resolvedRef.objectId( result );
    }

    bool Reference::isCurrentBranch() const
    {
        if ( !d ) return false;

        return git_branch_is_head( d->mRef );
    }

    bool Reference::isLocal() const
    {
        if ( !isValid() )
        {
            GitWrap::lastResult().setInvalidObject();
            return false;
        }

        return git_reference_is_branch( d->mRef );
    }

    bool Reference::isRemote() const
    {
        if ( !isValid() )
        {
            GitWrap::lastResult().setInvalidObject();
            return false;
        }

        return git_reference_is_remote( d->mRef );
    }

    void Reference::checkout(Result &result, bool force, bool updateHEAD,
                             const QStringList &paths) const
    {
        if (  !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        git_object *o = NULL;
        result = git_reference_peel( &o, d->mRef, GIT_OBJ_TREE );
        if ( !result ) return;

        git_checkout_opts opts = GIT_CHECKOUT_OPTS_INIT;
        opts.checkout_strategy = force ? GIT_CHECKOUT_FORCE : GIT_CHECKOUT_SAFE;
        if ( !paths.isEmpty() )
        {
            // TODO: don't copy, just map paths here
            result = git_strarray_copy( &opts.paths, Internal::StrArrayWrapper( paths ) );
            if ( !result ) return;
        }
        result = git_checkout_tree( d->repo()->mRepo, o, &opts );

        if ( updateHEAD )
            this->updateHEAD(result);
    }

    void Reference::destroy( Result& result )
    {
        if( !result || !isValid() ) return;

        result = git_reference_delete( d->mRef );
    }

    void Reference::move(Result &result, const ObjectCommit &target)
    {
        if ( !result ) return;

        if ( !isValid() )
        {
            result.setInvalidObject();
            return;
        }

        const ObjectId &targetId = target.id(result);
        if ( !result || targetId.isNull() ) return;

        git_oid *oid = NULL;
        git_oid_fromraw( oid, targetId.raw() );
        result = git_reference_set_target( &d->mRef, d->mRef, oid );
    }

    void Reference::rename(Result &result, const QString &newName, bool force)
    {
        if ( !result ) return;

        if ( !isValid() )
        {
            result.setInvalidObject();
            return;
        }

        result = git_reference_rename( &d->mRef, d->mRef, newName.toUtf8().constData(), force );
    }

    void Reference::updateHEAD(Result &result) const
    {
        if ( !result ) return;

        if ( !isValid() )
        {
            result.setInvalidObject();
            return;
        }

        if( git_reference_is_branch(d->mRef) )
        {
            // reference is a local branch
            result = git_repository_set_head(d->repo()->mRepo
                                             , git_reference_name(d->mRef) );
        }
        else
        {
            // reference is detached
            result = git_repository_set_head_detached( d->repo()->mRepo
                                                       , git_reference_target(d->mRef) );
        }
    }

}
