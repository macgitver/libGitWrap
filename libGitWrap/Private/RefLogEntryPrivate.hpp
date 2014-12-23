/*
 * MacGitver
 * Copyright (C) 2014 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Nils Fenner <nils@macgitver.org>
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

#ifndef GITWRAP_REFLOGENTRY_HPP
#define GITWRAP_REFLOGENTRY_HPP

#include "libGitWrap/Private/BasePrivate.hpp"

namespace Git
{
    namespace Internal
    {

        /**
         * @internal
         * @ingroup     GitWrap
         *
         * @brief       Internal wrapper for git_reflog_entry from libgit2.
         */
        class RefLogEntryPrivate : public BasePrivate
        {
        public:
            explicit RefLogEntryPrivate(const git_reflog_entry *entry);
            ~RefLogEntryPrivate();

        public:
            const git_reflog_entry *    mEntry;
        };
    }
}

#endif // REFLOGENTRY_HPP

