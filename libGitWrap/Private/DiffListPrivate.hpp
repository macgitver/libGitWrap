/*
 * MacGitver
 * Copyright (C) 2012-2013 Sascha Cunz <sascha@babbelbox.org>
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

#ifndef GIT_DIFFLIST_PRIVATE_H
#define GIT_DIFFLIST_PRIVATE_H

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/RepoObjectPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        /**
         * @internal
         * @ingroup     GitWrap
         *
         * @brief       The DiffListPrivate class
         */
        class DiffListPrivate : public RepoObjectPrivate
        {
        public:
            explicit DiffListPrivate(const RepositoryPrivate::Ptr& repo, git_diff* diff);
            ~DiffListPrivate();

        public:
            git_diff* mDiff;
        };

        /**
          * @internal
          * @ingroup GritWrap
          *
          * @brief Wrapper for git_diff_file.
          */
        class DiffFilePrivate : public RepoObjectPrivate
        {
        public:
            explicit DiffFilePrivate( const RepositoryPrivate::Ptr& repo, const git_diff_file* diffFile );
            explicit DiffFilePrivate( RepositoryPrivate* repo, const git_diff_file* diffFile );
            ~DiffFilePrivate();

        public:
            const git_diff_file* mDiffFile;
        };

    }

}

#endif
