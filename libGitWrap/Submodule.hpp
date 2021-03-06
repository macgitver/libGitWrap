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

#include "libGitWrap/Base.hpp"
#include "libGitWrap/Repository.hpp"

namespace Git
{

    namespace Internal
    {
        class RepositoryPrivate;
        class SubmodulePrivate;
    }

    /**
     * @ingroup     GitWrap
     * @brief       Represents a git submodule
     *
     */
    class GITWRAP_API Submodule : public RepoObject
    {
        GW_PRIVATE_DECL(Submodule, RepoObject, public)
    public:
        enum IgnoreStrategy
        {
            None,
            Dirty,
            Untracked,
            All
        };

        enum UpdateStrategy
        {
            Reset       = -1,
            Default     = 0,
            Checkout    = 1,
            Rebase      = 2,
            Merge       = 3,
            Ignore      = 4
        };

    public:
        /**
         * @brief       A list of submodules
         */
        typedef SubmoduleList List;

    public:
        QString name() const;
        QString path( Result& r ) const;
        QString url( Result& r ) const;

        bool fetchRecursive() const;
        IgnoreStrategy ignoreStrategy() const;
        UpdateStrategy updateStrategy() const;

        /**
         * @brief The returned ObjectId points from the owner reposiory's 'HEAD' commit to a commit in the submodule.
         *
         * An empty ObjectId is returned, when the submodule is not known in the owner repository's 'HEAD' commit.
         *
         * @return the oid in the owner repository's 'HEAD' commit or an empty ObjectId
         */
        ObjectId headOid() const;

        /**
         * @brief Returns the ObjectId, the owner repository's index points to.
         *
         * This ObjectId is empty, when the submodule is committed and unchanged.
         *
         * @return the oid in the owner repository's index or an empty ObjectId
         */
        ObjectId indexOid() const;

        /**
         * @brief Returns the ObjectId of the submodule's 'HEAD' commit.
         *
         * When the submodule is committed and unchanged, this will be the same as @see headOid().
         *
         * @return the submodule's 'HEAD' oid or an empty ObjectId
         */
        ObjectId wdOid() const;

    public:
        Repository subRepository(Result& result);
        StatusFlags status(Result &result) const;
    };

}

Q_DECLARE_METATYPE(Git::Submodule)
