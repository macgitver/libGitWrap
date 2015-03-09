/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2014 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Nils Fenner <nils@macgitver.org>
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

#include "RemoteOperations.hpp"
#include "Private/RemoteOperationsPrivate.hpp"

#include "libGitWrap/Events/Private/GitEventCallbacks.hpp"

#include "libGitWrap/Private/RemotePrivate.hpp"


namespace Git
{

    namespace Internal
    {

        //-- BaseRemoteOperationPrivate -->8

        int BaseRemoteOperationPrivate::CB_CreateRemote(git_remote** out, git_repository* repo, const char* name, const char* url, void* payload)
        {
            BaseRemoteOperationPrivate* p = static_cast< BaseRemoteOperationPrivate* >( payload );
            Q_ASSERT( p );

            Result r = p->mResult;
            GW_CHECK_RESULT( r, r.errorCode() );

            r = git_remote_create( out, repo, GW_StringFromQt_Def(p->mRemoteAlias, name), url);
            GW_CHECK_RESULT( r, r.errorCode() );

            r = git_remote_set_callbacks( *out, &p->mRemoteCallbacks );

            return r.errorCode();
        }

        Remote::PrivatePtr BaseRemoteOperationPrivate::lookupRemote(Result& result, Repository::Private* repo, QString& remoteName)
        {
            repo->isValid( result, repo );
            GW_CHECK_RESULT( result, Remote::PrivatePtr() );

            if ( remoteName.isEmpty() )
            {
                StrArray remoteNames;
                result = git_remote_list(remoteNames, repo->mRepo);
                GW_CHECK_RESULT( result, Remote::PrivatePtr() );

                if ( remoteNames.count() == 1 )
                {
                    remoteName = remoteNames.strings().first();
                }
                else
                {
                    result.setError( GIT_ENOTFOUND );
                }
            }

            GW_CHECK_RESULT( result, Remote::PrivatePtr() );

            git_remote* remote = NULL;
            result = git_remote_lookup( &remote, repo->mRepo, GW_StringFromQt(remoteName) );
            GW_CHECK_RESULT( result, Remote::PrivatePtr() );

            return Remote::PrivatePtr( new RemotePrivate( repo, remote ) );
        }

        BaseRemoteOperationPrivate::BaseRemoteOperationPrivate(BaseRemoteOperation *owner)
            : BaseOperationPrivate( owner )
        {
            RemoteCallbacks::initCallbacks( mRemoteCallbacks, owner );
        }

        BaseRemoteOperationPrivate::~BaseRemoteOperationPrivate()
        {
        }


        //-- FetchOperationPrivate -->8

        FetchOperationPrivate::FetchOperationPrivate(FetchOperation *owner)
            : BaseRemoteOperationPrivate( owner )
        {
        }

        void FetchOperationPrivate::run()
        {
            GW_CHECK_RESULT(mResult, void());

            git_signature* sig = mSignature.isEmpty()
                                 ? NULL
                                 : signature2git(mResult, mSignature);

            Repository::Private* rp = Repository::Private::dataOf<Repository>( mRepo );
            Remote::PrivatePtr remote = lookupRemote( mResult, rp, mRemoteAlias );

            if ( mResult ) {
                mResult = git_remote_set_callbacks( remote->mRemote, &mRemoteCallbacks );
            }

            if ( mResult ) {
                mResult = git_remote_fetch( remote->mRemote, StrArray(mRefSpecs), sig, GW_StringFromQt( mRefLogMsg ) );
            }

            git_signature_free( sig );
        }


        //-- PushOperationPrivate -->8

        PushOperationPrivate::PushOperationPrivate(PushOperation *owner)
            : BaseRemoteOperationPrivate( owner )
        {
            mResult = git_push_init_options( &mOpts, GIT_PUSH_OPTIONS_VERSION );
        }

        void PushOperationPrivate::run()
        {
            GW_CHECK_RESULT(mResult, void());

            git_signature* sig = mSignature.isEmpty()
                                 ? NULL
                                 : signature2git(mResult, mSignature);

            Repository::Private* rp = Repository::Private::dataOf<Repository>( mRepo );
            Remote::PrivatePtr remote = lookupRemote( mResult, rp, mRemoteAlias );

            if ( mResult ) {
                mResult = git_remote_push( remote->mRemote, StrArray(mRefSpecs), &mOpts, sig, GW_StringFromQt( mRefLogMsg ) );
            }

            git_signature_free( sig );
        }
    }


    //-- BaseRemoteOperation -->8

    BaseRemoteOperation::BaseRemoteOperation(Private &_d, QObject* parent)
        : BaseOperation(_d, parent)
    {
    }

    BaseRemoteOperation::~BaseRemoteOperation()
    {
    }

    const Repository& BaseRemoteOperation::repository() const
    {
        GW_CD( BaseRemoteOperation );
        return d->mRepo;
    }

    /**
     * @brief       The remote alias represents the URI to a remote
     *              repository.
     *
     * @return      the remote alias (e.g. "origin")
     *
     * The alias can be an empty string, if the repository has only a single
     * remote. Most commonly, this is called "origin".
     */
    QString BaseRemoteOperation::remoteAlias() const
    {
        GW_CD(BaseRemoteOperation);
        return d->mRemoteAlias;
    }

    /**
     * @brief       Sets the alias for the URI to fetch the repository from.
     *
     * @param       alias   the alias represents a remote's URI
     *
     * The remote alias represents the URI to a remote repository. It must be
     * defined, if the repository has configured more than one remotes.
     */
    void BaseRemoteOperation::setRemoteAlias(const QString& alias)
    {
        Q_ASSERT( !isRunning() );
        GW_D(BaseRemoteOperation);
        d->mRemoteAlias = alias;
    }

    const QStringList& BaseRemoteOperation::refSpecs() const
    {
        GW_CD( BaseRemoteOperation );
        return d->mRefSpecs;
    }

    void BaseRemoteOperation::setRefSpecs(const QStringList& refSpecs)
    {
        Q_ASSERT( !isRunning() );
        GW_D( BaseRemoteOperation );
        d->mRefSpecs = refSpecs;
    }

    const Signature& BaseRemoteOperation::signature() const
    {
        GW_CD( BaseRemoteOperation );
        return d->mSignature;
    }

    void BaseRemoteOperation::setSignature(const Signature& sig)
    {
        Q_ASSERT( !isRunning() );
        GW_D( BaseRemoteOperation );
        d->mSignature = sig;
    }

    QString BaseRemoteOperation::refLogMessage() const
    {
        GW_CD( BaseRemoteOperation );
        return d->mRefLogMsg;
    }

    void BaseRemoteOperation::setRefLogMessage(const QString& msg)
    {
        Q_ASSERT( !isRunning() );
        GW_D( BaseRemoteOperation );
        d->mRefLogMsg = msg;
    }

    /**
     * @brief           Internal initialization of the repository used for the operation.
     *
     * @param repo      the git repository used, when the operation is executed
     */
    void BaseRemoteOperation::setRepository(const Repository& repo)
    {
        GW_D( BaseRemoteOperation );
        d->mRepo = repo;
    }


    //-- FetchOperation -->8

    FetchOperation::FetchOperation(const Repository& repo, QObject* parent )
        : BaseRemoteOperation( *new Private(this), parent )
    {
        setRepository( repo );
    }


    //-- PushOperation -->8

    PushOperation::PushOperation(const Repository& repo, QObject* parent)
        : BaseRemoteOperation( *new Private(this), parent )
    {
        setRepository( repo );
    }

    unsigned int PushOperation::pbParallellism() const
    {
        GW_CD( PushOperation );
        return d->mOpts.pb_parallelism;
    }

    void PushOperation::setPBParallelism(unsigned int maxThreads)
    {
        GW_D( PushOperation );
        d->mOpts.pb_parallelism = maxThreads;
    }

}
