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

#include <QDebug>
#include <QList>

#include "GitWrap.hpp"
#include "ObjectId.hpp"
#include "Object.hpp"
#include "Signature.hpp"
#include "DiffList.hpp"
#include "Result.hpp"

namespace Git
{

    class Reference;
    class ObjectTree;

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
        QList< ObjectCommit > parentCommits( Result& result ) const;
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

        Reference createBranch( const QString& name, bool force,
                                Result& result ) const;

        DiffList diffFromParent( Result& result, unsigned int index );
        DiffList diffFromAllParents( Result& result );
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

// Should we keep this? If yes, we should provide them for all GitWrap-classes.
/**
 * @ingroup     GitWrap
 * @brief       Debug-Stream support of Git::ObjectCommit
 * @param[in]   debug   The Debug-Stream to output into
 * @param[in]   commit  The commit object to output
 * @return      The Debug-Stream
 */
inline QDebug operator<<( QDebug debug, const Git::ObjectCommit& commit )
{
    Git::Result r;
    return debug << "Commit(id=" << commit.id( r ) << ";author=" << commit.author( r ) << ")";
}

#endif
