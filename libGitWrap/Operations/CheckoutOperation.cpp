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

#include "CheckoutOperation.hpp"

#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Index.hpp"

#include "libGitWrap/Events/Private/GitEventCallbacks.hpp"

#include "libGitWrap/Private/BranchRefPrivate.hpp"
#include "libGitWrap/Private/IndexPrivate.hpp"
#include "libGitWrap/Private/RepositoryPrivate.hpp"
#include "libGitWrap/Private/TreePrivate.hpp"

#include "libGitWrap/Operations/Private/CheckoutOperationPrivate.hpp"


namespace Git
{

    namespace Internal
    {

        // -- CheckoutBaseOperationPrivate -- >8

        CheckoutBaseOperationPrivate::CheckoutBaseOperationPrivate(CheckoutBaseOperation* owner)
            : BaseOperationPrivate(owner)
            , mMode(CheckoutDryRun)
        {
            CheckoutCallbacks::initCallbacks( mOpts, owner );
        }

        CheckoutBaseOperationPrivate::~CheckoutBaseOperationPrivate()
        {
        }

        void CheckoutBaseOperationPrivate::prepare()
        {
            (*mOpts).target_directory = mPath.isEmpty() ? NULL : GW_StringFromQt(mPath);

            switch (mMode) {
            default:
            case CheckoutDryRun:     /* this is the default */                               break;
            case CheckoutSafe:       (*mOpts).checkout_strategy |= GIT_CHECKOUT_SAFE;        break;
            case CheckoutSafeCreate: (*mOpts).checkout_strategy |= GIT_CHECKOUT_SAFE_CREATE; break;
            case CheckoutForce:      (*mOpts).checkout_strategy |= GIT_CHECKOUT_FORCE;       break;
            }

            static const int flags[] = {
                CheckoutAllowConflicts,         GIT_CHECKOUT_ALLOW_CONFLICTS,
                CheckoutRemoveUntracked,        GIT_CHECKOUT_REMOVE_UNTRACKED,
                CheckoutRemoveIgnored,          GIT_CHECKOUT_REMOVE_IGNORED,
                CheckoutNoRefresh,              GIT_CHECKOUT_NO_REFRESH,
                CheckoutUpdateOnly,             GIT_CHECKOUT_UPDATE_ONLY,
                CheckoutDontUpdateIndex,        GIT_CHECKOUT_DONT_UPDATE_INDEX,
                CheckoutDisablePathSpecMatch,   GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH,
                CheckoutSkipLockedDirectories,  GIT_CHECKOUT_SKIP_LOCKED_DIRECTORIES,
                CheckoutNone
            };

            int i = 0;
            while (flags[i] != CheckoutNone) {
                if (mStrategy.testFlag(CheckoutFlag(flags[i++]))) {
                    (*mOpts).checkout_strategy |= flags[i];
                }
                i++;
            }

            if (mBaseline.isValid()) {
                (*mOpts).baseline = BasePrivate::dataOf<Tree>( mBaseline )->o();
            }
        }

        void CheckoutBaseOperationPrivate::unprepare()
        {
        }

        // -- CheckoutIndexOperationPrivate ----------------------------------------------------- >8

        CheckoutIndexOperationPrivate::CheckoutIndexOperationPrivate(CheckoutIndexOperation *owner)
            : CheckoutBaseOperationPrivate(owner)
        {
        }

        void CheckoutIndexOperationPrivate::run()
        {
            git_repository* grepo = NULL;
            git_index* gindex = NULL;

            prepare();

            if (mRepository.isValid()) {
                Repository::Private* rp = BasePrivate::dataOf<Repository>(mRepository);
                grepo = rp->mRepo;
            }

            if (mIndex.isValid()) {
                Index::Private* ip = BasePrivate::dataOf<Index>(mIndex);
                gindex = ip->index;
            }

            mResult = git_checkout_index(grepo, gindex, mOpts);

            unprepare();
        }

        // -- CheckoutTreeOperationPrivate ------------------------------------------------------ >8

        CheckoutTreeOperationPrivate::CheckoutTreeOperationPrivate(CheckoutTreeOperation *owner)
            : CheckoutBaseOperationPrivate(owner)
        {
        }

        void CheckoutTreeOperationPrivate::run()
        {
            git_repository* grepo = NULL;
            const git_object* gtree = NULL;

            prepare();

            if (mRepository.isValid()) {
                Repository::Private* rp = BasePrivate::dataOf<Repository>(mRepository);
                grepo = rp->mRepo;
            }

            if (mTree.isValid()) {
                Tree::Private* tp = BasePrivate::dataOf<Tree>(mTree);
                gtree = tp->mObj;
            }

            if (gtree) {
                mResult = git_checkout_tree(grepo, gtree, mOpts);
            }
            else {
                mResult = git_checkout_head(grepo, mOpts);
            }

            unprepare();
        }

        // -- CheckoutBranchOperationPrivate ---------------------------------------------------- >8

        CheckoutBranchOperationPrivate::CheckoutBranchOperationPrivate(
                CheckoutBranchOperation *owner)
            : CheckoutBaseOperationPrivate(owner)
        {
        }

        void CheckoutBranchOperationPrivate::run()
        {
            /*
            git_repository* grepo = NULL;
            const git_object* gtree = NULL;
            */

            prepare();

            if (mRepository.isValid()) {
                //Repository::Private* rp = BasePrivate::dataOf<Repository>(mRepository);
                //grepo = rp->mRepo;
            }

            /*
            if (mTree.isValid()) {
                Tree::Private* tp = BasePrivate::dataOf<Tree>(mTree);
                gtree = tp->mObj;
            }

            if (gtree) {
                mResult = git_checkout_tree(grepo, gtree, &mOpts);
            }
            else {
                mResult = git_checkout_head(grepo, &mOpts);
            }
            */

            unprepare();
        }

    }

    // -- CheckoutBaseOperation ----------------------------------------------------------------- >8

    CheckoutBaseOperation::CheckoutBaseOperation(Private& _d, QObject* parent)
        : BaseOperation(_d, parent)
    {
    }

    CheckoutBaseOperation::~CheckoutBaseOperation()
    {
    }

    void CheckoutBaseOperation::setRepository(const Repository& repo)
    {
        GW_D(CheckoutBaseOperation);
        Q_ASSERT(!isRunning());
        d->mRepository = repo;
    }

    void CheckoutBaseOperation::setMode(CheckoutMode mode)
    {
        GW_D(CheckoutBaseOperation);
        Q_ASSERT(!isRunning());
        d->mMode = mode;
    }

    void CheckoutBaseOperation::setStrategy(CheckoutFlags strategy)
    {
        GW_D(CheckoutBaseOperation);
        Q_ASSERT(!isRunning());
        d->mStrategy = strategy;
    }

    void CheckoutBaseOperation::setTargetDirectory(const QString& path)
    {
        GW_D(CheckoutBaseOperation);
        Q_ASSERT(!isRunning());
        d->mPath = path;
    }

    void CheckoutBaseOperation::setCheckoutPaths(const QStringList& paths)
    {
        GW_D(CheckoutBaseOperation);
        Q_ASSERT(!isRunning());
        d->mOpts.setPaths( paths );
    }

    void CheckoutBaseOperation::setBaseline(const Tree& baseline)
    {
        GW_D(CheckoutBaseOperation);
        Q_ASSERT(!isRunning());
        d->mBaseline = baseline;
    }

    Repository CheckoutBaseOperation::repository() const
    {
        GW_CD(CheckoutBaseOperation);
        return d->mRepository;
    }

    CheckoutMode CheckoutBaseOperation::mode() const
    {
        GW_CD(CheckoutBaseOperation);
        return d->mMode;
    }

    CheckoutFlags CheckoutBaseOperation::strategy() const
    {
        GW_CD(CheckoutBaseOperation);
        return d->mStrategy;
    }

    QString CheckoutBaseOperation::targetDirectory() const
    {
        GW_CD(CheckoutBaseOperation);
        return d->mPath;
    }

    QStringList CheckoutBaseOperation::checkoutPaths() const
    {
        GW_CD(CheckoutBaseOperation);
        return d->mOpts.paths();
    }

    Tree CheckoutBaseOperation::baseline() const
    {
        GW_CD(CheckoutBaseOperation);
        return d->mBaseline;
    }

    void CheckoutBaseOperation::setCancel(bool cancel)
    {
        GW_D(CheckoutBaseOperation);
        Q_ASSERT(isRunning());
        d->mCancel = cancel;
    }

    // -- CheckoutIndexOperation ---------------------------------------------------------------- >8

    CheckoutIndexOperation::CheckoutIndexOperation(QObject* parent)
        : CheckoutBaseOperation(*new Private(this), parent)
    {
    }

    CheckoutIndexOperation::CheckoutIndexOperation(const Index& index, QObject* parent)
        : CheckoutBaseOperation(*new Private(this), parent)
    {
        setRepository(index.repository());
        setIndex(index);
    }

    CheckoutIndexOperation::CheckoutIndexOperation(const Repository& repo, QObject* parent)
        : CheckoutBaseOperation(*new Private(this), parent)
    {
        setRepository(repo);
    }

    void CheckoutIndexOperation::setIndex(const Index& index)
    {
        GW_D(CheckoutIndexOperation);
        Q_ASSERT(!isRunning());
        d->mIndex = index;
    }

    // -- CheckoutTreeOperation ----------------------------------------------------------------- >8

    CheckoutTreeOperation::CheckoutTreeOperation(QObject* parent)
        : CheckoutBaseOperation(*new Private(this), parent)
    {
    }

    CheckoutTreeOperation::CheckoutTreeOperation(const Tree& tree, QObject* parent)
        : CheckoutBaseOperation(*new Private(this), parent)
    {
        setRepository(tree.repository());
        setTree(tree);
    }

    CheckoutTreeOperation::CheckoutTreeOperation(const Repository& repo, QObject* parent)
        : CheckoutBaseOperation(*new Private(this), parent)
    {
        setRepository(repo);
    }

    void CheckoutTreeOperation::setTree(const Tree& tree)
    {
        GW_D(CheckoutTreeOperation);
        Q_ASSERT(!isRunning());
        d->mTree = tree;
    }

    Tree CheckoutTreeOperation::tree() const
    {
        GW_CD(CheckoutTreeOperation);
        return d->mTree;
    }

    // -- CheckoutBranchOperation --------------------------------------------------------------- >8

    CheckoutBranchOperation::CheckoutBranchOperation(QObject* parent)
        : CheckoutBaseOperation(*new Private(this), parent)
    {
    }

    CheckoutBranchOperation::CheckoutBranchOperation(const BranchRef& branch, QObject* parent)
        : CheckoutBaseOperation(*new Private(this), parent)
    {
        setRepository(branch.repository());
        setBranch(branch);
    }

    CheckoutBranchOperation::CheckoutBranchOperation(const Repository& repo, QObject* parent)
        : CheckoutBaseOperation(*new Private(this), parent)
    {
        setRepository(repo);
    }

    CheckoutBranchOperation::CheckoutBranchOperation(const Repository& repo,
                                                     const QString& branchName,
                                                     QObject* parent)
        : CheckoutBaseOperation(*new Private(this), parent)
    {
        setRepository(repo);
        setBranch(branchName);
    }

    bool CheckoutBranchOperation::setBranch(const QString& branchName)
    {
        Q_ASSERT(!isRunning());

        if (!repository().isValid()) {
            return false;
        }

        GW_D(CheckoutBranchOperation);
        if (!d) {
            return false;
        }

        Result res;
        d->branch = repository().branchRef(res, branchName);
        return res && d->branch.isValid();
    }

    bool CheckoutBranchOperation::setBranch(const BranchRef& branch)
    {
        Q_ASSERT(!isRunning());
        GW_D(CheckoutBranchOperation);

        if (!d) {
            return false;
        }

        if (branch.repository() != repository()) {
            return false;
        }

        d->branch = branch;
        return true;
    }

    BranchRef CheckoutBranchOperation::branch() const
    {
        GW_CD(CheckoutBranchOperation);
        return d ? d->branch : BranchRef();
    }

}

