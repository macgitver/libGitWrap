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

#include "CloneOperation.hpp"

#include "Private/CloneOperationPrivate.hpp"
#include "Private/FetchCallbacks.hpp"

namespace Git
{

    namespace Internal
    {

        CloneOperationPrivate::CloneOperationPrivate( CloneOperation* owner )
            : mOwner( owner )
        {
            mBackgroundMode = false;
            mThread = NULL;

            git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
            memcpy( &mGitCloneOptions, &opts, sizeof( opts ) );

            git_remote_callbacks rcb = GIT_REMOTE_CALLBACKS_INIT;
            memcpy( &mRemoteCallbacks, &rcb, sizeof( rcb ) );

            mRemoteCallbacks.completion             = &FetchCallbacks::remoteComplete;
            mRemoteCallbacks.progress               = &FetchCallbacks::remoteProgress;
            mRemoteCallbacks.update_tips            = &FetchCallbacks::remoteUpdateTips;
            mRemoteCallbacks.payload                = static_cast< IFetchEvents* >( mOwner );
            mGitCloneOptions.remote_callbacks       = &mRemoteCallbacks;

            mGitCloneOptions.fetch_progress_cb      = &FetchCallbacks::fetchProgress;
            mGitCloneOptions.fetch_progress_payload = static_cast< IFetchEvents* >( mOwner );

            mGitCloneOptions.cred_acquire_cb        = &FetchCallbacks::credAccquire;
            mGitCloneOptions.cred_acquire_payload   = static_cast< IFetchEvents* >( mOwner );
        }

        CloneOperationPrivate::~CloneOperationPrivate()
        {
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
            Q_ASSERT( !d->mThread );
            d->mThread = new Internal::WorkerThread( this, d );
            connect( d->mThread, SIGNAL(finished()),
                     this, SLOT(workerFinished()) );
            d->mThread->start();
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
        return d->mThread;
    }

    void CloneOperation::workerFinished()
    {
        delete d->mThread;
        d->mThread = NULL;
        emit finished();
    }

    Result CloneOperation::result() const
    {
        return d->mResult;
    }

}
