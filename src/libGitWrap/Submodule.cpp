/*
 * MacGitver
 * Copyright (C) 2012 Sascha Cunz <sascha@babbelbox.org>
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

#include "Submodule.h"
#include "ObjectId.h"
#include "Repository.h"
#include "RepositoryPrivate.h"

namespace Git
{

    static inline git_submodule* getSM( const Internal::RepositoryPrivate* repo, const QString& smName )
    {
        git_submodule* result = NULL;

        if( repo && repo->mRepo )
        {
            int rc = git_submodule_lookup( &result, repo->mRepo, smName.toUtf8().constData() );
            if( rc < 0 )
            {
                return NULL;
            }
        }

        return result;
    }

    Submodule::Submodule()
        : mOwnerRepo( NULL )
        , mMyRepo( NULL )
    {
    }

    Submodule::Submodule( Internal::RepositoryPrivate* repo, const QString& name )
        : mOwnerRepo( repo )
        , mMyRepo( NULL )
        , mName( name )
    {
    }

    Submodule::Submodule( const Submodule& other )
        : mOwnerRepo( other.mOwnerRepo )
        , mMyRepo( other.mMyRepo )
        , mName( other.mName )
    {
    }

    Submodule& Submodule::operator=( const Submodule& other )
    {
        mOwnerRepo = other.mOwnerRepo;
        mMyRepo = other.mMyRepo;
        mName = other.mName;
        return *this;
    }

    Submodule::~Submodule()
    {
    }

    bool Submodule::isValid()
    {
        return ( mOwnerRepo && !mName.isEmpty() );
    }

    QString Submodule::name() const
    {
        return mName;
    }

    QString Submodule::path() const
    {
        git_submodule* sm = getSM( mOwnerRepo, mName );
        if( !sm )
        {
            return QString();
        }
        const char* data = git_submodule_path( sm );
        return data ? QString::fromUtf8( data ) : QString();
    }

    QString Submodule::url() const
    {
        git_submodule* sm = getSM( mOwnerRepo, mName );
        if( !sm )
        {
            return QString();
        }
        const char* data = git_submodule_url( sm );
        return data ? QString::fromUtf8( data ) : QString();
    }

    bool Submodule::fetchRecursive() const
    {
        git_submodule* sm = getSM( mOwnerRepo, mName );
        if( !sm )
        {
            return git_submodule_fetch_recurse_submodules( sm );
        }
        return false;
    }

    Submodule::IgnoreStrategy Submodule::ignoreStrategy() const
    {
        git_submodule* sm = getSM( mOwnerRepo, mName );
        if( !sm )
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
        git_submodule* sm = getSM( mOwnerRepo, mName );
        if( !sm )
        {
            return Ignore;
        }
        switch( git_submodule_update( sm ) )
        {
        default:
        case GIT_SUBMODULE_UPDATE_CHECKOUT: return Checkout;
        case GIT_SUBMODULE_UPDATE_MERGE:    return Merge;
        case GIT_SUBMODULE_UPDATE_REBASE:   return Rebase;
            // libgit2 doesn't support None
        }
    }

    ObjectId Submodule::headOid() const
    {
        git_submodule* sm = getSM( mOwnerRepo, mName );
        if( !sm )
        {
            return ObjectId();
        }

        const git_oid* oid = git_submodule_head_oid( sm );
        if( !oid )
        {
            return ObjectId();
        }

        return ObjectId::fromRaw( oid->id );
    }

    ObjectId Submodule::indexOid() const
    {
        git_submodule* sm = getSM( mOwnerRepo, mName );
        if( !sm )
        {
            return ObjectId();
        }

        const git_oid* oid = git_submodule_index_oid( sm );
        if( !oid )
        {
            return ObjectId();
        }

        return ObjectId::fromRaw( oid->id );
    }

    ObjectId Submodule::wdOid() const
    {
        git_submodule* sm = getSM( mOwnerRepo, mName );
        if( !sm )
        {
            return ObjectId();
        }

        const git_oid* oid = git_submodule_wd_oid( sm );
        if( !oid )
        {
            return ObjectId();
        }

        return ObjectId::fromRaw( oid->id );
    }

    Repository Submodule::repository() const
    {
        return Repository( const_cast<Internal::RepositoryPrivate *>(*mMyRepo));
    }

    bool Submodule::open(Result &result GITWRAP_DEFAULT_TLSRESULT)
    {
        if (!result)
            return false;

        // already open?
        if (mMyRepo)
            return true;

        git_repository *submodule_repo = 0;
        result = git_submodule_open(&submodule_repo, getSM(mOwnerRepo, name()));
        if (!result)
            return false;

        mMyRepo = new Internal::RepositoryPrivate( submodule_repo );
        return true;
    }

    void Submodule::close()
    {
        if (mMyRepo)
            mMyRepo->deref();
    }

}
