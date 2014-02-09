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

        class CommitBaseOperationPrivate : public BaseOperationPrivate
        {
        public:
            CommitBaseOperationPrivate(CommitOperation* owner)
                : BaseOperationPrivate(owner)
                , mCommitProvider(0)
            {
            }

            ~CommitBaseOperationPrivate()
            {
            }

        public:
            bool prepare()
            {
                Q_ASSERT( mCommitProvider );
                return mCommitProvider->prepare();
            }

            bool finalize()
            {
                Q_ASSERT( mCommitProvider );
                return mCommitProvider->finalize( ObjectId() );
            }

            void run()
            {
                prepare();

                Repository      repo = mCommitProvider->commitOperationRepository();
                Tree            tree = mCommitProvider->commitOperationTree(mResult);
                ObjectIdList    parents = mCommitProvider->commitOperationParents(mResult);

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
            CommitOperationProvider::Ptr   mCommitProvider;

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

    CommitOperationProvider::Ptr CommitOperation::operationProvider() const
    {
        GW_CD(CommitOperation);
        return d->mCommitProvider;
    }

    void CommitOperation::setOperationProvider(CommitOperationProvider::Ptr p)
    {
        GW_D(CommitOperation);
        Q_ASSERT(!isRunning());
        d->mCommitProvider = p;
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
