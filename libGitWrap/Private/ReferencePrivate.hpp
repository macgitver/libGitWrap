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

#ifndef GIT_REFERENCE_PRIVATE_HPP
#define GIT_REFERENCE_PRIVATE_HPP
#pragma once

#include "libGitWrap/Private/RefNamePrivate.hpp"

namespace Git
{

    namespace Internal
    {

        /**
         * @internal
         * @ingroup     GitWrap
         * @brief       The ReferencePrivate class
         *
         */
        class ReferencePrivate : public RefNamePrivate
        {
        public:
            ReferencePrivate(git_reference* ref, const RefNamePrivate* refName);
        public:
            ReferencePrivate(const RepositoryPrivate::Ptr& repo,
                             const QString& name,
                             git_reference* ref);
            ReferencePrivate(const RepositoryPrivate::Ptr& repo, git_reference* ref);
            ~ReferencePrivate();

        public:
            bool isRealReference() const;

        public:
            bool isValidObject(Result &r) const;

        public:
            bool            wasDeleted;
            git_reference*  reference;
        };

    }

}

#endif
