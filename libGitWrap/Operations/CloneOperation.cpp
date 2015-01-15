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

#include "CloneOperation.hpp"
#include "Private/CloneOperationPrivate.hpp"

#include "libGitWrap/Events/Private/GitEventCallbacks.hpp"

#include "libGitWrap/Private/RepositoryPrivate.hpp"


namespace Git
{

    namespace Internal
    {

        int CloneOperationPrivate::CB_CreateRepository(git_repository** out, const char* path, int bare, void* payload)
        {
            CloneOperationPrivate* p = static_cast< CloneOperationPrivate* >( payload );
            Q_ASSERT( p );

            GW_CHECK_RESULT( p->mResult, p->mResult.errorCode() );

            p->mResult = git_repository_init( out, path, bare );
            GW_CHECK_RESULT( p->mResult, p->mResult.errorCode() );

            return p->mResult.errorCode();
        }


        CloneOperationPrivate::CloneOperationPrivate(CloneOperation* owner)
            : BaseRemoteOperationPrivate( owner )
        {
            CheckoutOptionsRef coo = mCloneOpts.checkoutOptions();
            (*coo).checkout_strategy = GIT_CHECKOUT_SAFE_CREATE;

            CheckoutCallbacks::initCallbacks( coo, owner );

            (*mCloneOpts).repository_cb = CB_CreateRepository;
            (*mCloneOpts).repository_cb_payload = this;
        }

        void CloneOperationPrivate::run()
        {
            GW_CHECK_RESULT( mResult, void() );

            (*mCloneOpts).remote_cb = CB_CreateRemote;
            (*mCloneOpts).remote_cb_payload = this;

            git_repository* clone = NULL;
            mResult = git_clone(&clone, GW_StringFromQt(mUrl), GW_StringFromQt(mPath), mCloneOpts);
            GW_CHECK_RESULT( mResult, void() );
            mRepo = new RepositoryPrivate( clone );
        }

    }

    CloneOperation::CloneOperation(QObject* parent)
        : BaseRemoteOperation( *new Private(this), parent )
    {
    }

    void CloneOperation::setUrl(const QString& url)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mUrl = url;
    }

    void CloneOperation::setPath( const QString& path )
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        d->mPath = path;
    }

    void CloneOperation::setBare(bool bare)
    {
        GW_D(CloneOperation);
        Q_ASSERT(!isRunning());
        (*(d->mCloneOpts)).bare = bare;
    }

    void CloneOperation::setDepth(uint depth)
    {
        GW_D(CloneOperation);
        Q_ASSERT( !isRunning() );
        if ( depth ) {
            // TODO: not implemented in libgit2 api
            d->mResult.setError( "Setting the clone depth is not yet supported.", GIT_EUSER );
            qWarning( "%s: Missing implementation in libgit2 API.", __FUNCTION__ );
        }
    }

    void CloneOperation::setReference(const QString& refName)
    {
        GW_D( CloneOperation );
        Q_ASSERT( !isRunning() );
        d->mCloneOpts.setCheckoutBranch( refName );
    }

    void CloneOperation::setRemoteAlias(const QString& alias)
    {
        GW_D( CloneOperation );
        Q_ASSERT( !isRunning() );
        d->mRemoteAlias = alias;
    }

    QString CloneOperation::url() const
    {
        GW_CD(CloneOperation);
        return d->mUrl;
    }

    QString CloneOperation::path() const
    {
        GW_CD(CloneOperation);
        return d->mPath;
    }

    bool CloneOperation::bare() const
    {
        GW_CD(CloneOperation);
        return (*(d->mCloneOpts)).bare;
    }

    uint CloneOperation::depth() const
    {
        return 0;
    }

    QString CloneOperation::reference() const
    {
        GW_CD( CloneOperation );
        return d->mCloneOpts.checkoutBranch();
    }

    QString CloneOperation::remoteAlias() const
    {
        GW_CD( CloneOperation );
        return d->mRemoteAlias;
    }

}
