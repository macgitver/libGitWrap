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

#ifndef GIT_OBJECT_ID_H
#define GIT_OBJECT_ID_H

#include "GitWrap.hpp"

namespace Git
{

    /**
     * @ingroup     GitWrap
     * @brief       Represents an object identifier (OID) of a git object.
     *
     */
    class GITWRAP_API ObjectId
    {
    public:
        enum
        {
            SHA1_Length     = 20,
            SHA1_LengthHex  = 40
        };

    public:
        ObjectId();
        ObjectId( const QByteArray& raw );

    public:
        static ObjectId fromString( const QString& oid, int max = SHA1_LengthHex,
                                    bool *success = NULL );

        static ObjectId fromAscii( const QByteArray& oid, int max = SHA1_LengthHex,
                                   bool *success = NULL );

        static ObjectId fromRaw( const unsigned char* raw, int n = SHA1_Length );

        QString toString(int max = SHA1_LengthHex) const;
        QByteArray toAscii(int max = SHA1_LengthHex) const;

        const unsigned char* raw() const
        {
            return data;
        }

        unsigned char* rawWritable()
        {
            return data;
        }

        bool isNull() const;

        bool operator==( const ObjectId& other ) const;
        bool operator!=( const ObjectId& other ) const;

    private:
        unsigned char data[ SHA1_Length ];
    };

    typedef QVector< ObjectId > ObjectIdList;

    GITWRAP_API uint qHash( const ObjectId& sha1 );

}

GITWRAP_API QDebug operator<<( QDebug debug, const Git::ObjectId& id );

Q_DECLARE_METATYPE( Git::ObjectId )

#endif
