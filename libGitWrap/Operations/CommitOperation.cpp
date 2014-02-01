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
                , mParentProvider(0)
                , mTreeProvider(0)
            {
            }

            ~CommitBaseOperationPrivate()
            {
            }

        public:
            void prepare()
            {
            }

            void finalize()
            {
            }

            void run()
            {
            }

        public:
            void emitProgress(const QString& pathName, quint32 completed, quint32 total)
            {
                GW_OP_OWNER(CommitOperation);
                owner->progress(owner, pathName, completed, total);
            }

        public:
            CommitParentProvider::Ptr   mParentProvider;
            CommitTreeProvider::Ptr     mTreeProvider;

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

    CommitParentProvider::Ptr CommitOperation::parentProvider() const
    {
        GW_CD(CommitOperation);
        return d->mParentProvider;
    }

    void CommitOperation::setParentProvider(CommitParentProvider::Ptr p)
    {
        GW_D(CommitOperation);
        Q_ASSERT(!isRunning());
        d->mParentProvider = p;
    }

    CommitTreeProvider::Ptr CommitOperation::treeProvider() const
    {
        GW_CD(CommitOperation);
        return d->mTreeProvider;
    }

    void CommitOperation::setTreeProvider(CommitTreeProvider::Ptr p)
    {
        GW_D(CommitOperation);
        Q_ASSERT(!isRunning());
        d->mTreeProvider = p;
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
