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

#ifndef GITWRAP_GIT_EVENT_CALLBACKS_HPP
#define GITWRAP_GIT_EVENT_CALLBACKS_HPP
#pragma once

#include "libGitWrap/Private/GitWrapPrivate.hpp"

namespace Git
{

    class IRemoteEvents;
    class ICheckoutEvents;

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

        struct CheckoutCallbacks
        {
            static int notify(
                    git_checkout_notify_t why,
                    const char *path,
                    const git_diff_file *baseline,
                    const git_diff_file *target,
                    const git_diff_file *workdir,
                    void *payload);

            static void checkoutProgress(
                    const char *path,
                    size_t completed_steps,
                    size_t total_steps,
                    void *payload);

            static void initCallbacks(
                    git_checkout_options& opts,
                    ICheckoutEvents* receiver,
                    unsigned int notifyFlags = GIT_CHECKOUT_NOTIFY_ALL
                    );
        };
    }

}

#endif
