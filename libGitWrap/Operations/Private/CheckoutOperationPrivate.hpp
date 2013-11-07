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

#include "libGitWrap/Result.hpp"
#include "libGitWrap/Tree.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Index.hpp"
#include "libGitWrap/BranchRef.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"

#include "libGitWrap/Operations/Private/BaseOperationPrivate.hpp"

namespace Git
{

    class CheckoutBaseOperation;
    class CheckoutIndexOperation;
    class CheckoutTreeOperation;

    namespace Internal
    {

        class CheckoutBaseOperationPrivate : public BaseOperationPrivate
        {
        public:
            CheckoutBaseOperationPrivate(CheckoutBaseOperation* owner);
            ~CheckoutBaseOperationPrivate();

        public:
            void prepare();
            void unprepare();

        public:
            void emitFile(git_checkout_notify_t why, const char *path,
                          const git_diff_file *baseline, const git_diff_file *target,
                          const git_diff_file *workdir);
            void emitProgress(const QString& pathName, quint32 completed, quint32 total);

        public:
            Repository          mRepository;
            CheckoutOptions     mOptions;
            CheckoutMode        mMode;
            QStringList         mPaths;
            QString             mPath;
            Tree                mBaseline;
            git_checkout_opts   mOpts;
            bool                mCancel;
        };

        class CheckoutIndexOperationPrivate : public CheckoutBaseOperationPrivate
        {
        public:
            CheckoutIndexOperationPrivate(CheckoutIndexOperation* owner);

        public:
            void run();

        public:
            Index               mIndex;
        };

        class CheckoutTreeOperationPrivate : public CheckoutBaseOperationPrivate
        {
        public:
            CheckoutTreeOperationPrivate(CheckoutTreeOperation* owner);

        public:
            void run();

        public:
            Tree                mTree;
        };

        class CheckoutBranchOperationPrivate : public CheckoutBaseOperationPrivate
        {
        public:
            CheckoutBranchOperationPrivate(CheckoutBranchOperation* owner);

        public:
            void run();

        public:
            BranchRef           branch;
        };

    }

}
