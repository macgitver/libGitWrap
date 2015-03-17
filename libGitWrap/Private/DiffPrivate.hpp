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

#include "libGitWrap/Private/RepoObjectPrivate.hpp"


namespace Git
{

    namespace Internal
    {

        class DiffOptions : public QSharedData
        {
        public:
            typedef QExplicitlySharedDataPointer<DiffOptions> Ptr;

        public:
            DiffOptions(Result& result);
            DiffOptions(Result& result, git_diff_options* opts);
            ~DiffOptions();

        public:
            operator const git_diff_options*() const;
            git_diff_options& operator *();

        public:
            git_diff_options*   mOpts;
        };


        class DiffListPrivate : public RepoObjectPrivate
        {
        public:
            DiffListPrivate(const RepositoryPrivate::Ptr& repo, git_diff* diff);
            ~DiffListPrivate();

        public:
            git_diff* mDiff;
        };


        class DiffFilePrivate : public BasePrivate
        {
        public:
            DiffFilePrivate(const git_diff_file* diffFile);
            ~DiffFilePrivate();

        public:
            const git_diff_file* mDiffFile;
        };

    }

}
