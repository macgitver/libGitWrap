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
        }

        void PushOperationPrivate::run()
        {

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


    //-- FetchOperation -->8

    FetchOperation::FetchOperation( QObject* parent )
        : BaseRemoteOperation( *new Private(this), parent )
    {
    }

    Remote FetchOperation::remote() const
    {
        GW_CD( FetchOperation );
        return d->mRemote;
    }

    void FetchOperation::setRemote(const Remote& remote)
    {
        GW_D( FetchOperation );
        Q_ASSERT( !isRunning() );
        d->mRemote = Remote::Private::dataOf<Remote>( remote );
    }

    const QStringList& FetchOperation::refSpecs() const
    {
        GW_CD( FetchOperation );
        return d->mRefSpecs;
    }

    void FetchOperation::setRefSpecs(const QStringList& refSprecs)
    {
        GW_D( FetchOperation );
        Q_ASSERT( !isRunning() );
        d->mRefSpecs = refSprecs;
    }

    const Signature& FetchOperation::signature() const
    {
        GW_CD( FetchOperation );
        return d->mSignature;
    }

    void FetchOperation::setSignature(const Signature& sig)
    {
        GW_D( FetchOperation );
        Q_ASSERT( !isRunning() );
        d->mSignature = sig;
    }

    QString FetchOperation::refLogMessage() const
    {
        GW_CD( FetchOperation );
        return d->mRefLogMsg;
    }

    void FetchOperation::setRefLogMessage(const QString& msg)
    {
        GW_D( FetchOperation );
        Q_ASSERT( !isRunning() );
        d->mRefLogMsg = msg;
    }


    //-- PushOperation -->8

    PushOperation::PushOperation(QObject* parent)
        : BaseRemoteOperation( *new Private(this), parent )
    {
    }

}
