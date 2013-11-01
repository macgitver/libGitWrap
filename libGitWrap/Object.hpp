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

#ifndef GIT_OBJECT_H
#define GIT_OBJECT_H

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
    public:
        typedef Internal::ObjectPrivate Private;

    public:
        Object(Internal::ObjectPrivate& _d);
        Object(const Object& other);
        Object();
        ~Object();

    public:
        Object& operator=( const Object& other );
        bool operator==( const Object& other ) const;

        /**
         * @return the object's type
         */
        ObjectType type( Result& result ) const;

        /**
         * @return the object's id (OID)
         */
        ObjectId id() const;

        GW_DEPRECATED ObjectId id( Result& result ) const;

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

        GW_DEPRECATED bool isTree   (Result& result) const { return isTree();   }
        GW_DEPRECATED bool isTag    (Result& result) const { return isTag();    }
        GW_DEPRECATED bool isCommit (Result& result) const { return isCommit(); }
        GW_DEPRECATED bool isBlob   (Result& result) const { return isBlob();   }

        GW_DEPRECATED Tree   asTree  (Result& result) const;
        GW_DEPRECATED Commit asCommit(Result& result) const;
        GW_DEPRECATED Blob   asBlob  (Result& result) const;
        GW_DEPRECATED Tag    asTag   (Result& result) const;

        template< class T >
        GW_DEPRECATED T as(Result& result) const { return as<T>(); }

    };

}

Q_DECLARE_METATYPE( Git::Object )

#endif
