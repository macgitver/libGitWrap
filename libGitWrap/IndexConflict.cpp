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

#include "libGitWrap/IndexConflict.hpp"

#include "libGitWrap/Private/IndexConflictPrivate.hpp"
#include "libGitWrap/Private/IndexEntryPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        IndexConflictPrivate::IndexConflictPrivate(const git_index_entry *_from,
                                                   const git_index_entry *_ours,
                                                   const git_index_entry *_theirs)
            : from  (new IndexEntryPrivate(_from))
            , ours  (new IndexEntryPrivate(_ours))
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

    GW_PRIVATE_IMPL(IndexConflict, Base)

    IndexConflict IndexConflict::create(const IndexEntry& from,
                                        const IndexEntry& ours,
                                        const IndexEntry& theirs)
    {
        return new Private(from, ours, theirs);
    }

    IndexEntry IndexConflict::from() const
    {
        GW_CD(IndexConflict);
        return d ? d->from : IndexEntry();
    }

    IndexEntry IndexConflict::ours() const
    {
        GW_CD(IndexConflict);
        return d ? d->ours : IndexEntry();
    }

    IndexEntry IndexConflict::theirs() const
    {
        GW_CD(IndexConflict);
        return d ? d->theirs : IndexEntry();
    }

}
