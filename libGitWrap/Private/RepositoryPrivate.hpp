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

#pragma once

#include "libGitWrap/Private/BasePrivate.hpp"
#include "libGitWrap/Private/GitWrapPrivate.hpp"

#include "libGitWrap/Submodule.hpp"

namespace Git
{

    namespace Internal
    {
        class StatusPrivate;

        /**
         * @internal
         * @ingroup GitWrap
         * @brief The RepositoryPrivate class
         *
         */
        class RepositoryPrivate : public BasePrivate
        {
        public:
            typedef GitPtr<RepositoryPrivate> Ptr;

        public:
            RepositoryPrivate( git_repository* repo );
            ~RepositoryPrivate();

        public:
            Reference getHead(Result& result) const;

        public:
            git_repository* mRepo;
            IndexPrivate*   mIndex;
            Submodule       openedFrom;
        };

    }

}
