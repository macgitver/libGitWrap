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

#include "GitWrapPrivate.hpp"
#include "BasicObject.hpp"

namespace Git
{

    class CloneEvents;

    namespace Internal
    {

        /**
         * @internal
         * @ingroup     GitWrap
         *
         */
        class CloneOptsPrivate : public BasicObject
        {
        public:
            CloneOptsPrivate();
            ~CloneOptsPrivate();

        public:
            void setEvents( CloneEvents* events );

        public:
            const git_clone_options* asGitCloneOptions() const;

        public:
            QString                 mUrl;
            QString                 mPath;
            QByteArray              mRemoteName;
            QByteArray              mFetchSpec;
            QByteArray              mPushSpec;
            QByteArray              mPushUrl;
            git_clone_options       mGitCloneOptions;
            git_remote_callbacks    mRemoteCallbacks;
            CloneEvents*            mEvents;

        private:
            static int  credAccquire(git_cred** cred, const char* url, const char *username_from_url,
                                      unsigned int allowed_types, void* payload );
            static int  fetchProgress( const git_transfer_progress* stats, void* payload );
            static int  remoteComplete( git_remote_completion_type type, void* payload );
            static void remoteProgress( const char* str, int len, void* payload );
            static int  remoteUpdateTips( const char* refname, const git_oid* a, const git_oid* b,
                                          void* payload );
        };

    }


}
