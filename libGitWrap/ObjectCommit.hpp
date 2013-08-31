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

#ifndef GIT_OBJECT_COMMIT_H
#define GIT_OBJECT_COMMIT_H

#include "GitWrap.hpp"
#include "ObjectId.hpp"
#include "Object.hpp"
#include "Signature.hpp"
#include "DiffList.hpp"
#include "Result.hpp"

namespace Git
{

    /**
     * @ingroup     GitWrap
     * @brief       Represents a git commit object.
     *
     */
    class GITWRAP_API ObjectCommit : public Object
    {
    public:
        ObjectCommit();
        ObjectCommit( Internal::ObjectPrivate* _d );
        ObjectCommit( const ObjectCommit& o );

    public:
        bool operator==( const Git::ObjectCommit& commit ) const
        {
            Result r;
            return isEqual( r, commit ) && r;
        }

        bool operator!=( const Git::ObjectCommit& commit ) const
        {
            Result r;
            return !isEqual( r, commit ) && r;
        }

    public:
        ObjectTree tree( Result& result );
        ObjectId treeId( Result& result );

        ObjectIdList parentCommitIds( Result& result ) const;
        ObjectCommitList parentCommits( Result& result ) const;
        ObjectCommit parentCommit( Result& result, unsigned int index ) const;
        ObjectId parentCommitId( Result& result, unsigned int index ) const;
        unsigned int numParentCommits() const;

        bool isParentOf( Result& result, const Git::ObjectCommit& child ) const;
        bool isChildOf( Result& result, const Git::ObjectCommit& parent ) const;
        bool isEqual( Result& result, const Git::ObjectCommit& commit ) const;

        Signature author( Result& result ) const;
        Signature committer( Result& result ) const;

        QString message( Result& result ) const;
        QString shortMessage( Result& result ) const;

        void checkout( Result &result,
                       bool force = false,
                       bool updateHEAD = true,
                       const QStringList &paths = QStringList() ) const;
        Reference createBranch( Result& result, const QString& name, bool force ) const;

        DiffList diffFromParent( Result& result, unsigned int index );
        DiffList diffFromAllParents( Result& result );

        void updateHEAD(Result &result) const;
    };

    /**
     * @ingroup     GitWrap
     * @brief       qHash() for Git::ObjectCommit
     */
    inline uint qHash( const ObjectCommit& c )
    {
        Result r;
        return qHash( c.id( r ) );
    }

}

GITWRAP_API QDebug operator<<( QDebug debug, const Git::ObjectCommit& commit );

#endif
