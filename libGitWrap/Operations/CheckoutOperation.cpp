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

            git_repository* repo = gitPtr( mRepo );
            runCheckout( repo );
            postCheckout( repo );

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

        void CheckoutIndexOperationPrivate::postCheckout(git_repository* repo)
        {

        }

        // -- CheckoutTreeOperationPrivate -->8

        CheckoutTreeOperationPrivate::CheckoutTreeOperationPrivate(CheckoutTreeOperation *owner)
            : CheckoutBaseOperationPrivate(owner)
        {
        }

        void CheckoutTreeOperationPrivate::runCheckout(git_repository* repo)
        {
            git_object* tree = mTreeProvider ? gitObjectPtr( mTreeProvider->tree(mResult) ) : NULL;
            GW_CHECK_RESULT( mResult, void() );

            mResult = git_checkout_tree(repo, tree, mOpts);
        }

        void CheckoutTreeOperationPrivate::postCheckout(git_repository* repo)
        {

        }


        // -- CheckoutCommitOperationPrivate -->8

        CheckoutCommitOperationPrivate::CheckoutCommitOperationPrivate(CheckoutCommitOperation *owner)
            : CheckoutTreeOperationPrivate(owner)
        {
        }

        void CheckoutCommitOperationPrivate::runCheckout(git_repository* repo)
        {
            if ( !mCommit.isValid() )
            {
                mResult.setInvalidObject();
                return;
            }

            CheckoutTreeOperationPrivate::runCheckout( repo );
        }

        void CheckoutCommitOperationPrivate::postCheckout(git_repository *repo)
        {
            GW_CHECK_RESULT( mResult, void() );

            if ( mStrategy.testFlag(CheckoutUpdateHEAD) )
            {
                mRepo.setDetachedHEAD( mResult, mCommit.id() );
            }
        }


        // -- CheckoutReferenceOperationPrivate -->8

        CheckoutReferenceOperationPrivate::CheckoutReferenceOperationPrivate(CheckoutReferenceOperation *owner)
            : CheckoutTreeOperationPrivate(owner)
        {
        }

        void CheckoutReferenceOperationPrivate::runCheckout(git_repository* repo)
        {
            if ( !mBranch.isValid() )
            {
                mResult.setInvalidObject();
                return;
            }

            CheckoutTreeOperationPrivate::runCheckout( repo );
        }

        void CheckoutReferenceOperationPrivate::postCheckout(git_repository* repo)
        {
            GW_CHECK_RESULT( mResult, void() );

            ReferencePrivate* p = BasePrivate::dataOf<Reference>( mBranch );

            if ( mStrategy.testFlag(CheckoutUpdateHEAD) )
            {
                mResult = git_repository_set_head( repo,
                                                   git_reference_name( p->reference ),
                                                   NULL, NULL);
            }
        }

    }

    // -- CheckoutBaseOperation -->8

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

    void CheckoutBaseOperation::setStrategy(unsigned int strategy)
    {
        GW_D(CheckoutBaseOperation);
        Q_ASSERT(!isRunning());
        d->mStrategy = static_cast<CheckoutFlags>(strategy);
    }

    void CheckoutBaseOperation::setTargetDirectory(const QString& path)
    {
        GW_D(CheckoutBaseOperation);
        Q_ASSERT(!isRunning());
        d->mOpts.setTargetDirectory( path );
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
        return d->mOpts.targetDirectory();
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
        : CheckoutBaseOperation( *new Private(this), parent )
    {
    }

    CheckoutTreeOperation::CheckoutTreeOperation(const Repository& repo, QObject* parent)
        : CheckoutBaseOperation( *new Private(this), parent )
    {
        setRepository( repo );
    }

    CheckoutTreeOperation::CheckoutTreeOperation(TreeProviderPtr tp, QObject* parent)
        : CheckoutBaseOperation( *new Private(this), parent )
    {
        GW_D( CheckoutTreeOperation );
        d->mTreeProvider = tp;
    }

    CheckoutTreeOperation::CheckoutTreeOperation(CheckoutTreeOperation::Private& _d,
                                                 TreeProviderPtr tp, QObject* parent)
        : CheckoutBaseOperation( _d, parent )
    {
        GW_D( CheckoutTreeOperation );
        d->mTreeProvider = tp;
        if ( tp ) {
            d->mRepo = tp->repository();
        }
    }

    void CheckoutTreeOperation::setTreeProvider(TreeProviderPtr tp)
    {
        Q_ASSERT(!isRunning());
        GW_D( CheckoutTreeOperation );
        d->mTreeProvider = tp;
    }

    TreeProviderPtr CheckoutTreeOperation::treeProvider() const
    {
        GW_CD( CheckoutTreeOperation );
        return d->mTreeProvider;
    }


    // -- CheckoutCommitOperation -->8
    CheckoutCommitOperation::CheckoutCommitOperation(const Commit& commit, QObject* parent)
        : CheckoutTreeOperation( *new Private(this), commit, parent)
    {
        GW_D( CheckoutCommitOperation );
        d->mCommit = commit;
    }

    Commit CheckoutCommitOperation::commit()
    {
        GW_CD( CheckoutCommitOperation );
        return d ? d->mCommit : Commit();
    }


    // -- CheckoutReferenceOperation -->8

    CheckoutReferenceOperation::CheckoutReferenceOperation(const Reference& branch, QObject* parent)
        : CheckoutTreeOperation( *new Private(this), branch, parent )
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
        GW_CD( CheckoutReferenceOperation );
        return d ? d->mBranch : Reference();
    }

}
