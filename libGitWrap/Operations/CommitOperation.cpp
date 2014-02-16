/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2013-2014 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Nils Fenner <nilsfenner@web.de>
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

#include "libGitWrap/Operations/CommitOperation.hpp"
#include "libGitWrap/Operations/Private/BaseOperationPrivate.hpp"

#include "libGitWrap/Commit.hpp"
#include "libGitWrap/Index.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Signature.hpp"
#include "libGitWrap/Tree.hpp"

namespace Git
{
    namespace Internal
    {
        class CommitOperationPrivate : public BaseOperationPrivate
        {
        public:
            CommitOperationPrivate(CommitOperation* owner)
                : BaseOperationPrivate(owner)
            {
            }

            virtual ~CommitOperationPrivate()
            {
            }

        public:
            bool prepare()
            {
                Q_ASSERT( mProvider );
                return mProvider->prepare();
            }

            bool finalize()
            {
                Q_ASSERT( mProvider );
                return mProvider->finalize( ObjectId() );
            }

            void run()
            {
                prepare();

                Repository      repo = mProvider->commitOperationRepository();
                Tree            tree = mProvider->commitOperationTree(mResult);
                ObjectIdList    parents = mProvider->commitOperationParents(mResult);

                Commit::create( mResult, repo, tree, mMessage, mAuthor, mCommitter, parents );

                finalize();
            }

        public:
            void emitProgress(const QString& pathName, quint32 completed, quint32 total)
            {
                GW_OP_OWNER(CommitOperation);
                owner->progress(owner, pathName, completed, total);
            }

        public:
            CommitOperationProvider::Ptr   mProvider;

            QString             mMessage;
            Signature           mAuthor;
            Signature           mCommitter;
            bool                mSignoff;

            QStringList         mPaths;
            QString             mPath;
            bool                mCancel;
        };

    }

    // ********* CommitOperation *********

    CommitOperation::CommitOperation(QObject *parent)
        : BaseOperation(*new Private(this), parent)
    {

    }

    CommitOperation::~CommitOperation()
    {

    }

    Internal::CommitOperationProvider::Ptr CommitOperation::operationProvider() const
    {
        GW_CD(CommitOperation);
        return d->mProvider;
    }

    void CommitOperation::setOperationProvider(const Internal::CommitOperationProvider::Ptr &p)
    {
        GW_D(CommitOperation);
        Q_ASSERT(!isRunning());
        d->mProvider = p;
    }

    QString CommitOperation::message() const
    {
        GW_CD(CommitOperation);
        return d->mMessage;
    }

    void CommitOperation::setMessage(const QString &message)
    {
        GW_D(CommitOperation);
        Q_ASSERT(!isRunning());
        d->mMessage = message;
    }

    Signature CommitOperation::committer() const
    {
        GW_CD(CommitOperation);
        return d->mCommitter;
    }

    void CommitOperation::setCommitter(const Signature &value)
    {
        GW_D(CommitOperation);
        Q_ASSERT(!isRunning());
        d->mCommitter = value;
    }

    /**
     * @brief Sets the Git default signature for author and committer.
     *
     * A CommitOperationProvider must be set before calling this method.
     */
    void CommitOperation::setDefaultSignatures()
    {
        GW_D(CommitOperation);
        Q_ASSERT(!isRunning());

        if (!d->mProvider)
        {
            d->mResult.setInvalidObject();
            return;
        }

        d->mAuthor = Signature::defaultSignature( d->mResult,
                                                  d->mProvider->commitOperationRepository() );
        d->mCommitter = d->mAuthor;
    }

    Signature CommitOperation::author() const
    {
        GW_CD(CommitOperation);
        return d->mAuthor;
    }

    void CommitOperation::setAuthor(const Signature &value)
    {
        GW_D(CommitOperation);
        Q_ASSERT(!isRunning());
        d->mAuthor = value;
    }

}
