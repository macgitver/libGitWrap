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

#ifndef GITWRAP_CONFIG_H
#define GITWRAP_CONFIG_H

#include "libGitWrap/Base.hpp"

// -- DEPRECATED INCLUDES BEGIN --8>

#include "libGitWrap/Result.hpp"

// <8-- DEPRECATED INCLUDES END --

namespace Git
{

    namespace Internal
    {
        class ConfigPrivate;
    }

    typedef QHash< QString, QString > ConfigValues;

    /**
     * @ingroup     GitWrap
     * @brief       Provides access git configuration files
     *
     */
    class GITWRAP_API Config : public Base
    {
        GW_PRIVATE_DECL(Config, Base, public)

    public:
        bool addFile( Result &result, const QString& fileName, int priority );

        ConfigValues values( Result& result ) const;

    public:
        static QString globalFilePath( Result& result );
        static QString userFilePath( Result& result );

        static Config global( Result &result );
        static Config user( Result &result );
        static Config file( Result& result, const QString& fileName );

        static Config create( Result &result );

    public:
        // -- DEPRECATED FUNCTION BEGIN --8>

        /**
         * @brief Deprecated: Config::addFile
         * @deprecated Use @ref Config::addFile( Result &result, const QString& fileName, int priority ) instead.
         */
        GW_DEPRECATED inline bool addFile( const QString& fileName, int priority )
        {
            Result r;
            return addFile( r, fileName, priority );
        }

        /**
         * @brief Deprecated: Config::values
         * @deprecated Use @ref Config::values( Result &result ) instead.
         */
        GW_DEPRECATED inline ConfigValues values() const
        {
            Result r;
            return values( r );
        }

        /**
         * @brief Deprecated: Config::globalFilePath
         * @deprecated Use Config::globalFilePath( Result &result ) instead.
         */
        GW_DEPRECATED static QString globalFilePath()
        {
            Result r;
            return globalFilePath( r );
        }

        /**
         * @brief Deprecated: Config::userFilePath
         * @deprecated Use Config::userFilePath( Result &result ) instead.
         */
        GW_DEPRECATED static QString userFilePath()
        {
            Result r;
            return userFilePath( r );
        }

        /**
         * @brief Deprecated: Config::global
         * @deprecated Use Config::global( Result &result ) instead.
         */
        GW_DEPRECATED static Config global()
        {
            Result r;
            return global( r );
        }

        /**
         * @brief Deprecated: Config::user
         * @deprecated Use Config::user( Result &result ) instead.
         */
        GW_DEPRECATED static Config user()
        {
            Result r;
            return user(r);
        }

        /**
         * @brief Deprecated: Config::file
         * @deprecated Use Config::file( Result &result, const QString& fileName ) instead.
         */
        GW_DEPRECATED static Config file( const QString& fileName )
        {
            Result r;
            return file( r, fileName );
        }

        /**
         * @brief Deprecated: Config::create
         * @deprecated Use Config::create( Result &result ) instead.
         */
        GW_DEPRECATED static Config create()
        {
            Result r;
            return create( r );
        }

        // <8-- DEPRECATED FUNCTIONS END --
    };

}

Q_DECLARE_METATYPE(Git::Config)

#endif
