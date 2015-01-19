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
        Result r;
        return globalFilePath( r );
    }

    QString Config::globalFilePath(Result &result)
    {
        GW_CHECK_RESULT( result, QString() );

        Internal::Buffer path;
        result = git_config_find_system( path );
        GW_CHECK_RESULT( result, QString() );

        return path.toString();
    }

    QString Config::userFilePath()
    {
        Result r;
        return userFilePath( r );
    }

    QString Config::userFilePath( Result& result)
    {
        GW_CHECK_RESULT( result, QString() );

        Internal::Buffer path;
        result = git_config_find_global( path );
        GW_CHECK_RESULT( result, QString() );

        return path.toString();
    }

    Config Config::global()
    {
        Result r;
        return global( r );
    }

    Config Config::global(Result &result)
    {
        GW_CHECK_RESULT( result, Config() );

        Internal::Buffer path;
        git_config* cfg = NULL;

        result = git_config_find_system( path );
        GW_CHECK_RESULT( result, Config() );

        result = git_config_open_ondisk( &cfg, path );
        GW_CHECK_RESULT( result, Config() );

        return PrivatePtr(new Private(cfg));
    }

    Config Config::user()
    {
        Result r;
        return user(r);
    }

    Config Config::user(Result &result)
    {
        GW_CHECK_RESULT( result, Config() );

        Internal::Buffer path;
        git_config* cfg = NULL;

        result = git_config_find_global( path );
        GW_CHECK_RESULT( result, Config() );

        result = git_config_open_ondisk( &cfg, path );
        GW_CHECK_RESULT( result, Config() );

        return PrivatePtr(new Private(cfg));
    }

    Config Config::file( const QString& fileName )
    {
        Result r;
        return file( r, fileName );
    }

    Config Config::file( Result& result, const QString& fileName )
    {
        GW_CHECK_RESULT( result, Config() );
        git_config* cfg = NULL;

        result = git_config_open_ondisk( &cfg, fileName.toLocal8Bit().constData() );
        GW_CHECK_RESULT( result, Config() );

        return PrivatePtr(new Private(cfg));
    }

    Config Config::create()
    {
        Result r;
        return create( r );
    }

    Config Config::create(Result& result)
    {
        GW_CHECK_RESULT( result, Config() );

        git_config* cfg = NULL;
        result = git_config_new( &cfg );
        GW_CHECK_RESULT( result, Config() );

        return PrivatePtr(new Private(cfg));
    }

    bool Config::addFile(const QString& fileName, int priority)
    {
        Result r;
        return addFile( r, fileName, priority );
    }

    bool Config::addFile(Result& result, const QString& fileName, int priority)
    {
        GW_D(Config);

        if( !d || fileName.isEmpty() )
        {
            return false;
        }

        result = git_config_add_file_ondisk( d->mCfg, fileName.toLocal8Bit().constData(), (git_config_level_t)priority, 0 );

        return result;
    }

    static int read_config_cb( const git_config_entry* entry, void* data )
    {
        ConfigValues* cv = (ConfigValues*) data;
        cv->insert( GW_StringToQt( entry->name ),
                    GW_StringToQt( entry->value ) );
        return 0;
    }

    ConfigValues Config::values() const
    {
        Result r;
        return values( r );
    }

    ConfigValues Config::values(Result &result) const
    {
        GW_CD(Config);
        ConfigValues values;
        result = git_config_foreach( d->mCfg, &read_config_cb, (void*) &values );
        GW_CHECK_RESULT( result, ConfigValues() );

        return values;
    }

}
