/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2014 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Nils Fenner <nils@macgitver.org>
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

#ifndef GITWRAP_REMOTE_OPERATIONS_PRIVATE_HPP
#define GITWRAP_REMOTE_OPERATIONS_PRIVATE_HPP

#include "libGitWrap/Operations/Private/BaseOperationPrivate.hpp"

#include "libGitWrap/Private/RemotePrivate.hpp"
#include "libGitWrap/Signature.hpp"

namespace Git
{

    class FetchOperation;
    class PushOperation;

    namespace Internal
    {
        class BaseRemoteOperationPrivate : public BaseOperationPrivate
        {
        public:
            explicit BaseRemoteOperationPrivate(git_remote_callbacks& callbacks, BaseRemoteOperation* owner );
            virtual ~BaseRemoteOperationPrivate();

        public:
            Remote::PrivatePtr      mRemote;
            QString                 mRefLogMsg;
            QStringList             mRefSpecs;
            Signature               mSignature;
        };


        class FetchOperationPrivate : public BaseRemoteOperationPrivate
        {
        public:
            explicit FetchOperationPrivate(FetchOperation* owner);

        public:
            void run();

        public:
            git_remote_callbacks    mRemoteCallbacks;
        };


        class PushOperationPrivate : public BaseRemoteOperationPrivate
        {
        public:
            explicit PushOperationPrivate(PushOperation* owner);

        public:
            void run();

        public:
            git_remote_callbacks    mRemoteCallbacks;

            git_push_options        mOpts;
        };

    }

}

#endif

