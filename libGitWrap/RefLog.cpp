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

#include "GitWrapPrivate.hpp"
#include "RefLogPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        RefLogPrivate::RefLogPrivate(const GitPtr<RepositoryPrivate>& repo, git_reflog* _reflog)
            : RepoObject(repo)
        {
            reflog = _reflog;
        }

        RefLogPrivate::~RefLogPrivate()
        {
            if (reflog) {
                git_reflog_free(reflog);
            }
        }

    }

    RefLogEntry::RefLogEntry()
    {
    }

    RefLogEntry::RefLogEntry(Internal::RefLogPrivate *_d, int _index)
        : d(_d)
        , index(_index)
    {
    }

    RefLogEntry::~RefLogEntry()
    {
    }


    RefLog::RefLog()
    {
    }

    RefLog::RefLog(Internal::RefLogPrivate *_d)
        : d(_d)
    {
    }

    RefLog::~RefLog()
    {
    }

    RefLog& RefLog::operator=(const RefLog& other)
    {
        d = other.d;
        return *this;
    }

    bool RefLog::isValid() const
    {
        return d;
    }

    int RefLog::numEntries()
    {
        return 0;
    }

    RefLogEntry RefLog::at(int index) const
    {
        return RefLogEntry();
    }

    RefLogEntry::RawList RefLog::rawEntries() const
    {
        return RefLogEntry::RawList();
    }

    void RefLog::read()
    {

    }

    bool RefLog::isRead() const
    {
        return true;
    }

}
