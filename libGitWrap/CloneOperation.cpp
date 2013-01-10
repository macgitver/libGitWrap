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

#include "CloneOperationPrivate.hpp"
#include "CloneOperation.hpp"

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

        CloneOperationPrivate::CloneOperationPrivate( CloneOperation* owner )
            : mOwner( owner )
        {
            mGitCloneOptions = (git_clone_options) GIT_CLONE_OPTIONS_INIT;

            mRemoteCallbacks                        = (git_remote_callbacks)
                                                      GIT_REMOTE_CALLBACKS_INIT;
            mRemoteCallbacks.completion             = &CloneOperationPrivate::remoteComplete;
            mRemoteCallbacks.progress               = &CloneOperationPrivate::remoteProgress;
            mRemoteCallbacks.update_tips            = &CloneOperationPrivate::remoteUpdateTips;
            mRemoteCallbacks.payload                = static_cast< IFetchEvents* >( mOwner );
            mGitCloneOptions.remote_callbacks       = &mRemoteCallbacks;

            mGitCloneOptions.fetch_progress_cb      = &CloneOperationPrivate::fetchProgress;

            mGitCloneOptions.cred_acquire_cb        = &CloneOperationPrivate::credAccquire;
            mGitCloneOptions.cred_acquire_payload   = static_cast< IFetchEvents* >( mOwner );
        }

        CloneOperationPrivate::~CloneOperationPrivate()
        {
        }

        int CloneOperationPrivate::credAccquire( git_cred** cred, const char* url,
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

        void CloneOperationPrivate::fetchProgress( const git_transfer_progress* stats, void* payload )
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
            }
        }

        int CloneOperationPrivate::remoteComplete( git_remote_completion_type type, void* payload )
        {
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

        void CloneOperationPrivate::remoteProgress( const char* str, int len, void* payload )
        {
            IFetchEvents* events = static_cast< IFetchEvents* >( payload );

            debugEvents( "Remote Progress: %s", QByteArray( str, len ).constData() );

            if( events )
            {
                events->remoteMessage( QString::fromUtf8( str, len ) );
            }
        }

        int CloneOperationPrivate::remoteUpdateTips( const char* refname, const git_oid* a,
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

        void CloneOperationPrivate::run()
        {
            git_repository* repo = NULL;
            mResult = git_clone( &repo,
                                 mUrl.toUtf8().constData(),
                                 mPath.toUtf8().constData(),
                                 &mGitCloneOptions );

            if( mResult && repo )
            {
                git_repository_free( repo );
            }
        }

    }

    CloneOperation::CloneOperation( QObject* parent )
        : QObject( parent )
    {
        d = new Internal::CloneOperationPrivate( this );
    }

    CloneOperation::~CloneOperation()
    {
        delete d;
    }

    void CloneOperation::setUrl( const QString& url )
    {
        d->mUrl = url;
    }

    void CloneOperation::setPath( const QString& path )
    {
        d->mPath = path;
    }

    void CloneOperation::setBare( bool bare )
    {
        d->mGitCloneOptions.bare = bare ? 1 : 0;
    }

    void CloneOperation::setRemoteName( const QByteArray& remoteName )
    {
        d->mRemoteName = remoteName;
        d->mGitCloneOptions.remote_name = remoteName.isEmpty() ? NULL : remoteName.constData();
    }

    void CloneOperation::setFetchSpec( const QByteArray& fetchSpec )
    {
        d->mFetchSpec = fetchSpec;
        d->mGitCloneOptions.fetch_spec = fetchSpec.isEmpty() ? NULL : fetchSpec.constData();
    }

    void CloneOperation::setPushSpec( const QByteArray& pushSpec )
    {
        d->mPushSpec = pushSpec;
        d->mGitCloneOptions.push_spec = pushSpec.isEmpty() ? NULL : pushSpec.constData();
    }

    void CloneOperation::setPushUrl( const QByteArray& pushUrl )
    {
        d->mPushUrl = pushUrl;
        d->mGitCloneOptions.pushurl = pushUrl.isEmpty() ? NULL : pushUrl.constData();
    }

    QString CloneOperation::url() const
    {
        return d->mUrl;
    }

    QString CloneOperation::path() const
    {
        return d->mPath;
    }

    bool CloneOperation::bare() const
    {
        return d->mGitCloneOptions.bare != 0;
    }

    QByteArray CloneOperation::remoteName() const
    {
        return d->mRemoteName;
    }

    QByteArray CloneOperation::fetchSpec() const
    {
        return d->mFetchSpec;
    }

    QByteArray CloneOperation::pushSpec() const
    {
        return d->mPushSpec;
    }

    QByteArray CloneOperation::pushUrl() const
    {
        return d->mPushUrl;
    }

    void CloneOperation::setBackgroundMode( bool backgroundMode )
    {
        d->mBackgroundMode = backgroundMode;
    }

    bool CloneOperation::backgroundMode() const
    {
        return d->mBackgroundMode;
    }

    Result CloneOperation::execute()
    {
        if( d->mBackgroundMode )
        {
            return Result();
        }
        else
        {
            d->run();
            return d->mResult;
        }
    }

    bool CloneOperation::isRunning() const
    {
        return false;
    }

}
