/*
 * MacGitver
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

#pragma once

#include "libGitWrap/Private/ObjectPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        class TreePrivate : public ObjectPrivate
        {
        public:
            typedef QExplicitlySharedDataPointer<TreePrivate> Ptr;

        public:
            TreePrivate(RepositoryPrivate* repo, git_object* o);
            TreePrivate(RepositoryPrivate* repo, git_tree* o);

        public:
            git_tree* o()               { return reinterpret_cast<git_tree*>(mObj); }
            const git_tree* o() const   { return reinterpret_cast<git_tree*>(mObj); }

            git_otype otype() const;
            ObjectType objectType() const;
        };

    }

}
