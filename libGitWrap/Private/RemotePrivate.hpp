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

#ifndef GIT_REMOTE_PRIVATE_H
#define GIT_REMOTE_PRIVATE_H

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/RepoObjectPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        /**
         * @internal
         * @ingroup GitWrap
         * @brief The RemotePrivate class
         *
         */
        class RemotePrivate : public RepoObjectPrivate
        {
        public:
            RemotePrivate(RepositoryPrivate* repo, git_remote* remote);
            ~RemotePrivate();

        public:
            git_remote*         mRemote;
            git_off_t           mBytes;
            git_transfer_progress mStats;
        };

    }

}

#endif
