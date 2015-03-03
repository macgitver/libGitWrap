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

#pragma once

#include "libGitWrap/Base.hpp"

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
    };

}

Q_DECLARE_METATYPE(Git::Config)
