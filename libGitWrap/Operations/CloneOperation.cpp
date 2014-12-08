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
        {
            CheckoutOptionsRef coo = mCloneOpts.checkoutOptions();
            (*coo).checkout_strategy = GIT_CHECKOUT_SAFE_CREATE;
            // TODO: setup checkout callbacks for notification about the checkout progres
        }

        CloneOperationPrivate::~CloneOperationPrivate()
        {
        }

        void CloneOperationPrivate::run()
        {
            GW_OP_OWNER(CloneOperation);

            git_repository* repo = NULL;

            RemoteCallbacks::initCallbacks( (*mCloneOpts).remote_callbacks, owner );

            if (mResult) {
                mResult = git_clone(&repo, GW_StringFromQt(mUrl), GW_StringFromQt(mPath), mCloneOpts);
            }

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
        (*(d->mCloneOpts)).bare = bare;
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
        return (*(d->mCloneOpts)).bare;
    }

}
