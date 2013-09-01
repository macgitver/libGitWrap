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

#ifndef GIT_INDEX_ENTRY_PRIVATE_HPP
#define GIT_INDEX_ENTRY_PRIVATE_HPP

#include "BasicObject.hpp"

namespace Git
{
    namespace Internal
    {

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       The IndexEntryPrivate class
         */
        class IndexEntryPrivate : public BasicObject
        {
        public:
            IndexEntryPrivate(const git_index_entry *entry);
            ~IndexEntryPrivate();

        public:
            git_index_entry     mEntry;
        };

    }
}

#endif
