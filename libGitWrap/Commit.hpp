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

#include "libGitWrap/GitWrap.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Object.hpp"
#include "libGitWrap/Signature.hpp"
#include "libGitWrap/DiffList.hpp"
#include "libGitWrap/Result.hpp"

namespace Git
{

    namespace Internal
    {
        class CommitPrivate;
    }

    /**
     * @ingroup     GitWrap
     * @brief       Represents a git commit object.
     *
     */
    class GITWRAP_API Commit : public Object
    {
        GW_PRIVATE_DECL(Commit, Object, public);
    public:
        enum { ObjectTypeId = otCommit };

    public:
        Tree tree( Result& result ) const;
        ObjectId treeId( Result& result ) const;

        ObjectIdList parentCommitIds( Result& result ) const;
        CommitList parentCommits( Result& result ) const;
        Commit parentCommit( Result& result, unsigned int index ) const;
        ObjectId parentCommitId( Result& result, unsigned int index ) const;
        unsigned int numParentCommits() const;

        bool isParentOf( Result& result, const Git::Commit& child ) const;
        bool isChildOf( Result& result, const Git::Commit& parent ) const;
        bool isEqual( Result& result, const Git::Commit& commit ) const;

        GW_DEPRECATED Signature author( Result& result ) const;
        GW_DEPRECATED Signature committer( Result& result ) const;
        GW_DEPRECATED QString message( Result& result ) const;
        GW_DEPRECATED QString shortMessage( Result& result ) const;

        Signature author() const;
        Signature committer() const;

        QString message() const;
        QString shortMessage() const;

        void checkout( Result &result,
                       bool force = false,
                       bool updateHEAD = true,
                       const QStringList &paths = QStringList() ) const;
        Reference createBranch( Result& result, const QString& name, bool force ) const;

        DiffList diffFromParent( Result& result, unsigned int index );
        DiffList diffFromAllParents( Result& result );

        void setAsDetachedHEAD(Result& result) const;
    };

    template<>
    inline Commit Object::as() const
    {
        return asCommit();
    }

    /**
     * @ingroup     GitWrap
     * @brief       qHash() for Git::Commit
     */
    inline uint qHash(const Commit& c)
    {
        return qHash(c.id());
    }

}

GITWRAP_API QDebug operator<<( QDebug debug, const Git::Commit& commit );

Q_DECLARE_METATYPE(Git::Commit)

#endif
