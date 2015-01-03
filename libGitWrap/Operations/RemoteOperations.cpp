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

            const char* alias = p->mRemoteAlias.isEmpty() ? name : GW_StringFromQt(p->mRemoteAlias);

            int error = 0;
            error = git_remote_create(out, repo, alias, url);

            return error;
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

        BaseRemoteOperationPrivate::BaseRemoteOperationPrivate(git_remote_callbacks& callbacks, BaseRemoteOperation *owner)
            : BaseOperationPrivate( owner )
        {
            RemoteCallbacks::initCallbacks( callbacks, owner );
        }

        BaseRemoteOperationPrivate::~BaseRemoteOperationPrivate()
        {
        }


        //-- FetchOperationPrivate -->8

        FetchOperationPrivate::FetchOperationPrivate(FetchOperation *owner)
            : BaseRemoteOperationPrivate( mRemoteCallbacks, owner )
        {
        }

        void FetchOperationPrivate::run()
        {
            git_signature* sig = signature2git(mResult, mSignature);

            Repository::Private* rp = Repository::Private::dataOf<Repository>( mRepo );
            Remote::PrivatePtr remote = lookupRemote( mResult, rp, mRemoteAlias );

            if ( mResult )
            {
                mResult = git_remote_fetch( remote->mRemote, StrArray(mRefSpecs), sig, GW_StringFromQt( mRefLogMsg ) );
            }

            git_signature_free( sig );
        }


        //-- PushOperationPrivate -->8

        PushOperationPrivate::PushOperationPrivate(PushOperation *owner)
            : BaseRemoteOperationPrivate(mRemoteCallbacks, owner)
        {
            git_push_init_options( &mOpts, GIT_PUSH_OPTIONS_VERSION );
        }

        void PushOperationPrivate::run()
        {
            git_signature* sig = signature2git( mResult, mSignature );

            Repository::Private* rp = Repository::Private::dataOf<Repository>( mRepo );
            Remote::PrivatePtr remote = lookupRemote( mResult, rp, mRemoteAlias );

            if ( mResult )
            {
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

    const QStringList& BaseRemoteOperation::refSpecs() const
    {
        GW_CD( BaseRemoteOperation );
        return d->mRefSpecs;
    }

    void BaseRemoteOperation::setRefSpecs(const QStringList& refSprecs)
    {
        GW_D( BaseRemoteOperation );
        Q_ASSERT( !isRunning() );
        d->mRefSpecs = refSprecs;
    }

    const Signature& BaseRemoteOperation::signature() const
    {
        GW_CD( BaseRemoteOperation );
        return d->mSignature;
    }

    void BaseRemoteOperation::setSignature(const Signature& sig)
    {
        GW_D( BaseRemoteOperation );
        Q_ASSERT( !isRunning() );
        d->mSignature = sig;
    }

    QString BaseRemoteOperation::refLogMessage() const
    {
        GW_CD( BaseRemoteOperation );
        return d->mRefLogMsg;
    }

    void BaseRemoteOperation::setRefLogMessage(const QString& msg)
    {
        GW_D( BaseRemoteOperation );
        Q_ASSERT( !isRunning() );
        d->mRefLogMsg = msg;
    }


    //-- FetchOperation -->8

    FetchOperation::FetchOperation( QObject* parent )
        : BaseRemoteOperation( *new Private(this), parent )
    {
    }


    //-- PushOperation -->8

    PushOperation::PushOperation(QObject* parent)
        : BaseRemoteOperation( *new Private(this), parent )
    {
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
