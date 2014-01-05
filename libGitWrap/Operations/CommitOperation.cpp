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

        class CommitOperationPrivate : public BaseOperationPrivate
        {
        public:
            CommitOperationPrivate(CommitOperation* owner)
                : BaseOperationPrivate(owner)
            {
            }

            ~CommitOperationPrivate()
            {
            }

        public:
            void prepare()
            {

            }

            void unprepare()
            {

            }

            void run()
            {
                Result r;
                mRepository.index(r).commitToRepository(r, mCommitMessage);
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

    CommitOperation::CommitOperation(CommitOperation::Private &_d, QObject *parent)
        : BaseOperation(_d, parent)
    {

    }

    CommitOperation::~CommitOperation()
    {

    }
}
