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

#include "libGitWrap/RepoObject.hpp"
#include "libGitWrap/Repository.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/RepoObjectPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        RepoObjectPrivate::RepoObjectPrivate(const RepositoryPrivate::Ptr& repo)
            : BasePrivate()
            , mRepo(repo)
        {
        }

        Repository::PrivatePtr RepoObjectPrivate::repo() const
        {
            return mRepo;
        }

    }

    RepoObject::RepoObject()
    {
    }

    RepoObject::RepoObject(const PrivatePtr& _d)
        : Base(_d)
    {
    }

    bool RepoObject::operator==(const RepoObject& other) const
    {
        return Base::operator==(other);
    }

    Repository RepoObject::repository(Result &result) const
    {
        return repository();
    }

    Repository RepoObject::repository() const
    {
        GW_CD(RepoObject);
        return d->repo();
    }

}
