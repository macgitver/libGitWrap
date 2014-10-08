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

#include <QStringBuilder>

#include "libGitWrap/Operations/CloneOperation.hpp"
#include "libGitWrap/Operations/Private/CloneOperationPrivate.hpp"

#include "libGitWrap/Events//Private/RemoteCallbacks.hpp"

namespace Git
{

    namespace Internal
    {

        CloneOperationPrivate::CloneOperationPrivate(CloneOperation* owner)
            : BaseOperationPrivate(owner)
            , mCloneBare(false)
        {
            git_checkout_options coo = GIT_CHECKOUT_OPTIONS_INIT;
            coo.checkout_strategy = GIT_CHECKOUT_SAFE_CREATE;
            memcpy(&mCheckoutOpts, &coo, sizeof(coo));
        }

        CloneOperationPrivate::~CloneOperationPrivate()
        {
        }

        void CloneOperationPrivate::run()
        {
            GW_OP_OWNER(CloneOperation);

            git_repository* repo = NULL;
            git_remote* remo = NULL;

            mResult = git_repository_init(&repo, mPath.toUtf8().constData(), mCloneBare);
            if (!mResult) {
                return;
            }

            if (mRemoteName.isEmpty()) {
                mRemoteName = "origin";
            }

            if (mFetchSpec.isEmpty()) {
                mFetchSpec = QByteArray("+refs/heads/*:refs/remotes/") %
                             mRemoteName %
                             QByteArray("/*");
            }

            mResult = git_remote_create_with_fetchspec(
                        &remo, repo, mRemoteName.constData(),
                        mUrl.toUtf8().constData(),
                        mFetchSpec.constData());

            if (mResult && !mPushSpec.isEmpty()) {
                mResult = git_remote_add_push(remo, mPushSpec.constData());
            }

            if (mResult && !mPushUrl.isEmpty()) {
                mResult = git_remote_set_pushurl(remo, mPushUrl.constData());
            }

            if (mResult) {
                mResult = git_remote_save(remo);
            }

            if (mResult) {
                git_remote_callbacks cbs;
                RemoteCallbacks::initCallbacks(cbs, owner);
                mResult = git_remote_set_callbacks(remo, &cbs);
            }

            if (mResult) {
                mResult = git_clone_into(repo, remo, &mCheckoutOpts, NULL, NULL);
            }

            git_remote_free(remo);
            git_repository_free(repo);
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
        d->mCloneBare = bare;
    }

    void CloneOperation::setRemoteName(const QByteArray& remoteName)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mRemoteName = remoteName;
    }

    void CloneOperation::setRemoteName(const QString& remoteName)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mRemoteName = remoteName.toUtf8();
    }

    void CloneOperation::setFetchSpec(const QByteArray& fetchSpec)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mFetchSpec = fetchSpec;
    }

    void CloneOperation::setPushSpec(const QByteArray& pushSpec)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mPushSpec = pushSpec;
    }

    void CloneOperation::setPushUrl(const QByteArray& pushUrl)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mPushUrl = pushUrl;
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
        return d->mCloneBare;
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
