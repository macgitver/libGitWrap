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

#include "Remote.hpp"
#include "Reference.hpp"
#include "RefSpec.hpp"

#include "Private/RemotePrivate.hpp"

namespace Git
{

    namespace Internal
    {

        RemotePrivate::RemotePrivate( const GitPtr<RepositoryPrivate>& repo, git_remote* remote )
            : RepoObject( repo )
            , mRemote( remote )
        {
            Q_ASSERT( remote );
        }

        RemotePrivate::~RemotePrivate()
        {
            git_remote_free( mRemote );
        }

    }

    Remote::Remote()
    {
    }

    Remote::Remote( Internal::RemotePrivate* _d )
        : d( _d )
    {
    }

    Remote::Remote( const Remote& other )
        : d( other.d )
    {
    }

    Remote::~Remote()
    {
    }

    Remote& Remote::operator=( const Remote& other )
    {
        d = other.d;
        return * this;
    }

    bool Remote::isValid() const
    {
        return d;
    }

    bool Remote::save( Result& result )
    {
        if( !result )
        {
            return false;
        }
        if( !d )
        {
            result.setInvalidObject();
            return false;
        }

        result = git_remote_save( d->mRemote );
        return result;
    }

    QString Remote::name() const
    {
        if( !d )
        {
            GitWrap::lastResult().setInvalidObject();
            return QString();
        }

        return QString::fromUtf8( git_remote_name( d->mRemote ) );
    }

    QString Remote::url() const
    {
        if( !d )
        {
            GitWrap::lastResult().setInvalidObject();
            return QString();
        }

        return QString::fromUtf8( git_remote_url( d->mRemote ) );
    }

    bool Remote::addFetchSpec(Result& result, const QString& spec)
    {
        if( !result )
        {
            return false;
        }
        if( !d )
        {
            result.setInvalidObject();
            return false;
        }

        result = git_remote_add_fetch( d->mRemote, spec.toUtf8().constData() );
        return result;
    }

    bool Remote::addPushSpec(Result& result, const QString& spec)
    {
        if( !result )
        {
            return false;
        }
        if( !d )
        {
            result.setInvalidObject();
            return false;
        }

        result = git_remote_add_push( d->mRemote, spec.toUtf8().constData() );
        return result;
    }

    QVector<RefSpec> Remote::fetchSpecs() const
    {
        if( !d )
        {
            GitWrap::lastResult().setInvalidObject();
            return QVector<RefSpec>();
        }

        QVector<RefSpec> result;
        for (size_t i = 0; i < git_remote_refspec_count( d->mRemote ); ++i)
        {
            const git_refspec *spec = git_remote_get_refspec( d->mRemote, i );
            if ( git_refspec_direction( spec ) == GIT_DIRECTION_FETCH )
                result << Internal::mkRefSpec( spec );
        }

        return result;
    }

    QVector<RefSpec> Remote::pushSpecs() const
    {
        if( !d )
        {
            GitWrap::lastResult().setInvalidObject();
            return QVector<RefSpec>();
        }

        QVector<RefSpec> result;
        for (size_t i = 0; i < git_remote_refspec_count( d->mRemote ); ++i)
        {
            const git_refspec *spec = git_remote_get_refspec( d->mRemote, i );
            if ( git_refspec_direction( spec ) == GIT_DIRECTION_PUSH )
                result << Internal::mkRefSpec( spec );
        }

        return result;
    }

    bool Remote::isValidUrl( const QString& url )
    {
        return git_remote_valid_url( url.toUtf8().constData() );
    }

    bool Remote::isSupportedUrl( const QString& url )
    {
        return git_remote_supported_url( url.toUtf8().constData() );
    }


    bool Remote::connect(Result& result, bool forFetch)
    {
        if( !result )
        {
            return false;
        }

        if( !d )
        {
            result.setInvalidObject();
            return false;
        }

        result = git_remote_connect( d->mRemote, forFetch ? GIT_DIRECTION_FETCH
                                                          : GIT_DIRECTION_PUSH );
        return result;
    }

    void Remote::disconnect( Result& result )
    {
        if( !result )
        {
            return;
        }

        if( !d )
        {
            result.setInvalidObject();
            return;
        }

        git_remote_disconnect( d->mRemote );
    }

    namespace Internal
    {

        int download_progress( const git_transfer_progress* prg, void* _d )
        {
            RemotePrivate* d = (RemotePrivate*) _d;
            d->mStats = *prg;
            return 0;
        }

    }

    bool Remote::download( Result& result )
    {
        if( !result )
        {
            return false;
        }

        if( !d )
        {
            result.setInvalidObject();
            return false;
        }

        result = git_remote_download( d->mRemote, &Internal::download_progress,
                                      (Internal::RemotePrivate*) d );
        return result;
    }

    bool Remote::updateTips( Result& result )
    {
        if( !result )
        {
            return false;
        }

        if( !d )
        {
            result.setInvalidObject();
            return false;
        }

        result = git_remote_update_tips( d->mRemote );
        return result;
    }

}

