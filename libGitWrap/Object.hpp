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

#include "libGitWrap/RepoObject.hpp"

#include "libGitWrap/ObjectId.hpp"


namespace Git
{

    namespace Internal
    {
        class ObjectPrivate;
    }

    /**
     * @brief The Object class is the base class for Git repository objects.
     *
     * @ingroup GitWrap
     * @{
     */
    class GITWRAP_API Object : public RepoObject
    {
        GW_PRIVATE_DECL(Object, RepoObject, public)

    public:

        /**
         * @return the object's type
         */
        ObjectType type( Result& result ) const;

        /**
         * @return the object's id (OID)
         */
        ObjectId id() const;

        /**
         * @brief Converts a generic object into a Git tree object.
         *
         * @return the valid or invalid converted Tree
         *
         * @see isValid()
         */
        Tree asTree() const;

        /**
         * @brief Converts a generic object into a Git commit object.
         *
         * @return the valid or invalid converted Commit
         *
         * @see isValid()
         */
        Commit asCommit() const;

        /**
         * @brief Converts a generic object into a Git BLOB object.
         *
         * @return the valid or invalid converted Blob
         *
         * @see isValid()
         */
        Blob asBlob() const;

        /**
         * @brief Converts a generic object into a Git tag object.
         *
         * @return the valid or invalid converted Tag
         *
         * @see isValid()
         */
        Tag asTag() const;

        // This method has no general implementation. There are four implementations that are all
        // located in the reimplementation headers of this class (i.e. Tag.hpp)
        template< class T >
        T as() const;

        /**
         * @brief Checks, if this is a Tree object.
         * @return true or false
         */
        bool isTree() const;

        /**
         * @brief Checks, if this is a Tag object.
         * @return true or false
         */
        bool isTag() const;

        /**
         * @brief Checks, if this is a Commit object.
         * @return true or false
         */
        bool isCommit() const;

        /**
         * @brief Checks, if this is a Blob object.
         * @return true or false
         */
        bool isBlob() const;
    };

}

Q_DECLARE_METATYPE( Git::Object )
