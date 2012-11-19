/*
 * MacGitver
 * Copyright (C) 2012 Sascha Cunz <sascha@babbelbox.org>
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

#ifndef GIT_SUBMODULE_H
#define GIT_SUBMODULE_H

#include "GitWrap.h"

namespace Git
{

    namespace Internal
    {
        class RepositoryPrivate;
    }

    class ObjectId;
    class Repository;

    /**
     * @ingroup     GitWrap
     * @brief       Represents a git submodule
     *
     */
    class GITWRAP_API Submodule
    {
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
            Checkout,
            Rebase,
            Merge,
            Ignore
        };

    public:
        /**
         * @brief Submodule
         * @param repo the owner repository
         * @param name is used to lookup the submodule in the owner repository
         */
        Submodule( Internal::RepositoryPrivate* repo, const QString& name );
        Submodule( const Submodule& other );
        Submodule();
        ~Submodule();

        Submodule& operator=( const Submodule& other );

    public:
        bool isValid();

    public:
        QString name() const;
        QString path() const;
        QString url() const;
        bool fetchRecursive() const;
        IgnoreStrategy ignoreStrategy() const;
        UpdateStrategy updateStrategy() const;

        /**
         * @brief The returned ObjectId points from the owner reposiory's 'HEAD' commit to a commit in the submodule.
         * An empty ObjectId is returned, when the submodule is not known in the owner repository's 'HEAD' commit.
         *
         * @return the oid in the owner repository's 'HEAD' commit or an empty ObjectId
         */
        ObjectId headOid() const;

        /**
         * @brief Returns the ObjectId, the owner repository's index points to.
         * This ObjectId is empty, when the submodule is committed and unchanged.
         *
         * @return the oid in the owner repository's index or an empty ObjectId
         */
        ObjectId indexOid() const;

        /**
         * @brief Returns the ObjectId of the submodule's 'HEAD' commit.
         * When the submodule is committed and unchanged, this will be the same as @see headOid().
         *
         * @return the submodule's 'HEAD' oid or an empty ObjectId
         */
        ObjectId wdOid() const;

    public:
        Git::Repository repository() const;

        /**
         * @brief Opens a submodule's repository.
         * @return true, when repoitory could be opened successfully; false otherwise
         */
        bool open(Result &result);

        /**
         * @brief Dereferences the submodule's repository @see GitPtr.
         */
        void close();

    private:
        Internal::GitPtr< Internal::RepositoryPrivate > mOwnerRepo;
        Internal::GitPtr< Internal::RepositoryPrivate > mMyRepo;
        QString mName;
    };

}

Q_DECLARE_METATYPE( Git::Submodule )

#endif
