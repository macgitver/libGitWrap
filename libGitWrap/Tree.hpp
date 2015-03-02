/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2015 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
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

#ifndef GIT_OBJECT_TREE_H
#define GIT_OBJECT_TREE_H

#include "libGitWrap/Object.hpp"

// -- DEPRECATED INCLUDES BEGIN --8>
#include "libGitWrap/Diff.hpp"
#include "libGitWrap/DiffList.hpp"
// <8-- DEPRECATED INCLUDES END --

namespace Git
{

    class DiffTree;
    class TreeEntry;

    namespace Internal
    {
        class TreePrivate;
    }

    /**
     * @ingroup     GitWrap
     * @brief       Represents a git tree object
     *
     */
    class GITWRAP_API Tree : public Object
    {
        GW_PRIVATE_DECL(Tree, Object, public)
    public:
        enum { ObjectTypeId = otTree };

    public:
        Tree subPath(Result& result, const QString& pathName) const;

        size_t entryCount() const;
        TreeEntry entryAt( size_t index ) const;
        TreeEntry entry( const QString& fileName ) const;

    public:
        inline TreeEntry operator[]( size_t index ) const;
        inline TreeEntry operator[]( const QString& fileName ) const;

    public:
        // -- DEPRECATED METHODS BEGIN --8>
        /**
         * @brief           Deprecated: Tree::diffToTree
         * @deprecated      Use @ref Diff::treeToTree(Result& result, const Tree& oldTree, const Tree& newTree) instead.
         */
        GW_DEPRECATED
        inline DiffList diffToTree(Result& result, const Tree& newTree)
        {
            return Diff( result ).treeToTree( result, *this, newTree );
        }

        /**
         * @brief           Deprecated: Tree::diffToIndex
         * @deprecated      Use @ref Diff::treeToIndex(Result& result, const Tree& tree) instead.
         */
        GW_DEPRECATED
        inline DiffList diffToIndex(Result& result)
        {
            return Diff( result ).treeToIndex( result, *this );
        }

        /**
         * @brief           Deprecated: Tree::diffToWorkingDir
         * @deprecated      Use @ref Diff::treeToWorkDir(Result& result, const Tree& tree) instead.
         */
        GW_DEPRECATED
        inline DiffList diffToWorkingDir(Result& result)
        {
            return Diff( result ).treeToWorkDir( result, *this );
        }

        /**
         * @brief           Deprecated: Tree::diffToWorkingDirWithIndex
         * @deprecated      Use @ref Diff::treeToWorkDirWithIndex(Result& result, const Tree& tree) instead.
         */
        GW_DEPRECATED
        inline DiffList diffToWorkingDirWithIndex(Result& result)
        {
            return Diff( result ).treeToWorkDirWithIndex( result, *this );
        }

        // <8-- DEPRECATED METHODS END --
    };

    template<>
    inline Tree Object::as() const
    {
        return asTree();
    }

}

Q_DECLARE_METATYPE(Git::Tree)

#endif
