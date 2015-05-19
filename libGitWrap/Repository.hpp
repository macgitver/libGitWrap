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

#include "libGitWrap/Base.hpp"
#include "libGitWrap/Commit.hpp"
#include "libGitWrap/Diff.hpp"
#include "libGitWrap/DiffList.hpp"
#include "libGitWrap/Object.hpp"
#include "libGitWrap/Reference.hpp"
#include "libGitWrap/Remote.hpp"
#include "libGitWrap/RevisionWalker.hpp"
#include "libGitWrap/Tree.hpp"

namespace Git
{

    namespace Internal
    {
        class RepositoryPrivate;
    }

    class CommitOperation;

    typedef QHash< QString, ObjectId > ResolvedRefs;

    /**
     * @ingroup     GitWrap
     * @brief       A git repository
     *
     */
    class GITWRAP_API Repository : public Base
    {
        GW_PRIVATE_DECL(Repository, Base, public)

    public:
        static Repository create(Result& result,
                                 const QString& path,
                                 bool bare);

        static QString discover(Result& result , const QString& startPath,
                                bool acrossFs = false,
                                const QStringList& ceilingDirs = QStringList());

        static Repository open(Result& result,
                               const QString& path );

        Repository reopen(Result& result) const;

        bool isBare() const;
        bool isHeadDetached() const;
        bool isHeadUnborn() const;

        bool detachHead(Result& result);
        void setDetachedHEAD(Result& result, const ObjectId& sha);

        void setHEAD(Result& result, const BranchRef& branch);
        void setHEAD(Result& result, const QString& branchName);

        QString workTreePath() const;
        QString path() const;

        QString name() const;

        QStringList allReferenceNames( Result& result );
        ReferenceList allReferences( Result& result );

        QStringList allBranchNames( Result& result );
        QStringList branchNames( Result& result, bool local, bool remote );
        QString currentBranch( Result& result );

        QStringList allTagNames( Result& result );

        ResolvedRefs allResolvedRefs( Result& result );

        // TODO: move to BranchRef
        bool renameBranch(Result& result, const QString& oldName, const QString& newName, bool force = false);

        Repository superproject() const;
        Submodule superprojectSubmodule() const;

        Index index( Result& result );

        Git::StatusFlags status(Result &result, const QString &fileName) const;
        Git::StatusHash status(Result &result) const;

        Reference HEAD( Result& result ) const;
        BranchRef headBranch( Result& result ) const;
        QString headBranchName(Result& result) const;

        ObjectId resolveRef(Result& result, const QString& refName);

        Reference reference(Result& result, const QString& refName, bool dwim = false);
        BranchRef branchRef(Result& result, const QString& branchName);
        TagRef tagRef(Result& result, const QString& tagName);
        NoteRef noteRef(Result& result, const QString& noteName);

        // TODO: Deprecate and name these: commit, tree, blob and tag.
        // TODO: Figure out what the QString-overload should be useful for!
        Commit lookupCommit( Result& result, const ObjectId& id );
        Commit lookupCommit( Result& result, const QString& refName );

        Tree lookupTree( Result& result, const ObjectId& id );
        Tree lookupTree( Result& result, const QString& refName );

        Blob lookupBlob( Result& result, const ObjectId& id );
        Blob lookupBlob( Result& result, const QString& refName );

        Tag lookupTag( Result& result, const ObjectId& id );
        Tag lookupTag( Result& result, const QString& refName );

        Object lookup(Result& result, const ObjectId& id, ObjectType ot);
        Object lookup(Result& result, const QString& refName, ObjectType ot);

        template< class T >
        T lookup(Result& result, const ObjectId& id);

        // TODO: Figure out, what this overload could be useful for!
        template< class T >
        T lookup(Result& result, const QString& refName);

        bool shouldIgnore( Result& result, const QString& filePath ) const;

        QStringList allRemoteNames( Result& result ) const;
        Remote::List allRemotes(Result& result) const;
        Remote remote( Result& result, const QString& remoteName ) const;

        SubmoduleList submodules(Result& result);
        QStringList submoduleNames(Result& result) const;
        Submodule submodule(Result& result, const QString& name) const;

        void calculateDivergence(Result& result,
                                 const ObjectId& idLocal, const ObjectId& idRemote,
                                 size_t& ahead, size_t& behind) const;

    public:
        CommitOperation* commitOperation(Result& result, const QString& msg);

    public:
        /**
         * @brief       Deprecated: Repository::gitPath
         * @deprecated  Use @ref Repository::path() instead
         */
        GW_DEPRECATED
        inline QString gitPath() const
        {
            return path();
        }

        /**
         * @brief       Deprecated: Repository::basePath
         * @deprecated  Use @ref Repository::workTreePath() instead
         */
        GW_DEPRECATED
        inline QString basePath() const
        {
            return workTreePath();
        }

        /**
         * @brief       Deprecated: Repository::diffIndexToWorkingDir
         * @deprecated  Use @ref Diff::indexToWorkDir(Result& result, const Repository& repo) instead.
         */
        GW_DEPRECATED
        inline DiffList diffIndexToWorkingDir( Result& result )
        {
            return Diff( result ).indexToWorkDir( result, *this );
        }

        /**
         * @brief           Deprecated: Repository::diffCommitToCommit
         * @deprecated      Use @ref Commit::diffFrom(Result& result, const Commit& oldCommit) instead.
         */
        GW_DEPRECATED
        inline DiffList diffCommitToCommit( Result& result, Commit oldCommit, Commit newCommit )
        {
            return newCommit.diffFrom( result, newCommit );
        }

        /**
         * @brief           Deprecated: Repository::diffTreeToTree
         * @deprecated      Use @ref Diff::diffTreeToTree(Result& result, const Tree& oldTree, const Tree& newTree) instead.
         */
        GW_DEPRECATED
        inline DiffList diffTreeToTree(Result& result, Tree oldTree, Tree newTree)
        {
            return Diff(result).treeToTree( result, oldTree, newTree );
        }

        /**
         * @brief           Deprecated: Repository::diffTreeToWorkingDir
         * @deprecated      Use @ref Diff::treeToWorkDir(Result& result, const Tree& tree) instead.
         */
        GW_DEPRECATED
        inline DiffList diffTreeToWorkingDir(Result& result, Tree oldTree)
        {
            return Diff( result ).treeToWorkDir( result, oldTree );
        }

        /**
         * @brief           Deprecated: Repository::diffIndexToTree
         * @deprecated      Use @ref Diff::diffTreeToIndex(Result& result, const Tree& tree) instead.
         */
        GW_DEPRECATED
        inline DiffList diffIndexToTree(Result& result, Tree oldTree)
        {
            return Diff( result ).treeToIndex( result, oldTree );
        }
    };

    template< class T >
    inline T Repository::lookup(Result& result, const ObjectId& id)
    {
        return lookup(result, id, ObjectType(T::ObjectTypeId)).as<T>();
    }

    template< class T >
    inline T Repository::lookup(Result& result, const QString& refName)
    {
        return lookup(result, refName, ObjectType(T::ObjectTypeId)).as<T>();
    }

    template<>
    inline Object Repository::lookup(Result& result, const ObjectId& id)
    {
        return lookup(result, id, otAny);
    }

    template<>
    inline Object Repository::lookup(Result& result, const QString& refName)
    {
        return lookup(result, refName, otAny);
    }

}

Q_DECLARE_METATYPE( Git::Repository )
