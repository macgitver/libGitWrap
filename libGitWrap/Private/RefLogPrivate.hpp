/*
 * libGitWrap - A Qt wrapper library for libgit2
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

#ifndef GIT_REF_LOG_PRIVATE_HPP
#define GIT_REF_LOG_PRIVATE_HPP

#include "libGitWrap/Private/RepoObjectPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        class RefLogPrivate : public RepoObjectPrivate
        {
        public:
            RefLogPrivate( RepositoryPrivate* repo, git_reflog* _reflog );
            RefLogPrivate( const RepositoryPrivate::Ptr& repo, git_reflog* _reflog );
            ~RefLogPrivate();

        public:
            git_reflog*         reflog;
        };

    }

}

#endif
