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

#include "Submodule.hpp"
#include "ObjectId.hpp"
#include "Repository.hpp"
#include "RepositoryPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        class SubmodulePrivate : public BasicObject
        {
        public:
            GitPtr< RepositoryPrivate > mOwnerRepo;
            GitPtr< RepositoryPrivate > mMyRepo;
            QString mName;

        public:
            inline git_submodule* getSM( Result& rc ) const
            {
                git_submodule* sm = NULL;

                if( rc && mOwnerRepo && !mName.isEmpty() )
                {
                    rc = git_submodule_lookup( &sm, mOwnerRepo->mRepo,
                                               mName.toUtf8().constData() );
                    if( !rc )
                    {
                        return NULL;
                    }
                }

                return sm;
            }
        };

    }

    Submodule::Submodule()
        : d( NULL )
    {
    }

    Submodule::Submodule( const Internal::GitPtr< Internal::RepositoryPrivate >& repo,
                          const QString& name )
    {
        d = new Internal::SubmodulePrivate;
        d->mOwnerRepo = repo;
        d->mName = name;
    }

    Submodule::Submodule( const Submodule& other )
        : d( other.d )
    {
    }

    Submodule& Submodule::operator=( const Submodule& other )
    {
        d = other.d;
        return *this;
    }

    Submodule::~Submodule()
    {
    }

    bool Submodule::isValid()
    {
        Result r;
        return d && ( d->getSM( r ) != NULL ) && r;
    }

    QString Submodule::name() const
    {
        return d ? d->mName : QString();
    }

    QString Submodule::path( Result& r ) const
    {
        git_submodule* sm = d ? d->getSM( r ) : NULL;

        if( !r || !sm )
        {
            return QString();
        }

        const char* data = git_submodule_path( sm );
        return data ? QString::fromUtf8( data ) : QString();
    }

    QString Submodule::url( Result& r ) const
    {
        git_submodule* sm = d ? d->getSM( r ) : NULL;

        if( !r || !sm )
        {
            return QString();
        }

        const char* data = git_submodule_url( sm );
        return data ? QString::fromUtf8( data ) : QString();
    }

    bool Submodule::fetchRecursive() const
    {
        Result r;
        git_submodule* sm = d ? d->getSM( r ) : NULL;

        if( !r || !sm )
        {
            return git_submodule_fetch_recurse_submodules( sm );
        }

        return false;
    }

    Submodule::IgnoreStrategy Submodule::ignoreStrategy() const
    {
        Result r;
        git_submodule* sm = d ? d->getSM( r ) : NULL;

        if( !r || !sm )
        {
            return None;
        }

        switch( git_submodule_ignore( sm ) )
        {
        case GIT_SUBMODULE_IGNORE_ALL:          return All;
        case GIT_SUBMODULE_IGNORE_DIRTY:        return Dirty;
        case GIT_SUBMODULE_IGNORE_UNTRACKED:    return Untracked;
        case GIT_SUBMODULE_IGNORE_NONE:
        default:                                return None;
        }
    }

    Submodule::UpdateStrategy Submodule::updateStrategy() const
    {
        Result r;
        git_submodule* sm = d ? d->getSM( r ) : NULL;

        if( !r || !sm )
        {
            return Ignore;
        }

        switch( git_submodule_update( sm ) )
        {
        default:
        case GIT_SUBMODULE_UPDATE_CHECKOUT: return Checkout;
        case GIT_SUBMODULE_UPDATE_MERGE:    return Merge;
        case GIT_SUBMODULE_UPDATE_REBASE:   return Rebase;
        case GIT_SUBMODULE_UPDATE_NONE:     return Ignore;
        }
    }

    ObjectId Submodule::headOid() const
    {
        Result r;
        git_submodule* sm = d ? d->getSM( r ) : NULL;

        if( !r || !sm )
        {
            return ObjectId();
        }

        const git_oid* oid = git_submodule_head_id( sm );
        if( !oid )
        {
            return ObjectId();
        }

        return ObjectId::fromRaw( oid->id );
    }

    ObjectId Submodule::indexOid() const
    {
        Result r;
        git_submodule* sm = d ? d->getSM( r ) : NULL;

        if( !r || !sm )
        {
            return ObjectId();
        }

        const git_oid* oid = git_submodule_index_id( sm );
        if( !oid )
        {
            return ObjectId();
        }

        return ObjectId::fromRaw( oid->id );
    }

    ObjectId Submodule::wdOid() const
    {
        Result r;
        git_submodule* sm = d ? d->getSM( r ) : NULL;

        if( !r || !sm )
        {
            return ObjectId();
        }

        const git_oid* oid = git_submodule_wd_id( sm );
        if( !oid )
        {
            return ObjectId();
        }

        return ObjectId::fromRaw( oid->id );
    }

    Repository Submodule::repository() const
    {
        return Repository( d->mMyRepo );
    }

    bool Submodule::isOpened() const
    {
        return d && d->mMyRepo;
    }

    Submodule::Status Submodule::status(Result &result) const
    {
        if ( !result || !d )
        {
            return InHead;
        }

        unsigned int status = GIT_SUBMODULE_STATUS_IN_HEAD;
        result = git_submodule_status( &status, d->getSM( result ) );

        return static_cast< Submodule::Status >( status );
    }

    bool Submodule::open( Result& result )
    {
        if( !result || !d )
            return false;

        // already open?
        if( d->mMyRepo )
            return true;

        git_repository *submodule_repo = 0;
        result = git_submodule_open(&submodule_repo, d->getSM( result ) );
        if (!result)
            return false;

        d->mMyRepo = new Internal::RepositoryPrivate( submodule_repo );
        return true;
    }

    void Submodule::close()
    {
        d->mMyRepo = NULL;
    }

}
