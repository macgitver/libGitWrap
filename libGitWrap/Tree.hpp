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

#ifndef GIT_OBJECT_TREE_H
#define GIT_OBJECT_TREE_H

#include "libGitWrap/GitWrap.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Object.hpp"
#include "libGitWrap/TreeEntry.hpp"

namespace Git
{

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
        GW_PRIVATE_DECL(Tree, Object, public);
    public:
        enum { ObjectTypeId = otTree };

    public:
        Tree();
        Tree(const Tree& o);

    public:
        Tree subPath( Result& result, const QString& pathName ) const;

        DiffList diffToTree( Result& result, Tree newTree );
        DiffList diffToIndex( Result& result );
        DiffList diffToWorkingDir( Result& result );

        size_t entryCount() const;
        TreeEntry entryAt( size_t index ) const;
        TreeEntry entry( const QString& fileName ) const;

        void checkout(Result& result,
                      bool force = false,
                      const QStringList &paths = QStringList()) const;

    public:
        inline TreeEntry operator[]( size_t index ) const
        {
            return entryAt( index );
        }

        inline TreeEntry operator[]( const QString& fileName ) const
        {
            return entry( fileName );
        }
    };

    template<>
    inline Tree Object::as() const
    {
        return asTree();
    }

}

Q_DECLARE_METATYPE(Git::Tree)

#endif
