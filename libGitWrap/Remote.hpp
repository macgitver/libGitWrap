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

#ifndef GIT_REMOTE_H
#define GIT_REMOTE_H

#include "GitWrap.hpp"

namespace Git
{

    namespace Internal
    {
        class RemotePrivate;
    }

    class ObjectId;
    class Reference;
    class RefSpec;

    /**
     * @ingroup     GitWrap
     * @brief       Represents a git remote.
     *
     * Remotes are used to synchronize with foreign repositories, which may actually be on the local
     * or on a remote site.
     *
     */
    class GITWRAP_API Remote
    {
    public:
        Remote();
        Remote( Internal::RemotePrivate* _d );
        Remote( const Remote& other );
        ~Remote();
        Remote& operator=( const Remote& other );

    public:
        bool isValid() const;

        bool save( Result& result );

        QString name() const;
        QString url() const;

        bool addFetchSpec( Result& result, const QString& spec );
        bool addPushSpec( Result& result, const QString& spec );

        QVector<RefSpec> fetchSpecs() const;
        QVector<RefSpec> pushSpecs() const;

        static bool isValidUrl( const QString& url );
        static bool isSupportedUrl( const QString& url );

        bool connect( Result& result, bool forFetch );
        void disconnect( Result& result );
        bool download( Result& result );
        bool updateTips( Result& result );

    private:
        Internal::GitPtr< Internal::RemotePrivate > d;
    };

}

Q_DECLARE_METATYPE( Git::Remote )

#endif
