/*
 * MacGitver
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
 * (C) Cunz RaD Ltd.
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

#include "libGitWrap/Config.hpp"

#include "libGitWrap/Private/ConfigPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        ConfigPrivate::ConfigPrivate( git_config* cfg )
            : mCfg( cfg )
        {
        }

        ConfigPrivate::~ConfigPrivate()
        {
            git_config_free( mCfg );
        }

    }

    GW_PRIVATE_IMPL(Config, Base)

    QString Config::globalFilePath()
    {
        QString filePath;
        git_buf path = {0};

        int rc = git_config_find_system( &path );
        if( rc >= 0 )
        {
            filePath = QString::fromLocal8Bit( path.ptr );
        }

        git_buf_free( &path );
        return filePath;
    }

    QString Config::userFilePath()
    {
        QString filePath;
        git_buf path = {0};

        int rc = git_config_find_global( &path );
        if( rc >= 0 )
        {
            filePath = QString::fromLocal8Bit( path.ptr );
        }

        git_buf_free( &path );
        return filePath;
    }

    Config Config::global()
    {
        git_buf path = {0};

        int rc = git_config_find_system( &path );
        if( rc < 0 )
        {
            git_buf_free( &path );
            return Config();
        }

        git_config* cfg = NULL;
        rc = git_config_open_ondisk( &cfg, path.ptr );
        if( rc < 0 )
        {
            git_buf_free( &path );
            return Config();
        }

        git_buf_free( &path );
        return PrivatePtr(new Private(cfg));
    }

    Config Config::user()
    {
        git_buf path = {0};

        int rc = git_config_find_global( &path );
        if( rc < 0 )
        {
            git_buf_free( &path );
            return Config();
        }

        git_config* cfg = NULL;
        rc = git_config_open_ondisk( &cfg, path.ptr );
        if( rc < 0 )
        {
            git_buf_free( &path );
            return Config();
        }

        git_buf_free( &path );
        return PrivatePtr(new Private(cfg));
    }

    Config Config::file( const QString& fileName )
    {
        git_config* cfg = NULL;

        int rc = git_config_open_ondisk( &cfg, fileName.toLocal8Bit().constData() );

        if( rc < 0 )
        {
            return Config();
        }

        return PrivatePtr(new Private(cfg));
    }

    Config Config::create()
    {
        git_config* cfg = NULL;
        int rc = git_config_new( &cfg );

        if( rc < 0 )
        {
            return Config();
        }

        return PrivatePtr(new Private(cfg));
    }

    bool Config::addFile(const QString& fileName, int priority)
    {
        GW_D(Config);

        if( !d || fileName.isEmpty() )
        {
            return false;
        }

        int rc = git_config_add_file_ondisk( d->mCfg, fileName.toLocal8Bit().constData(), (git_config_level_t)priority, 0 );
        if( rc < 0 )
        {
            return false;
        }

        return true;
    }

    static int read_config_cb( const git_config_entry* entry, void* data )
    {
        ConfigValues* cv = (ConfigValues*) data;
        cv->insert( QString::fromUtf8( entry->name ),
                    QString::fromUtf8( entry->value ) );
        return 0;
    }

    ConfigValues Config::values() const
    {
        GW_CD(Config);
        ConfigValues values;
        git_config_foreach( d->mCfg, &read_config_cb, (void*) &values );
        return values;
    }

}

