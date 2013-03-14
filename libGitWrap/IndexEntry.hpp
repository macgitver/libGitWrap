/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Nils Fenner <nilsfenner@web.de>
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

#ifndef GIT_INDEX_ENTRY_H
#define GIT_INDEX_ENTRY_H

#include "GitWrap.hpp"

#include <QTime>

struct git_index_entry;

namespace Git
{
    class ObjectId;

    /**
     * @ingroup GitWrap
     * @brief The IndexEntry class provides information about a Git index entry.
     * Each instance of IndexEntry encapsulates a git_index_entry struct from libgit2.
     */
    class GITWRAP_API IndexEntry
    {
    public:
        IndexEntry();
        IndexEntry(const IndexEntry &other);
        IndexEntry(const git_index_entry *entry);
        ~IndexEntry();

        IndexEntry& operator=( const IndexEntry& other );

    public:
        bool isValid() const;

        QString path() const;
        ObjectId blobSha() const;
        unsigned int mode() const;
        unsigned int ino() const;
        unsigned int uid() const;
        unsigned int gid() const;
        QTime cTime() const;
        QTime mTime() const;
        int stage() const;

    private:
        git_index_entry *   d;
    };

}

#endif
