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

#include "libGitWrap/Repository.hpp"
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
            static int CB_CreateRemote( git_remote** out,
                                        git_repository* repo,
                                        const char* name,
                                        const char* url,
                                        void* payload );

            static Remote::PrivatePtr lookupRemote( Result& result,
                                                    Repository::Private* repo,
                                                    QString& remoteName );

        public:
            explicit BaseRemoteOperationPrivate(BaseRemoteOperation* owner );
            virtual ~BaseRemoteOperationPrivate();

        public:
            Repository      mRepo;

            QString         mRemoteAlias;
            QString         mRefLogMsg;
            QStringList     mRefSpecs;
            Signature       mSignature;

            git_remote_callbacks    mRemoteCallbacks;
        };


        class FetchOperationPrivate : public BaseRemoteOperationPrivate
        {
        public:
            explicit FetchOperationPrivate(FetchOperation* owner);

        public:
            void run();
        };


        class PushOperationPrivate : public BaseRemoteOperationPrivate
        {
        public:
            explicit PushOperationPrivate(PushOperation* owner);

        public:
            void run();

        public:
            git_push_options        mOpts;
        };

    }

}

#endif

