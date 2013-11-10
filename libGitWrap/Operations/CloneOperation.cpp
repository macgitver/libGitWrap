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

#include "libGitWrap/Operations/CloneOperation.hpp"

#include "libGitWrap/Operations/Private/CloneOperationPrivate.hpp"
#include "libGitWrap/Operations/Private/FetchCallbacks.hpp"

namespace Git
{

    namespace Internal
    {

        CloneOperationPrivate::CloneOperationPrivate(CloneOperation* owner)
            : BaseOperationPrivate(owner)
        {
            git_clone_options opts = GIT_CLONE_OPTIONS_INIT;
            memcpy(&mGitCloneOptions, &opts, sizeof(opts));

            git_remote_callbacks rcb = GIT_REMOTE_CALLBACKS_INIT;
            memcpy(&mRemoteCallbacks, &rcb, sizeof(rcb));

            mRemoteCallbacks.completion             = &FetchCallbacks::remoteComplete;
            mRemoteCallbacks.progress               = &FetchCallbacks::remoteProgress;
            mRemoteCallbacks.update_tips            = &FetchCallbacks::remoteUpdateTips;
            mRemoteCallbacks.payload                = static_cast< IFetchEvents* >(owner);
            mGitCloneOptions.remote_callbacks       = &mRemoteCallbacks;

            mGitCloneOptions.fetch_progress_cb      = &FetchCallbacks::fetchProgress;
            mGitCloneOptions.fetch_progress_payload = static_cast< IFetchEvents* >(owner);

            mGitCloneOptions.cred_acquire_cb        = &FetchCallbacks::credAccquire;
            mGitCloneOptions.cred_acquire_payload   = static_cast< IFetchEvents* >(owner);
        }

        CloneOperationPrivate::~CloneOperationPrivate()
        {
        }

        void CloneOperationPrivate::run()
        {
            git_repository* repo = NULL;
            mResult = git_clone(&repo,
                                mUrl.toUtf8().constData(),
                                mPath.toUtf8().constData(),
                                &mGitCloneOptions );

            if(mResult && repo) {
                git_repository_free( repo );
            }
        }

    }

    CloneOperation::CloneOperation(QObject* parent)
        : BaseOperation(*new Private(this), parent)
    {
    }

    CloneOperation::~CloneOperation()
    {
    }

    void CloneOperation::setUrl(const QString& url)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mUrl = url;
    }

    void CloneOperation::setPath( const QString& path )
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mPath = path;
    }

    void CloneOperation::setBare(bool bare)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mGitCloneOptions.bare = bare ? 1 : 0;
    }

    void CloneOperation::setRemoteName(const QByteArray& remoteName)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mRemoteName = remoteName;
        d->mGitCloneOptions.remote_name = remoteName.isEmpty() ? NULL : remoteName.constData();
    }

    void CloneOperation::setRemoteName(const QString& remoteName)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mRemoteName = remoteName.toUtf8();
        d->mGitCloneOptions.remote_name = remoteName.isEmpty() ? NULL : d->mRemoteName.constData();
    }

    void CloneOperation::setFetchSpec(const QByteArray& fetchSpec)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mFetchSpec = fetchSpec;
        d->mGitCloneOptions.fetch_spec = fetchSpec.isEmpty() ? NULL : fetchSpec.constData();
    }

    void CloneOperation::setPushSpec(const QByteArray& pushSpec)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mPushSpec = pushSpec;
        d->mGitCloneOptions.push_spec = pushSpec.isEmpty() ? NULL : pushSpec.constData();
    }

    void CloneOperation::setPushUrl(const QByteArray& pushUrl)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mPushUrl = pushUrl;
        d->mGitCloneOptions.pushurl = pushUrl.isEmpty() ? NULL : pushUrl.constData();
    }

    QString CloneOperation::url() const
    {
        GW_CD(CloneOperation);
        return d->mUrl;
    }

    QString CloneOperation::path() const
    {
        GW_CD(CloneOperation);
        return d->mPath;
    }

    bool CloneOperation::bare() const
    {
        GW_CD(CloneOperation);
        return d->mGitCloneOptions.bare != 0;
    }

    QByteArray CloneOperation::remoteName() const
    {
        GW_CD(CloneOperation);
        return d->mRemoteName;
    }

    QByteArray CloneOperation::fetchSpec() const
    {
        GW_CD(CloneOperation);
        return d->mFetchSpec;
    }

    QByteArray CloneOperation::pushSpec() const
    {
        GW_CD(CloneOperation);
        return d->mPushSpec;
    }

    QByteArray CloneOperation::pushUrl() const
    {
        GW_CD(CloneOperation);
        return d->mPushUrl;
    }

}
