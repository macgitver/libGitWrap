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

#include "libGitWrap/Submodule.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Repository.hpp"

#include "libGitWrap/Private/RepositoryPrivate.hpp"
#include "libGitWrap/Private/SubmodulePrivate.hpp"

namespace Git
{

    namespace Internal
    {

        /**
         * @internal
         * @ingroup GitWrap
         *
         * @brief convertSubmoduleStatus
         * Converts an unsigned int value to the File::StatusFlags enum.
         *
         * @param v     the value to convert
         *
         * @return      the converted flags
         */
        static Git::StatusFlags convertSubmoduleStatus( unsigned int v )
        {
            Git::StatusFlags s = Git::FileInvalidStatus;

            if ( v & GIT_SUBMODULE_STATUS_INDEX_ADDED )         s |= Git::SubmoduleIndexNew;
            if ( v & GIT_SUBMODULE_STATUS_INDEX_DELETED )       s |= Git::SubmoduleIndexDeleted;
            if ( v & GIT_SUBMODULE_STATUS_INDEX_MODIFIED )      s |= Git::SubmoduleIndexModified;
            if ( v & GIT_SUBMODULE_STATUS_WD_ADDED )            s |= Git::SubmoduleWorkingTreeNew;
            if ( v & GIT_SUBMODULE_STATUS_WD_DELETED )          s |= Git::SubmoduleWorkingTreeDeleted;
            if ( v & GIT_SUBMODULE_STATUS_WD_MODIFIED )         s |= Git::SubmoduleWorkingTreeModified;

            if ( v & GIT_SUBMODULE_STATUS_IN_HEAD )             s |= Git::SubmoduleInHead;
            if ( v & GIT_SUBMODULE_STATUS_IN_INDEX )            s |= Git::SubmoduleInIndex;
            if ( v & GIT_SUBMODULE_STATUS_IN_CONFIG )           s |= Git::SubmoduleInConfig;
            if ( v & GIT_SUBMODULE_STATUS_IN_WD )               s |= Git::SubmoduleInWorkingTree;
            if ( v & GIT_SUBMODULE_STATUS_WD_UNINITIALIZED )    s |= Git::SubmoduleWorkingTreeUninitialized;
            if ( v & GIT_SUBMODULE_STATUS_WD_INDEX_MODIFIED )   s |= Git::SubmoduleWorkingTreeIndexModified;
            if ( v & GIT_SUBMODULE_STATUS_WD_WD_MODIFIED )      s |= Git::SubmoduleWorkingTreeWtModified;
            if ( v & GIT_SUBMODULE_STATUS_WD_UNTRACKED )        s |= Git::SubmoduleWorkingTreeUntracked;

            return s;
        }

        SubmodulePrivate::SubmodulePrivate(const Repository::PrivatePtr& repo, const QString& name)
            : RepoObjectPrivate(repo)
            , mName(name)
            , mSubRepo(NULL)
        {
        }

        git_submodule* SubmodulePrivate::getSM(Result& rc) const
        {
            git_submodule* sm = NULL;

            if (rc && repo() && !mName.isEmpty()) {
                rc = git_submodule_lookup(&sm, repo()->mRepo, GW_StringFromQt(mName));
                if (!rc) {
                    return NULL;
                }
            }

            return sm;
        }

        bool SubmodulePrivate::open(Result& result)
        {
            if (!result) {
                return false;
            }

            // already open?
            if (mSubRepo) {
                return true;
            }

            git_repository* submodule_repo = NULL;
            result = git_submodule_open(&submodule_repo, getSM(result));
            if (!result) {
                return false;
            }

            mSubRepo = new Repository::Private(submodule_repo);
            return true;
        }

    }

    GW_PRIVATE_IMPL(Submodule, RepoObject)


    QString Submodule::name() const
    {
        GW_CD(Submodule);
        return d ? d->mName : QString();
    }

    QString Submodule::path( Result& r ) const
    {
        GW_CD(Submodule);
        git_submodule* sm = d ? d->getSM( r ) : NULL;

        if( !r || !sm )
        {
            return QString();
        }

        const char* data = git_submodule_path( sm );
        return data ? GW_StringToQt( data ) : QString();
    }

    QString Submodule::url( Result& r ) const
    {
        GW_CD(Submodule);
        git_submodule* sm = d ? d->getSM( r ) : NULL;

        if( !r || !sm )
        {
            return QString();
        }

        const char* data = git_submodule_url( sm );
        return data ? GW_StringToQt( data ) : QString();
    }

    bool Submodule::fetchRecursive() const
    {
        GW_CD(Submodule);
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
        GW_CD(Submodule);
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
        GW_CD(Submodule);
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
        GW_CD(Submodule);
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
        GW_CD(Submodule);
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
        GW_CD(Submodule);
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

    /**
     * @brief           Get a Repository object for the submodule
     *
     * This method tries to open the submodule as a Repository and returns it. Subsequent calls will
     * return the same Repository object until that goes totally out of scope.
     *
     * @param[in,out]	result  A Result object; see @ref GitWrapErrorHandling
     *
     * @return          An existing or new Repository object for the submodule. An invalid
     *                  Repository object if the submodule cannot be opened.
     *
     */
    Repository Submodule::subRepository(Result& result)
    {
        GW_D_CHECKED(Submodule, Repository(), result);

        if (!d->mSubRepo) {
            if (!d->open(result)) {
                return Repository();
            }
        }

        return d->mSubRepo;
    }

    StatusFlags Submodule::status(Result &result) const
    {
        GW_CD(Submodule);
        if ( !result || !d )
        {
            return FileInvalidStatus;
        }

        unsigned int status = GIT_SUBMODULE_STATUS_IN_HEAD;
        result = git_submodule_status( &status, d->getSM( result ) );

        return Internal::convertSubmoduleStatus( status );
    }

}
