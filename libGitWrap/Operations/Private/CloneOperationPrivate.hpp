/*
 * MacGitver
 * Copyright (C) 2014 Sascha Cunz <sascha@macgitver.org>
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

#include "libGitWrap/Result.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"

#include "libGitWrap/Operations/Private/RemoteOperationsPrivate.hpp"

namespace Git
{

    class CloneOperation;

    namespace Internal
    {

        /**
         * @internal
         * @ingroup     GitWrap
         *
         */
        class CloneOperationPrivate : public BaseRemoteOperationPrivate
        {
        private:
            static int CB_CreateRepository( git_repository** out, const char* path, int bare, void* payload );

        public:
            CloneOperationPrivate(CloneOperation* owner);

        public:
            void run();

        public:
            RepositoryPrivate::Ptr  mClonedRepo;

            QString                 mUrl;
            QString                 mPath;

            CloneOptions            mCloneOpts;
        };

    }

}
