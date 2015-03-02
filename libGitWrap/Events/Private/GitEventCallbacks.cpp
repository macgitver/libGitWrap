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

#include "libGitWrap/Events/IGitEvents.hpp"

#include "libGitWrap/DiffList.hpp"

#include "libGitWrap/Events/Private/GitEventCallbacks.hpp"
#include "libGitWrap/Private/DiffPrivate.hpp"

#if 0
#define debugEvents qDebug
#else
#define debugEvents if(0) qDebug
#endif

namespace Git
{
    class CredentialRequest{}; // temporary dummy

    class CheckoutNotify
    {
    public:
        enum Notify {
            NotifyNone        = GIT_CHECKOUT_NOTIFY_NONE      ,
            NotifyConflict    = GIT_CHECKOUT_NOTIFY_CONFLICT  ,
            NotifyDirty       = GIT_CHECKOUT_NOTIFY_DIRTY     ,
            NotifyUpdated     = GIT_CHECKOUT_NOTIFY_UPDATED   ,
            NotifyUntracked   = GIT_CHECKOUT_NOTIFY_UNTRACKED ,
            NotifyIgnored     = GIT_CHECKOUT_NOTIFY_IGNORED   ,

            NotifyAll         = GIT_CHECKOUT_NOTIFY_ALL
        };

    public:
        explicit CheckoutNotify( git_checkout_notify_t why )
            : mWhy( static_cast<Notify>( why ) )
        {
        }

    private:
        Notify  mWhy;
    };

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

            debugEvents("fetch progress: %u %u %u %lu",
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

            debugEvents( "remote progress: %s", QByteArray( str, len ).constData() );

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

            debugEvents("remote update tips: %s [%s->%s]",
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
            Result r;
            r = git_remote_init_callbacks( &cb, GIT_REMOTE_CALLBACKS_VERSION );
            Q_ASSERT_X( r, "git_remote_init_callbacks", qUtf8Printable(r.errorText()) );
            GW_CHECK_RESULT( r, void() );

            cb.sideband_progress   = RemoteCallbacks::remoteProgress;
            cb.transfer_progress   = RemoteCallbacks::fetchProgress;
            cb.completion          = RemoteCallbacks::remoteComplete;
            cb.update_tips         = RemoteCallbacks::remoteUpdateTips;
            cb.credentials         = RemoteCallbacks::credAccquire;

            cb.payload             = receiver;
        }


        // -- CheckoutCallbacks ->8

        int CheckoutCallbacks::notify( git_checkout_notify_t why,
                                       const char* path,
                                       const git_diff_file* baseline,
                                       const git_diff_file* target,
                                       const git_diff_file* workdir,
                                       void* payload )
        {
            ICheckoutEvents* events = static_cast< ICheckoutEvents* >( payload );

            if ( events ) {
                events->checkoutNotify( CheckoutNotify( why ),
                                        GW_StringToQt( path ),
                                        new DiffFilePrivate( baseline ),
                                        new DiffFilePrivate( target ),
                                        new DiffFilePrivate( workdir )
                                        );
            }

            return GITERR_NONE;
        }

        void CheckoutCallbacks::checkoutProgress(const char* path, size_t completed_steps,
                                                 size_t total_steps, void* payload)
        {
            ICheckoutEvents* events = static_cast< ICheckoutEvents* >( payload );
            Q_ASSERT( events );

            debugEvents( "checkout progress: %lu / %lu", completed_steps, total_steps );

            if (events) {
                events->checkoutProgress( GW_StringToQt(path), total_steps, completed_steps );

                if ( completed_steps == total_steps )
                {
                    events->doneCheckout();
                }
            }
        }

        void CheckoutCallbacks::initCallbacks( git_checkout_options& opts,
                                               ICheckoutEvents* receiver,
                                               unsigned int notifyFlags )
        {
            opts.notify_cb          = CheckoutCallbacks::notify;
            opts.notify_payload     = receiver;

            opts.progress_cb        = CheckoutCallbacks::checkoutProgress;
            opts.progress_payload   = receiver;

            opts.notify_flags       = notifyFlags;
        }


        // -- DiffCallbacks --8>

        int DiffCallbacks::notify(const git_diff* diff_so_far,
                                  const git_diff_delta* delta_to_add,
                                  const char* matched_pathspec, void* payload)
        {
            IDiffEvents* events = static_cast< IDiffEvents* >( payload );
            Q_ASSERT( events );

            if (events) {
                //RepositoryPrivate* rp = new RepositoryPrivate( diff_so_far->repo );
                //events->diffNotify( new DiffListPrivate( rp, diff ) );
            }

            return 0;
        }

        void DiffCallbacks::initCallbacks(git_diff_options& opts, IDiffEvents* receiver)
        {
            opts.notify_cb      = DiffCallbacks::notify;
            opts.notify_payload = receiver;
        }

    }

}
