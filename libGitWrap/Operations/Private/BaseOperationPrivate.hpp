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

#ifndef GITWRAP_OPS_BASEOP_PRIVATE_HPP
#define GITWRAP_OPS_BASEOP_PRIVATE_HPP
#pragma once

#include "libGitWrap/Result.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"

#include "libGitWrap/Operations/Private/WorkerThread.hpp"

namespace Git
{

    class BaseOperation;

    namespace Internal
    {

        /**
         * @internal
         * @ingroup     GitWrap
         *
         */
        class BaseOperationPrivate : public Worker
        {
        public:
            BaseOperationPrivate(BaseOperation* owner);
            ~BaseOperationPrivate();

        public:
            BaseOperation*          mOwner;
            bool                    mBackgroundMode;

            Result                  mResult;
            WorkerThread*           mThread;
        };

    }

}

#define GW_OP_OWNER(CLS) \
    CLS* owner = static_cast<CLS*>(mOwner)

#endif
