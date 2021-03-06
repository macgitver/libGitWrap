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

#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Object.hpp"
#include "libGitWrap/Operations/Providers.hpp"
#include "libGitWrap/Result.hpp"
#include "libGitWrap/Signature.hpp"
#include "libGitWrap/DiffList.hpp"

namespace Git
{

    class DiffTree;

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
        GW_PRIVATE_OBJECT_DECL(Commit, Object, public)

    public:
        static Commit create(Result& result, Repository& repo, const Tree& tree,
                             const QString& message,
                             const Signature& author, const Signature& committer,
                             const CommitList& parents );
        static Commit create(Result &result, Repository &repo, const Tree &tree,
                             const QString &message,
                             const Signature &author, const Signature &committer,
                             const ObjectIdList &parents);

    public:
        operator TreeProviderPtr() const;
        operator ParentProviderPtr() const;

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

        Signature author() const;
        Signature committer() const;

        QString message() const;
        QString shortMessage() const;

        DiffList diffFromAllParents(Result& result) const;
        DiffList diffToAllParents(Result& result) const;

        DiffList diffFromParent(Result& result, unsigned int index) const;
        DiffList diffToParent(Result& result, unsigned int index) const;

        DiffList diffFrom(Result& result, const Commit& oldCommit) const;
        DiffList diffTo(Result& result, const Commit& oldCommit) const;

    public:
        // -- DEPRECATED FUNCTIONS BEGIN --8>

        /**
         * @brief       Deprecated: Commit::author
         * @deprecated  Use @ref Commit::author() instead.
         */
        GW_DEPRECATED inline Signature author( Result& result ) const
        {
            return author();
        }

        /**
         * @brief       Deprecated: Commit::committer
         * @deprecated  Use @ref Commit::committer() instead.
         */
        GW_DEPRECATED inline Signature committer( Result& result ) const
        {
            return committer();
        }

        /**
         * @brief       Deprecated: Commit::message
         * @deprecated  Use @ref Commit::message() instead.
         */
        GW_DEPRECATED inline QString message( Result& result ) const
        {
            return message();
        }

        /**
         * @brief       Deprecated: Commit::shortMessage
         * @deprecated  Use @ref Commit::shortMessage() instead.
         */
        GW_DEPRECATED inline QString shortMessage( Result& result ) const
        {
            return shortMessage();
        }

        // <8-- DEPRECATED FUNCTIONS END --
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


    class GITWRAP_API CommitParentProvider : public ParentProvider
    {
    public:
        CommitParentProvider(const Commit& commit );

    public:
        Repository repository() const;
        ObjectIdList parents(Result& result) const;

    private:
        const Commit&   mCommit;
    };

    class GITWRAP_API CommitTreeProvider :public TreeProvider
    {
    public:
        CommitTreeProvider( const Commit& commit );

    private:
        const Commit&   mCommit;

    public:
        Repository repository() const;
        Tree tree(Result& result);
    };
}

GITWRAP_API QDebug operator<<( QDebug debug, const Git::Commit& commit );

Q_DECLARE_METATYPE(Git::Commit)
