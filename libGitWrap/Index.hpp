/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
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

#ifndef GIT_INDEX_H
#define GIT_INDEX_H

#include "GitWrap.hpp"

namespace Git
{

    class IndexEntry;
    class Repository;

    namespace Internal
    {
        class IndexPrivate;
    }

    /**
     * @ingroup     GitWrap
     * @brief       Provides access to the git index.
     *
     */
    class GITWRAP_API Index
    {
    public:
        explicit Index( Internal::IndexPrivate* _d );

    public:
        Index( bool create = false );
        Index( Result& result, const QString& path );
        Index( const Index& other );
        ~Index();

    public:
        Index& operator=( const Index& other );

    public:
        bool isValid() const;

        void read( Result& result );
        void write( Result& result );

        int count( Result& result ) const;
        Repository repository( Result& result ) const;

        IndexEntry getEntry(Result &result, int n) const;
        IndexEntry getEntry(Result &result, const QString &path) const;

        void addEntry(Result &result, const QString &path);
        void removeEntry(Result &result, const QString &path);

    private:
        Internal::GitPtr< Internal::IndexPrivate > d;
    };

}

Q_DECLARE_METATYPE( Git::Index )

#endif
