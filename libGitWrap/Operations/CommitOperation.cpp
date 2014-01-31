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
            Repository          mRepository;
            QString             mCommitMessage;
            bool                mSignoff;

            QStringList         mPaths;
            QString             mPath;
            bool                mCancel;
        };

    }

    // ********* CommitOperation *********

    CommitOperation::CommitOperation(QObject *parent)
        : BaseOperation(*new Private(this), parent)
        , mParentProvider(0)
        , mTreeProvider(0)
    {

    }

    CommitOperation::~CommitOperation()
    {

    }

    CommitParentProvider *CommitOperation::parentProvider() const
    {
        return mParentProvider;
    }

    void CommitOperation::setParentProvider(CommitParentProvider* p)
    {
        mParentProvider = p;
    }

    CommitTreeProvider *CommitOperation::treeProvider() const
    {
        return mTreeProvider;
    }

    void CommitOperation::setTreeProvider(CommitTreeProvider* p)
    {
        mTreeProvider = p;
    }

    QString CommitOperation::message() const
    {
        return mMessage;
    }

    void CommitOperation::setMessage(const QString &message)
    {
        mMessage = message;
    }

    Signature CommitOperation::committer() const
    {
        return mCommitter;
    }

    void CommitOperation::setCommitter(const Signature &value)
    {
        mCommitter = value;
    }

    Signature CommitOperation::author() const
    {
        return mAuthor;
    }

    void CommitOperation::setAuthor(const Signature &value)
    {
        mAuthor = value;
    }

}
