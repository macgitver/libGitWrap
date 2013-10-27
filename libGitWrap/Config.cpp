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

#include "Config.hpp"

#include "Private/ConfigPrivate.hpp"

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

    Config::Config()
    {
    }

    Config::Config(Internal::ConfigPrivate& _d)
        : Base(_d)
    {
    }

    QString Config::globalFilePath()
    {
        QString filePath;
        char path[ 2048 ];

        int rc = git_config_find_system( path, 2048 - 1 );
        if( rc >= 0 )
        {
            filePath = QString::fromLocal8Bit( path );
        }

        return filePath;
    }

    QString Config::userFilePath()
    {
        QString filePath;
        char path[ 2048 ];

        int rc = git_config_find_global( path, 2048 - 1 );
        if( rc >= 0 )
        {
            filePath = QString::fromLocal8Bit( path );
        }

        return filePath;
    }

    Config Config::global()
    {
        char path[ 2048 ];

        int rc = git_config_find_system( path, 2048 - 1 );
        if( rc < 0 )
        {
            return Config();
        }

        git_config* cfg = NULL;
        rc = git_config_open_ondisk( &cfg, path );
        if( rc < 0 )
        {
            return Config();
        }

        return *new Internal::ConfigPrivate(cfg);
    }

    Config Config::user()
    {
        char path[ 2048 ];

        int rc = git_config_find_global( path, 2048 - 1 );
        if( rc < 0 )
        {
            return Config();
        }

        git_config* cfg = NULL;
        rc = git_config_open_ondisk( &cfg, path );
        if( rc < 0 )
        {
            return Config();
        }

        return *new Internal::ConfigPrivate(cfg);
    }

    Config Config::file( const QString& fileName )
    {
        git_config* cfg = NULL;

        int rc = git_config_open_ondisk( &cfg, fileName.toLocal8Bit().constData() );

        if( rc < 0 )
        {
            return Config();
        }

        return *new Internal::ConfigPrivate(cfg);
    }

    Config Config::create()
    {
        git_config* cfg = NULL;
        int rc = git_config_new( &cfg );

        if( rc < 0 )
        {
            return Config();
        }

        return *new Internal::ConfigPrivate(cfg);
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

    ConfigValues Config::values()
    {
        GW_D(Config);
        ConfigValues values;
        git_config_foreach( d->mCfg, &read_config_cb, (void*) &values );
        return values;
    }

}

