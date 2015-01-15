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

#include "libGitWrap/BranchRef.hpp"
#include "libGitWrap/Commit.hpp"
#include "libGitWrap/Index.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Result.hpp"
#include "libGitWrap/Tree.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"

#include "libGitWrap/Operations/Private/BaseOperationPrivate.hpp"

namespace Git
{

    class CheckoutBaseOperation;
    class CheckoutCommitOperation;
    class CheckoutIndexOperation;
    class CheckoutReferenceOperation;
    class CheckoutTreeOperation;

    namespace Internal
    {

        class CheckoutBaseOperationPrivate : public BaseOperationPrivate
        {
        public:
            CheckoutBaseOperationPrivate(CheckoutBaseOperation* owner);

        protected:
            static git_repository* gitPtr(const Repository& obj);
            static git_index* gitPtr(const Index& obj);

            static git_object* gitObjectPtr(const Tree& obj);

        protected:
            virtual void prepare();
            virtual void unprepare();

        private:
            void run();
            virtual void postCheckout(git_repository* repo) = 0;

        private:
            virtual void runCheckout(git_repository* repo) = 0;

        public:
            Repository              mRepo;
            CheckoutFlags           mStrategy;
            CheckoutMode            mMode;
            Tree                    mBaseline;
            CheckoutOptions         mOpts;
            bool                    mCancel;
        };


        class CheckoutIndexOperationPrivate : public CheckoutBaseOperationPrivate
        {
        public:
            CheckoutIndexOperationPrivate(CheckoutIndexOperation* owner);

        protected:
            virtual void runCheckout(git_repository* repo);
            virtual void postCheckout(git_repository* repo);

        public:
            Index               mIndex;
        };


        class CheckoutTreeOperationPrivate : public CheckoutBaseOperationPrivate
        {
        public:
            CheckoutTreeOperationPrivate(CheckoutTreeOperation* owner);

        protected:
            virtual void runCheckout(git_repository* repo);
            virtual void postCheckout(git_repository* repo);

        public:
            TreeProviderPtr     mTreeProvider;
        };


        class CheckoutCommitOperationPrivate : public CheckoutTreeOperationPrivate
        {
        public:
            CheckoutCommitOperationPrivate(CheckoutCommitOperation* owner);

        public:
            void runCheckout(git_repository* repo);
            void postCheckout(git_repository* repo);

        public:
            Commit      mCommit;
        };


        class CheckoutReferenceOperationPrivate : public CheckoutTreeOperationPrivate
        {
        public:
            CheckoutReferenceOperationPrivate(CheckoutReferenceOperation* owner);

        protected:
            virtual void runCheckout(git_repository *repo);
            virtual void postCheckout(git_repository* repo);

        public:
            Reference   mBranch;
        };

    }

}
