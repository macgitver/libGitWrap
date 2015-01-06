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

        git_repository* CheckoutBaseOperationPrivate::gitPtr( const Repository& obj )
        {
            return obj.isValid() ? BasePrivate::dataOf<Repository>( obj )->mRepo : NULL;
        }

        git_index* CheckoutBaseOperationPrivate::gitPtr( const Index& obj )
        {
            return obj.isValid() ? (BasePrivate::dataOf<Index>( obj )->index) : NULL;
        }

        git_object* CheckoutBaseOperationPrivate::gitObjectPtr( const Tree& obj )
        {
            return obj.isValid() ? BasePrivate::dataOf<Tree>( obj )->mObj : NULL;
        }

        CheckoutBaseOperationPrivate::CheckoutBaseOperationPrivate(CheckoutBaseOperation* owner)
            : BaseOperationPrivate(owner)
            , mMode(CheckoutDryRun)
        {
            CheckoutCallbacks::initCallbacks( mOpts, owner );
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

        void CheckoutBaseOperationPrivate::run()
        {
            GW_CHECK_RESULT( mResult, void() );

            prepare();

            runCheckout( gitPtr( mRepo ) );

            unprepare();
        }

        // -- CheckoutIndexOperationPrivate -->8

        CheckoutIndexOperationPrivate::CheckoutIndexOperationPrivate(CheckoutIndexOperation *owner)
            : CheckoutBaseOperationPrivate(owner)
        {
        }

        void CheckoutIndexOperationPrivate::runCheckout(git_repository* repo)
        {
            mResult = git_checkout_index(repo, gitPtr( mIndex ), mOpts);
        }

        // -- CheckoutTreeOperationPrivate -->8

        CheckoutTreeOperationPrivate::CheckoutTreeOperationPrivate(CheckoutTreeOperation *owner)
            : CheckoutBaseOperationPrivate(owner)
        {
        }

        void CheckoutTreeOperationPrivate::runCheckout(git_repository* repo)
        {

            git_repository* repo = NULL;
            if ( mRepo.isValid() ) {
                repo = BasePrivate::dataOf<Repository>( mRepo )->mRepo;
            }

            git_object* tree = NULL;
            if ( mTree.isValid() ) {
                tree = BasePrivate::dataOf<Tree>( mTree )->mObj;
            }

            mResult = git_checkout_tree(repo, tree, mOpts);
        }

        // -- CheckoutBranchOperationPrivate ---------------------------------------------------- >8

        CheckoutReferenceOperationPrivate::CheckoutReferenceOperationPrivate(CheckoutReferenceOperation *owner)
            : CheckoutBaseOperationPrivate(owner)
        {
        }

        void CheckoutReferenceOperationPrivate::runCheckout(git_repository* repo)
        {
            if ( !mBranch.isValid() )
            {
                mResult.setInvalidObject();
                return;
            }

            ReferencePrivate* p = BasePrivate::dataOf<Reference>( mBranch );

            git_object* tree = NULL;
            mResult = git_reference_peel( &tree, p->reference, GIT_OBJ_TREE );
            git_object_free( tree );
            GW_CHECK_RESULT( mResult, void() );
            Q_ASSERT( tree );

            mResult = git_checkout_tree( NULL, tree, mOpts);

            git_object_free( tree );

            if ( mResult && git_reference_is_branch( p->reference ) )
            {
                if ( mResult ) {
                    mResult = git_repository_set_head( p->repo()->mRepo,
                                                       git_reference_name( p->reference ),
                                                       NULL, NULL);
                }
            }

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
        d->mRepo = repo;
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
        return d->mRepo;
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
        Q_ASSERT(!isRunning());
        GW_D(CheckoutIndexOperation);
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
        Q_ASSERT(!isRunning());
        GW_D( CheckoutTreeOperation );
        d->mTree = tree;
    }

    Tree CheckoutTreeOperation::tree() const
    {
        GW_CD( CheckoutTreeOperation );
        return d->mTree;
    }

    // -- CheckoutReferenceOperation -- >8

    CheckoutReferenceOperation::CheckoutReferenceOperation(const Git::Reference& branch, QObject* parent)
        : CheckoutBaseOperation(*new Private(this), parent)
    {
        setBranch( branch );
    }

    void CheckoutReferenceOperation::setBranch(const Reference& ref)
    {
        GW_D( CheckoutReferenceOperation );
        d->mBranch = ref;
    }

    Reference CheckoutReferenceOperation::branch() const
    {
        GW_CD(CheckoutReferenceOperation);
        return d ? d->mBranch : Reference();
    }

}
