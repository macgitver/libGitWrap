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

#include "RevisionWalker.hpp"
#include "RevisionWalkerPrivate.hpp"
#include "ObjectId.hpp"
#include "Reference.hpp"

namespace Git
{

    namespace Internal
    {

        RevisionWalkerPrivate::RevisionWalkerPrivate( const GitPtr< RepositoryPrivate >& repo,
                                                      git_revwalk* walker )
            : RepoObject( repo )
            , mWalker( walker )
        {
            Q_ASSERT( walker );
        }

        RevisionWalkerPrivate::~RevisionWalkerPrivate()
        {
            git_revwalk_free( mWalker );
        }

    }

    RevisionWalker::RevisionWalker()
    {
    }

    RevisionWalker::RevisionWalker( Internal::RevisionWalkerPrivate* _d )
        : d( _d )
    {
    }

    RevisionWalker::~RevisionWalker()
    {
    }

    RevisionWalker& RevisionWalker::operator=( const RevisionWalker& other )
    {
        d = other.d;
        return * this;
    }

    bool RevisionWalker::isValid() const
    {
        return d;
    }

    void RevisionWalker::reset( Result& result )
    {
        if( !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        git_revwalk_reset( d->mWalker );
    }

    void RevisionWalker::push(Result& result, const ObjectId& id)
    {
        if( !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        result = git_revwalk_push( d->mWalker, (const git_oid*) id.raw() );
    }

    void RevisionWalker::push(Result& result, const Reference& ref)
    {

        if( !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        pushRef( ref.name(), result );
    }

    void RevisionWalker::pushRef(Result& result, const QString& name)
    {
        if( !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        result = git_revwalk_push_ref( d->mWalker, name.toUtf8().constData() );
    }

    void RevisionWalker::pushHead( Result& result )
    {
        if( !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        result = git_revwalk_push_head( d->mWalker );
    }

    void RevisionWalker::hide( Result& result, const ObjectId& id )
    {
        if( !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        result = git_revwalk_hide( d->mWalker, (const git_oid*) id.raw() );
    }

    void RevisionWalker::hide(Result& result, const Reference& ref)
    {
        if( !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        hideRef( ref.name(), result );
    }

    void RevisionWalker::hideRef(Result& result, const QString& name)
    {
        if( !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        result = git_revwalk_hide_ref( d->mWalker, name.toUtf8().constData() );
    }

    void RevisionWalker::hideHead( Result& result )
    {
        if( !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        result = git_revwalk_hide_head( d->mWalker );
    }

    bool RevisionWalker::next(Result& result, ObjectId& oidNext)
    {
        if( !result )
        {
            return false;
        }

        if( !d )
        {
            result.setInvalidObject();
            return false;
        }

        git_oid oid;
        Result tmp( git_revwalk_next( &oid, d->mWalker ) );
        if( tmp.errorCode() == GIT_ITEROVER )
        {
            return false;
        }
        if( !tmp )
        {
            result = tmp;
            return false;
        }

        oidNext = ObjectId::fromRaw( oid.id );
        return true;
    }

    QVector< ObjectId > RevisionWalker::all( Result& result )
    {
        QVector< ObjectId > ids;

        if( !result )
        {
            return ids;
        }

        if( !d )
        {
            return ids;
        }

        ObjectId id;
        while( next( id, result ) )
        {
            if( !result )
            {
                return QVector< ObjectId >();
            }
            ids.append( id );
        }

        return ids;
    }

    void RevisionWalker::setSorting(Result& result, bool topological, bool timed)
    {
        if( !result )
        {
            return;
        }

        if( !d )
        {
            return;
        }

        git_revwalk_sorting( d->mWalker,
                             ( topological ? GIT_SORT_TOPOLOGICAL : 0 ) |
                             ( timed ? GIT_SORT_TIME : 0 ) );
    }

}

