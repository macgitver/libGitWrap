/*
 * libGitWrap - A Qt wrapper library for libgit2
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

#ifndef GIT_REFLOG_HPP
#define GIT_REFLOG_HPP

#include "GitWrap.hpp"

#include "Signature.hpp"
#include "ObjectId.hpp"

namespace Git
{

    namespace Internal
    {
        class RefLogPrivate;
    }

    class GITWRAP_API RefLogEntry
    {
    public:
        struct Raw
        {
            QDateTime           timeStamp;
            Signature           comitter;
            ObjectId            shaOld;
            ObjectId            shaNew;
        };

        typedef QVector< Raw > RawList;

    public:
        explicit RefLogEntry(Internal::RefLogPrivate* _d, int index);

    public:
        RefLogEntry();
        RefLogEntry(const RefLogEntry& other);
        ~RefLogEntry();

    public:
        RefLogEntry& operator=(const RefLogEntry& other);
        bool isValid() const;

    public:
        RefLog log() const;

        const Raw& raw() const;

    private:
        Internal::GitPtr< Internal::RefLogPrivate > d;
        int index;
    };

    class GITWRAP_API RefLog
    {
    public:
        explicit RefLog(Internal::RefLogPrivate* _d);

        RefLog();
        RefLog(const RefLog& other);
        ~RefLog();

    public:
        RefLog& operator=(const RefLog& other);
        bool isValid() const;

    public:
        int numEntries();
        RefLogEntry at(int index) const;

        RefLogEntry::RawList rawEntries() const;

        void read();
        bool isRead() const;

    private:
        Internal::GitPtr< Internal::RefLogPrivate > d;
    };

}

#endif
