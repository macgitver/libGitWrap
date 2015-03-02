/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2015 The MacGitver-Developers <dev@macgitver.org>
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

#pragma once

#include "libGitWrap/Events/IGitEvents.hpp"

#include "libGitWrap/RepoObject.hpp"

namespace Git
{

    class DiffList;

    namespace Internal
    {
        class DiffBasePrivate;
        class DiffIndexPrivate;
        class DiffTreePrivate;
        class DiffRepoPrivate;

        class DiffOptions;
    }

    class GITWRAP_API Diff
    {
    public:
        Diff(Result& result);
        ~Diff();

    public:
        // index
        DiffList indexToWorkDir(Result& result, const Repository& repo) const;
        DiffList indexToWorkDir(Result& result, const Repository& repo, const Index& index) const;

        // tree
        DiffList treeToTree(Result& result, const Tree& oldTree, const Tree& newTree) const;
        DiffList treeToIndex(Result& result, const Tree& tree) const;
        DiffList treeToIndex(Result &result, const Repository &repo) const;
        DiffList treeToWorkDir(Result& result, const Tree& tree) const;
        DiffList treeToWorkDir(Result& result, const Repository& repo) const;
        DiffList treeToWorkDirWithIndex(Result& result, const Tree& tree) const;
        DiffList treeToWorkDirWithIndex(Result &result, const Repository &repo) const;

    public:
        void setFlags(quint32 flags);

    private:
        Internal::DiffOptions*  mOpts;
    };

}
