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

#include "libGitWrap/Events/IRemoteEvents.hpp"

#include "libGitWrap/Events/Private/RemoteCallbacks.hpp"

#if 0
#define debugEvents qDebug
#else
#define debugEvents if(0) qDebug
#endif

namespace Git
{
    class CredentialRequest{}; // temporary dummy

    namespace Internal
    {

        int RemoteCallbacks::credAccquire(git_cred** cred, const char* url,
                                          const char *username_from_url, unsigned int allowed_types,
                                          void* payload)
        {
            IRemoteEvents* events = static_cast< IRemoteEvents* >( payload );

            debugEvents( "credAccquire: %s %i", url, allowed_types );

            if( events )
            {
                CredentialRequest request;
                events->askCredentials( request );
            }

            return 0;
        }

        int RemoteCallbacks::fetchProgress(const git_transfer_progress* stats, void* payload)
        {
            IRemoteEvents* events = static_cast< IRemoteEvents* >(payload);

            debugEvents("fetchProgress: %u %u %u %lu",
                        stats->total_objects,
                        stats->received_objects,
                        stats->indexed_objects,
                        stats->received_bytes);

            if (events) {

                events->transportProgress(stats->total_objects, stats->indexed_objects,
                                          stats->received_objects, stats->received_bytes);

                if (stats->received_objects == stats->total_objects) {
                    events->doneDownloading();
                }

                if (stats->indexed_objects == stats->total_objects) {
                    events->doneIndexing();
                }

            }

            return 0;
        }

        int RemoteCallbacks::remoteComplete(git_remote_completion_type type, void* payload)
        {
            /* THIS event is not triggered by lg anywhere! */

            qDebug( "**********************************************************\n"
                    "libgit2 started triggering completion events from remotes!\n"
                    "**********************************************************\n"
                    "\n\n\n");
            return 0;

            IRemoteEvents* events = static_cast< IRemoteEvents* >( payload );

            debugEvents( "fetchComplete: %i", type );

            if( events )
            {
                switch( type )
                {
                case GIT_REMOTE_COMPLETION_DOWNLOAD:
                    events->doneDownloading();
                    break;

                case GIT_REMOTE_COMPLETION_INDEXING:
                    events->doneIndexing();
                    break;

                case GIT_REMOTE_COMPLETION_ERROR:
                    events->error();
                    break;
                }
            }
            return 0;
        }

        int RemoteCallbacks::remoteProgress( const char* str, int len, void* payload )
        {
            IRemoteEvents* events = static_cast< IRemoteEvents* >( payload );

            debugEvents( "Remote Progress: %s", QByteArray( str, len ).constData() );

            if (events) {
                events->remoteMessage(GW_StringToQt(str, len));
            }

            return GITERR_NONE;
        }

        int RemoteCallbacks::remoteUpdateTips(const char* refname, const git_oid* a,
                                              const git_oid* b, void* payload)
        {
            IRemoteEvents* events = static_cast< IRemoteEvents* >(payload);

            Git::ObjectId oidFrom = Git::ObjectId::fromRaw(a->id);
            Git::ObjectId oidTo   = Git::ObjectId::fromRaw(b->id);

            debugEvents("Remote Update Tips: %s [%s->%s]",
                        refname,
                        oidFrom.toAscii().constData(),
                        oidTo.toAscii().constData());

            if (events) {
                events->updateTip(GW_StringToQt(refname), oidFrom, oidTo);
            }

            return 0;
        }

        void RemoteCallbacks::initCallbacks(git_remote_callbacks& cb, IRemoteEvents* receiver)
        {
            git_remote_init_callbacks( &cb, GIT_REMOTE_CALLBACKS_VERSION );

            cb.sideband_progress   = &RemoteCallbacks::remoteProgress;
            cb.completion          = &RemoteCallbacks::remoteComplete;
            cb.update_tips         = &RemoteCallbacks::remoteUpdateTips;
            cb.credentials         = &RemoteCallbacks::credAccquire;
            cb.payload             = receiver;
        }

    }

}
