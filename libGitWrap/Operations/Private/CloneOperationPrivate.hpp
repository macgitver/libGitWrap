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

#include "libGitWrap/Result.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"

#include "libGitWrap/Operations/Private/BaseOperationPrivate.hpp"

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
        class CloneOperationPrivate : public BaseOperationPrivate
        {
        public:
            CloneOperationPrivate(CloneOperation* owner);
            ~CloneOperationPrivate();

        public:
            void run();

        public:
            QString                 mUrl;
            QString                 mPath;

            QByteArray              mRemoteName;
            QByteArray              mFetchSpec;
            QByteArray              mPushSpec;
            QByteArray              mPushUrl;

            CloneOptions            mCloneOpts;
        };

    }

}
