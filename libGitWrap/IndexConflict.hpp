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

#ifndef GIT_INDEX_CONFLICT_HPP
#define GIT_INDEX_CONFLICT_HPP

#include "libGitWrap/Base.hpp"

namespace Git
{

    namespace Internal
    {

        class IndexConflictPrivate;

    }

    class IndexConflict : public Base
    {
        GW_PRIVATE_DECL(IndexConflict, Base, public);

    public:
        IndexConflict();
        IndexConflict(const IndexConflict& other);
        IndexConflict(const IndexEntry& from, const IndexEntry& ours, const IndexEntry& theirs);

    public:
        IndexEntry from() const;
        IndexEntry ours() const;
        IndexEntry theirs() const;
    };

}

#endif
