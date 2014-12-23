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

#include "libGitWrap/RefLog.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/RefLogPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        RefLogPrivate::RefLogPrivate(const RepositoryPrivate::Ptr& repo, git_reflog* _reflog)
            : RepoObjectPrivate(repo)
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

    GW_PRIVATE_IMPL(RefLog, RepoObject)

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


    int RefLog::numEntries()
    {
        // TODO: missing implementation
        return 0;
    }

    RefLogEntry RefLog::at(int index) const
    {
        // TODO: missing implementation
        return RefLogEntry();
    }

    RefLogEntry::RawList RefLog::rawEntries() const
    {
        // TODO: missing implementation
        return RefLogEntry::RawList();
    }

    void RefLog::read()
    {
        // TODO: missing implementation
    }

    bool RefLog::isRead() const
    {
        // TODO: missing implementation
        return true;
    }

}
