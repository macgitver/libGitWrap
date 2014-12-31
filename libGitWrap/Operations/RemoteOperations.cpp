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

#include "libGitWrap/Events/Private/GitEventCallbacks.hpp"

#include "Private/RemoteOperationsPrivate.hpp"


namespace Git
{

    namespace Internal
    {

        //-- BaseRemoteOperationPrivate -->8

        int BaseRemoteOperationPrivate::CB_GetRemote(git_remote** out, git_repository* repo, const char* name, const char* url, void* payload)
        {
            // Do not modify "out" here!
            // It is already configured correctly.

            Q_UNUSED( repo )
            Q_UNUSED( name )
            Q_UNUSED( url )

            RemotePrivate* p = static_cast< RemotePrivate* >( payload );
            Q_ASSERT( p );

            *out = p->mRemote;

            return 0;
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
            git_signature* sig = Internal::signature2git(mResult, mSignature);

            if ( mResult )
            {
                mResult = git_remote_fetch( mRemote->mRemote, StrArray(mRefSpecs), sig, GW_StringFromQt( mRefLogMsg ) );
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
            git_signature* sig = Internal::signature2git( mResult, mSignature );

            if ( mResult )
            {
                mResult = git_remote_push( mRemote->mRemote, StrArray(mRefSpecs), &mOpts, sig, GW_StringFromQt( mRefLogMsg ) );
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

    Remote BaseRemoteOperation::remote() const
    {
        GW_CD( BaseRemoteOperation );
        return d->mRemote;
    }

    void BaseRemoteOperation::setRemote(const Remote& remote)
    {
        GW_D( BaseRemoteOperation );
        Q_ASSERT( !isRunning() );
        d->mRemote = Remote::Private::dataOf<Remote>( remote );
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
