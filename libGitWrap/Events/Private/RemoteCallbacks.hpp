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

#ifndef GITWRAP_REMOTE_CALLBACKS_HPP
#define GITWRAP_REMOTE_CALLBACKS_HPP
#pragma once

#include "libGitWrap/Private/GitWrapPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        struct RemoteCallbacks
        {

            static int credAccquire(
                    git_cred**                      cred,
                    const char*                     url,
                    const char*                     username_from_url,
                    unsigned int                    allowed_types,
                    void*                           payload);

            static int fetchProgress(
                    const git_transfer_progress*    stats,
                    void*                           payload);

            static int remoteComplete(
                    git_remote_completion_type      type,
                    void*                           payload);

            static int remoteProgress(
                    const char*                     str,
                    int                             len,
                    void*                           payload);

            static int remoteUpdateTips(
                    const char*                     refname,
                    const git_oid*                  a,
                    const git_oid*                  b,
                    void*                           payload);

            static void initCallbacks(
                    git_remote_callbacks&           cb,
                    IRemoteEvents*                  receiver);
        };

    }

}

#endif
