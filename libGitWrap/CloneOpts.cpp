/*
 * MacGitver
 * Copyright (C) 2012-2013 Sascha Cunz <sascha@babbelbox.org>
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

#include "CloneOptsPrivate.hpp"
#include "CloneOpts.hpp"

namespace Git
{

    namespace Internal
    {

        CloneOptsPrivate::CloneOptsPrivate()
        {
            mGitCloneOptions = (git_clone_options) GIT_CLONE_OPTIONS_INIT;
        }

        CloneOptsPrivate::~CloneOptsPrivate()
        {
        }

        const git_clone_options* CloneOptsPrivate::asGitCloneOptions() const
        {
            return &mGitCloneOptions;
        }

    }

    CloneOpts::CloneOpts()
    {
        d = new Internal::CloneOptsPrivate;
    }

    CloneOpts::CloneOpts( const CloneOpts& other )
        : d( other.d )
    {
    }

    CloneOpts::~CloneOpts()
    {
    }

    CloneOpts& CloneOpts::operator=( const CloneOpts& other )
    {
        d = other.d;
        return *this;
    }

    bool CloneOpts::operator==( const CloneOpts& other ) const
    {
        return d == other.d || *d == *other.d;
    }

    void CloneOpts::setUrl( const QByteArray& url )
    {
        d->mUrl = url;
    }

    void CloneOpts::setPath( const QByteArray& path )
    {
        d->mPath = path;
    }

    void CloneOpts::setBare( bool bare )
    {
        d->mGitCloneOptions.bare = bare ? 0 : 1;
    }

    void CloneOpts::setRemoteName( const QByteArray& remoteName )
    {
        d->mRemoteName = remoteName;
        d->mGitCloneOptions.remote_name = remoteName.isEmpty() ? NULL : remoteName.constData();
    }

    void CloneOpts::setFetchSpec( const QByteArray& fetchSpec )
    {
        d->mFetchSpec = fetchSpec;
        d->mGitCloneOptions.fetch_spec = fetchSpec.isEmpty() ? NULL : fetchSpec.constData();
    }

    void CloneOpts::setPushSpec( const QByteArray& pushSpec )
    {
        d->mPushSpec = pushSpec;
        d->mGitCloneOptions.push_spec = pushSpec.isEmpty() ? NULL : pushSpec.constData();
    }

    void CloneOpts::setPushUrl( const QByteArray& pushUrl )
    {
        d->mPushUrl = pushUrl;
        d->mGitCloneOptions.pushurl = pushUrl.isEmpty() ? NULL : pushUrl.constData();
    }

    QByteArray CloneOpts::url() const
    {
        return d->mUrl;
    }

    QByteArray CloneOpts::path() const
    {
        return d->mPath;
    }

    bool CloneOpts::bare() const
    {
        return d->mGitCloneOptions.bare != 0;
    }

    QByteArray CloneOpts::remoteName() const
    {
        return d->mRemoteName;
    }

    QByteArray CloneOpts::fetchSpec() const
    {
        return d->mFetchSpec;
    }

    QByteArray CloneOpts::pushSpec() const
    {
        return d->mPushSpec;
    }

    QByteArray CloneOpts::pushUrl() const
    {
        return d->mPushUrl;
    }

}
