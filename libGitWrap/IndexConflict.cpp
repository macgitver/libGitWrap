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

#include "IndexConflict.hpp"

#include "Private/IndexConflictPrivate.hpp"
#include "Private/IndexEntryPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        IndexConflictPrivate::IndexConflictPrivate(const git_index_entry *_from,
                                                   const git_index_entry *_ours,
                                                   const git_index_entry *_theirs)
            : from(new IndexEntryPrivate(_from))
            , ours(new IndexEntryPrivate(_ours))
            , theirs(new IndexEntryPrivate(_theirs))
        {
        }

        IndexConflictPrivate::IndexConflictPrivate(const IndexEntry &_from,
                                                   const IndexEntry &_ours,
                                                   const IndexEntry &_theirs)
            : from(_from)
            , ours(_ours)
            , theirs(_theirs)
        {
        }

        IndexConflictPrivate::~IndexConflictPrivate()
        {
        }

    }

    IndexConflict::IndexConflict()
    {
    }

    IndexConflict::IndexConflict(const IndexConflict& other)
        : d(other.d)
    {
    }

    IndexConflict::IndexConflict(Internal::IndexConflictPrivate* _d)
        : d(_d)
    {
    }

    IndexConflict::IndexConflict(const IndexEntry& from,
                                 const IndexEntry& ours,
                                 const IndexEntry& theirs)
    {
        d = new Internal::IndexConflictPrivate(from, ours, theirs);
    }

    bool IndexConflict::isValid() const
    {
        return d;
    }

    IndexConflict::~IndexConflict()
    {
    }

    IndexConflict& IndexConflict::operator=(const IndexConflict& other)
    {
        d = other.d;
        return *this;
    }

    IndexEntry IndexConflict::from() const
    {
        return d ? d->from : IndexEntry();
    }

    IndexEntry IndexConflict::ours() const
    {
        return d ? d->ours : IndexEntry();
    }

    IndexEntry IndexConflict::theirs() const
    {
        return d ? d->theirs : IndexEntry();
    }

}
