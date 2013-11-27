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

#include "libGitWrap/Operations/IFetchEvents.hpp"

#include "libGitWrap/Operations/Private/FetchCallbacks.hpp"

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

        int FetchCallbacks::credAccquire( git_cred** cred, const char* url,
                                          const char *username_from_url,
                                          unsigned int allowed_types, void* payload )
        {
            IFetchEvents* events = static_cast< IFetchEvents* >( payload );

            debugEvents( "credAccquire: %s %i", url, allowed_types );

            if( events )
            {
                CredentialRequest request;
                events->askCredentials( request );
            }

            return 0;
        }

        int FetchCallbacks::fetchProgress( const git_transfer_progress* stats, void* payload )
        {
            IFetchEvents* events = static_cast< IFetchEvents* >( payload );

            debugEvents( "fetchProgress: %u %u %u %lu",
                         stats->total_objects,
                         stats->received_objects,
                         stats->indexed_objects,
                         stats->received_bytes );

            if( events )
            {
                events->transportProgress( stats->total_objects, stats->indexed_objects,
                                           stats->received_objects, stats->received_bytes );

                if( stats->received_objects == stats->total_objects )
                {
                    events->doneDownloading();
                }

                if( stats->indexed_objects == stats->total_objects )
                {
                    events->doneIndexing();
                }
            }

            return 0;
        }

        int FetchCallbacks::remoteComplete( git_remote_completion_type type, void* payload )
        {
            /* THIS event is not triggered by lg anywhere! */

            qDebug( "**********************************************************\n"
                    "libgit2 started triggering completion events from remotes!\n"
                    "**********************************************************\n"
                    "\n\n\n");
            return 0;

            IFetchEvents* events = static_cast< IFetchEvents* >( payload );

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

        int FetchCallbacks::remoteProgress( const char* str, int len, void* payload )
        {
            IFetchEvents* events = static_cast< IFetchEvents* >( payload );

            debugEvents( "Remote Progress: %s", QByteArray( str, len ).constData() );

            if( events )
            {
                events->remoteMessage( QString::fromUtf8( str, len ) );
            }

            return GITERR_NONE;
        }

        int FetchCallbacks::remoteUpdateTips( const char* refname, const git_oid* a,
                                              const git_oid* b, void* payload )
        {
            IFetchEvents* events = static_cast< IFetchEvents* >( payload );

            Git::ObjectId oidFrom = Git::ObjectId::fromRaw( a->id );
            Git::ObjectId oidTo   = Git::ObjectId::fromRaw( b->id );

            debugEvents( "Remote Update Tips: %s [%s->%s]",
                         refname,
                         oidFrom.toAscii().constData(),
                         oidTo.toAscii().constData() );

            if( events )
            {
                events->updateTip( QString::fromUtf8( refname ), oidFrom, oidTo );
            }

            return 0;
        }

    }

}
