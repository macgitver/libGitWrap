/*
 * MacGitver
 * Copyright (C) 2014 Sascha Cunz <sascha@macgitver.org>
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
    class CheckoutReferenceOperation;
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
            Repository              mRepo;
            CheckoutFlags           mStrategy;
            CheckoutMode            mMode;
            QString                 mPath;
            Tree                    mBaseline;
            CheckoutOptions         mOpts;
            bool                    mCancel;
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

        class CheckoutReferenceOperationPrivate : public CheckoutBaseOperationPrivate
        {
        public:
            CheckoutReferenceOperationPrivate(CheckoutReferenceOperation* owner);

        public:
            void run();

        public:
            Reference       mBranch;
        };

    }

}
