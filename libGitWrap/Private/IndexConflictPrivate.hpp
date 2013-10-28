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

#ifndef GIT_INDEX_CONFLICT_PRIVATE_HPP
#define GIT_INDEX_CONFLICT_PRIVATE_HPP

#include "libGitWrap/Private/GitWrapPrivate.hpp"

#include "libGitWrap/Private/BasePrivate.hpp"
#include "libGitWrap/IndexEntry.hpp"

namespace Git
{

    namespace Internal
    {

        class IndexConflictPrivate : public BasePrivate
        {
        public:
            IndexConflictPrivate(const IndexEntry& _from, const IndexEntry& _ours,
                                 const IndexEntry& _theirs );
            IndexConflictPrivate(const git_index_entry* _from, const git_index_entry* _ours,
                                 const git_index_entry* _theirs);
            IndexConflictPrivate();
            ~IndexConflictPrivate();

        public:
            const IndexEntry    from;
            const IndexEntry    ours;
            const IndexEntry    theirs;
        };

    }

}

#endif
