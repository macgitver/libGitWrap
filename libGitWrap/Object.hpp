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

#include "GitWrap.hpp"
#include "ObjectId.hpp"

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
    class GITWRAP_API Object
    {
    public:
        Object( Internal::ObjectPrivate* _d );
        Object( const Object& other );
        Object();
        ~Object();

    public:
        Object& operator=( const Object& other );
        bool operator==( const Object& other ) const;

        /**
         * @brief Checks, if this is a valid Git object.
         *
         * A Git object is valid, when it has a valid pointer.
         *
         * @return true or false
         */
        bool isValid() const;

        /**
         * @return the object's type
         */
        ObjectType type( Result& result ) const;

        /**
         * @return the object's id (OID)
         */
        ObjectId id( Result& result ) const;

        /**
         * @brief Converts a generic object into a Git tree object.
         *
         * @return the valid or invalid converted ObjectTree
         *
         * @see isValid()
         */
        ObjectTree asTree( Result& result );

        /**
         * @brief Converts a generic object into a Git commit object.
         *
         * @return the valid or invalid converted ObjectCommit
         *
         * @see isValid()
         */
        ObjectCommit asCommit( Result& result );

        /**
         * @brief Converts a generic object into a Git BLOB object.
         *
         * @return the valid or invalid converted ObjectBlob
         *
         * @see isValid()
         */
        ObjectBlob asBlob( Result& result );

        /**
         * @brief Converts a generic object into a Git tag object.
         *
         * @return the valid or invalid converted ObjectTag
         *
         * @see isValid()
         */
        ObjectTag asTag( Result& result );

        /**
         * @brief Checks, if this is a ObjectTree object.
         * @return true or false
         */
        bool isTree( Result& result ) const;

        /**
         * @brief Checks, if this is a ObjectTree object.
         * @return true or false
         */
        bool isTag( Result& result ) const;

        /**
         * @brief Checks, if this is a ObjectTree object.
         * @return true or false
         */
        bool isCommit( Result& result ) const;

        /**
         * @brief Checks, if this is a ObjectTree object.
         * @return true or false
         */
        bool isBlob( Result& result ) const;

        /**
         * @return the owner repository or an invalid repository
         */
        Repository repository( Result& result ) const;

    protected:
        Internal::GitPtr< Internal::ObjectPrivate > d;
    };

}

Q_DECLARE_METATYPE( Git::Object )

#endif
