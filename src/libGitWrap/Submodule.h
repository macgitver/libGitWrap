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
        ObjectId headOid() const;
        ObjectId wdOid() const;

    public:
        Git::Repository repository() const;

        /**
         * @brief Opens a submodule's repository.
         * @return true, when repoitory could be opened successfully; false otherwise
         */
        bool open(Result &result);

    private:
        Internal::GitPtr< Internal::RepositoryPrivate > mOwnerRepo;
        Internal::GitPtr< Internal::RepositoryPrivate > mMyRepo;
        QString mName;
    };

}

Q_DECLARE_METATYPE( Git::Submodule )

#endif
