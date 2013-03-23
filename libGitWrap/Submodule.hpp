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

#ifndef GIT_SUBMODULE_H
#define GIT_SUBMODULE_H

#include "GitWrap.hpp"

namespace Git
{

    namespace Internal
    {
        class RepositoryPrivate;
        class SubmodulePrivate;
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

        /**
         * @brief The Status enum describes a submodule's status.
         */
        enum Status
        {
            InHead          = (1u << 0),
            InIndex         = (1u << 1),
            InConfig        = (1u << 2),
            InWorkingTree   = (1u << 3),
            IndexAdded      = (1u << 4),
            IndexDeleted    = (1u << 5),
            IndexModified   = (1u << 6),
            WorkingTreeUninitialized    = (1u << 7),
            WorkingTreeAdded            = (1u << 8),
            WorkingTreeDeleted          = (1u << 9),
            WorkingTreeModified         = (1u << 10),
            WorkingTreeIndexModified    = (1u << 11),
            WorkingTreeWtModified       = (1u << 12),
            WorkingTreeUntracked        = (1u << 13)
        };
        Q_DECLARE_FLAGS ( StatusFlags, Status )

    public:
        /**
         * @brief Submodule
         * @param repo the owner repository
         * @param name is used to lookup the submodule in the owner repository
         */
        Submodule( const Internal::GitPtr< Internal::RepositoryPrivate >& repo,
                   const QString& name );
        Submodule( const Submodule& other );
        Submodule();
        ~Submodule();

        Submodule& operator=( const Submodule& other );

    public:
        bool isValid();

    public:
        QString name() const;
        QString path( Result& r ) const;
        QString url( Result& r ) const;

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
         * @brief Closes the submodule's repository.
         */
        void close();

        bool isOpened() const;

        Submodule::Status status(Result &result) const;

    private:
        Internal::GitPtr< Internal::SubmodulePrivate > d;
    };

    typedef QList< Submodule > SubmoduleList;

}

Q_DECLARE_METATYPE( Git::Submodule )

#endif
