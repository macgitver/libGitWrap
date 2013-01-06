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

#include "CloneOptsPrivate.hpp"
#include "CloneOpts.hpp"

#if 0
#define debugEvents qDebug
#else
#define debugEvents if(0) qDebug
#endif

namespace Git
{

struct CredentialRequest{}; // temporary dummy

    namespace Internal
    {

        CloneOptsPrivate::CloneOptsPrivate()
        {
            mGitCloneOptions = (git_clone_options) GIT_CLONE_OPTIONS_INIT;

            mEvents = NULL;
        }

        CloneOptsPrivate::~CloneOptsPrivate()
        {
            setEvents( NULL );
        }

        const git_clone_options* CloneOptsPrivate::asGitCloneOptions() const
        {
            return &mGitCloneOptions;
        }

        void CloneOptsPrivate::setEvents( CloneEvents* e )
        {
            if( e != mEvents )
            {
                if( mEvents )
                {
                    mEvents->mOpts = NULL;
                    mEvents->disconnect();
                    mEvents->deleteLater();
                }
                else
                {
                    mRemoteCallbacks                        = (git_remote_callbacks)
                                                              GIT_REMOTE_CALLBACKS_INIT;
                    mRemoteCallbacks.completion             = &CloneOptsPrivate::remoteComplete;
                    mRemoteCallbacks.progress               = &CloneOptsPrivate::remoteProgress;
                    mRemoteCallbacks.update_tips            = &CloneOptsPrivate::remoteUpdateTips;
                    mRemoteCallbacks.payload                = this;
                    mGitCloneOptions.remote_callbacks       = &mRemoteCallbacks;

                    mGitCloneOptions.fetch_progress_cb      = &CloneOptsPrivate::fetchProgress;

                    mGitCloneOptions.cred_acquire_cb        = &CloneOptsPrivate::credAccquire;
                    mGitCloneOptions.cred_acquire_payload   = this;
                }

                mEvents = e;
                if( mEvents )
                {
                    mEvents->mOpts = this;
                }
            }
        }


        int CloneOptsPrivate::credAccquire( git_cred** cred, const char* url,
                                            unsigned int allowed_types, void* payload )
        {
            CloneOptsPrivate* that = static_cast< CloneOptsPrivate* >( payload );
            debugEvents( "credAccquire: %s %i", url, allowed_types );

            if( that->mEvents )
            {
                CredentialRequest request;
                that->mEvents->askCredentials( request );
            }

            return 0;
        }

        void CloneOptsPrivate::fetchProgress( const git_transfer_progress* stats, void* payload )
        {
            CloneOptsPrivate* that = static_cast< CloneOptsPrivate* >( payload );

            debugEvents( "fetchProgress: %u %u %u %lu",
                         stats->total_objects,
                         stats->received_objects,
                         stats->indexed_objects,
                         stats->received_bytes );

            if( that && that->mEvents )
            {
                that->mEvents->transportProgress( stats->total_objects, stats->indexed_objects,
                                                  stats->received_objects, stats->received_bytes );
            }
        }

        int CloneOptsPrivate::remoteComplete( git_remote_completion_type type, void* payload )
        {
            CloneOptsPrivate* that = static_cast< CloneOptsPrivate* >( payload );

            debugEvents( "fetchComplete: %i", type );

            if( that && that->mEvents )
            {
                switch( type )
                {
                case GIT_REMOTE_COMPLETION_DOWNLOAD:
                    that->mEvents->doneDownloading();
                    break;

                case GIT_REMOTE_COMPLETION_INDEXING:
                    that->mEvents->doneIndexing();
                    break;

                case GIT_REMOTE_COMPLETION_ERROR:
                    that->mEvents->error();
                    break;
                }
            }
            return 0;
        }

        void CloneOptsPrivate::remoteProgress( const char* str, int len, void* payload )
        {
            CloneOptsPrivate* that = static_cast< CloneOptsPrivate* >( payload );

            debugEvents( "Remote Progress: %s", QByteArray( str, len ).constData() );

            if( that && that->mEvents )
            {
                that->mEvents->remoteMessage( QString::fromUtf8( str, len ) );
            }
        }

        int CloneOptsPrivate::remoteUpdateTips( const char* refname, const git_oid* a,
                                                const git_oid* b, void* payload )
        {
            CloneOptsPrivate* that = static_cast< CloneOptsPrivate* >( payload );

            Git::ObjectId oidFrom = Git::ObjectId::fromRaw( a->id );
            Git::ObjectId oidTo   = Git::ObjectId::fromRaw( b->id );

            debugEvents( "Remote Update Tips: %s [%s->%s]",
                         refname,
                         oidFrom.toAscii().constData(),
                         oidTo.toAscii().constData() );

            if( that && that->mEvents )
            {
                that->mEvents->updateTip( QString::fromUtf8( refname ), oidFrom, oidTo );
            }

            return 0;
        }

    }

    CloneEvents::CloneEvents( QObject* parent )
        : QObject( parent )
    {
    }

    CloneOpts CloneEvents::options() const
    {
        return CloneOpts( mOpts );
    }

    CloneOpts::CloneOpts()
    {
        d = new Internal::CloneOptsPrivate;
    }

    CloneOpts::CloneOpts( Internal::CloneOptsPrivate* _d )
        : d( _d )
    {
    }

    CloneOpts::CloneOpts( const CloneOpts& other )
        : d( other.d )
    {
    }

    CloneOpts::~CloneOpts()
    {
    }

    CloneOpts& CloneOpts::operator=( const CloneOpts& other )
    {
        d = other.d;
        return *this;
    }

    bool CloneOpts::operator==( const CloneOpts& other ) const
    {
        return d == other.d || *d == *other.d;
    }

    void CloneOpts::setUrl( const QByteArray& url )
    {
        d->mUrl = url;
    }

    void CloneOpts::setPath( const QByteArray& path )
    {
        d->mPath = path;
    }

    void CloneOpts::setBare( bool bare )
    {
        d->mGitCloneOptions.bare = bare ? 1 : 0;
    }

    void CloneOpts::setRemoteName( const QByteArray& remoteName )
    {
        d->mRemoteName = remoteName;
        d->mGitCloneOptions.remote_name = remoteName.isEmpty() ? NULL : remoteName.constData();
    }

    void CloneOpts::setFetchSpec( const QByteArray& fetchSpec )
    {
        d->mFetchSpec = fetchSpec;
        d->mGitCloneOptions.fetch_spec = fetchSpec.isEmpty() ? NULL : fetchSpec.constData();
    }

    void CloneOpts::setPushSpec( const QByteArray& pushSpec )
    {
        d->mPushSpec = pushSpec;
        d->mGitCloneOptions.push_spec = pushSpec.isEmpty() ? NULL : pushSpec.constData();
    }

    void CloneOpts::setPushUrl( const QByteArray& pushUrl )
    {
        d->mPushUrl = pushUrl;
        d->mGitCloneOptions.pushurl = pushUrl.isEmpty() ? NULL : pushUrl.constData();
    }

    QByteArray CloneOpts::url() const
    {
        return d->mUrl;
    }

    QByteArray CloneOpts::path() const
    {
        return d->mPath;
    }

    bool CloneOpts::bare() const
    {
        return d->mGitCloneOptions.bare != 0;
    }

    QByteArray CloneOpts::remoteName() const
    {
        return d->mRemoteName;
    }

    QByteArray CloneOpts::fetchSpec() const
    {
        return d->mFetchSpec;
    }

    QByteArray CloneOpts::pushSpec() const
    {
        return d->mPushSpec;
    }

    QByteArray CloneOpts::pushUrl() const
    {
        return d->mPushUrl;
    }

    void CloneOpts::setEvents( CloneEvents* events )
    {
        d->setEvents( events );
    }

    CloneEvents* CloneOpts::events() const
    {
        if( d->mEvents == NULL )
        {
            Internal::CloneOptsPrivate* d2 = const_cast< Internal::CloneOptsPrivate* >( d.data() );
            d2->setEvents( new CloneEvents( NULL ) );
            return d2->mEvents;
        }
        return d->mEvents;
    }

}
