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

#ifndef GITWRAP_CLONEOPTS_H
#define GITWRAP_CLONEOPTS_H

#include "GitWrap.hpp"

namespace Git
{
    namespace Internal
    {
        class CloneOptsPrivate;
    }

    class CloneOpts
    {
    public:
        CloneOpts();
        CloneOpts( const CloneOpts& other );
        ~CloneOpts();
        CloneOpts& operator=( const CloneOpts& other );

        bool operator==( const CloneOpts& other ) const;

    public:
        void setUrl( const QByteArray& url );
        void setPath( const QByteArray& path );

    public:
        void setBare( bool bare );
        void setRemoteName( const QByteArray& remoteName );
        void setFetchSpec( const QByteArray& fetchSpec );
        void setPushSpec( const QByteArray& pushSpec );
        void setPushUrl( const QByteArray& pushUrl );

    private:
        QByteArray url() const;
        QByteArray path() const;
        bool bare() const;

        QByteArray remoteName() const;
        QByteArray fetchSpec() const;
        QByteArray pushSpec() const;
        QByteArray pushUrl() const;

    private:
        Internal::GitPtr< Internal::CloneOptsPrivate > d;
    };

}

#endif
