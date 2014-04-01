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

#ifndef GITWRAP_COMMIT_PRIVATE_HPP
#define GITWRAP_COMMIT_PRIVATE_HPP
#pragma once

#include "libGitWrap/Private/ObjectPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        class CommitPrivate : public ObjectPrivate
        {
        public:
            CommitPrivate(const RepositoryPrivate::Ptr& repo, git_object* o);
            CommitPrivate(const RepositoryPrivate::Ptr& repo, git_commit* o);

        public:
            git_commit* o()             { return reinterpret_cast<git_commit*>(mObj); }
            const git_commit* o() const { return reinterpret_cast<git_commit*>(mObj); }

            git_otype otype() const;
            ObjectType objectType() const;

        public:
            static const git_commit** commitList2git(Result& result, const CommitList& list);
        };

    }

}

#endif
