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

#ifndef GIT_REPOSITORY_H
#define GIT_REPOSITORY_H

#include "libGitWrap/Base.hpp"
#include "libGitWrap/Remote.hpp"
#include "libGitWrap/Object.hpp"


// -- DEPRECATED INCLUDES BEGIN --8>

#include "libGitWrap/RevisionWalker.hpp"
#include "libGitWrap/Reference.hpp"

// <8-- DEPRECATED INCLUDES END --


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

        bool detachHead(Result& result);
        void setDetachedHEAD(Result& result, const ObjectId& sha);

        void setHEAD(Result& result, const BranchRef& branch);
        void setHEAD(Result& result, const QString& branchName);

        QString basePath() const;
        QString gitPath() const;

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

        DiffList diffCommitToCommit(Result& result, Commit oldCommit, Commit newCommit);

        DiffList diffTreeToTree( Result& result, Tree oldTree,
                                 Tree newTree);

        DiffList diffIndexToTree( Result& result, Tree oldTree );

        DiffList diffTreeToWorkingDir( Result& result, Tree oldTree );
        DiffList diffIndexToWorkingDir( Result& result );

        SubmoduleList submodules(Result& result);
        QStringList submoduleNames(Result& result) const;
        Submodule submodule(Result& result, const QString& name) const;

        void calculateDivergence(Result& result,
                                 const ObjectId& idLocal, const ObjectId& idRemote,
                                 size_t& ahead, size_t& behind) const;

    public:
        CommitOperation* commitOperation(Result& result, const QString& msg);

    public:
        // -- DEPRECATED FUNCTIONS BEGIN --8>

        /**
         * @brief           Deprecated: Repository::create
         * @deprecated      Use @ref Repository::create(Result& result, const QString& path, bool bare) instead.
         */
        GW_DEPRECATED
        static Repository create(const QString& path, bool bare, Result& result)
        {
            return create( result, path, bare );
        }

        /**
         * @brief           Deprecated: Repository::discover
         * @deprecated      Use @ref Repository::create(Result& result, const QString& startPath, bool acrossFs, const QStringList& ceilingDirs) instead.
         */
        GW_DEPRECATED
        static QString discover(const QString& startPath, bool acrossFs, const QStringList& ceilingDirs, Git::Result& result)
        {
            return discover(result, startPath, acrossFs, ceilingDirs);
        }

        /**
         * @brief           Deprecated: Repository::open
         * @deprecated      Use @ref Repository::open(Result& result, const QString& startPath, bool acrossFs, const QStringList& ceilingDirs) instead.
         */
        GW_DEPRECATED
        static Repository open(const QString &path, Result &result)
        {
            return open( result, path );
        }

        /**
         * @brief           Deprecated: Repository::newWalker
         * @deprecated      Use @ref RevisionWalker::create() instead.
         */
        GW_DEPRECATED
        inline RevisionWalker newWalker( Result& result )
        {
            return RevisionWalker::create(result, *this);
        }

        /**
         * @brief           Deprecated: Repository::createRemote
         * @deprecated      Use @ref Remote::create() instead.
         */
        GW_DEPRECATED
        inline Remote createRemote(Result& result, const QString& remoteName, const QString& url,
                                   const QString& fetchSpec)
        {
            return Remote::create(result, *this, remoteName, url, fetchSpec);
        }

        /**
         * @brief           Deprecated: Repository::renameBranch
         * @deprecated      Use @ref Repository::renameBranch(Result& result, const QString& oldName, const QString& newName, bool force) instead.
         */
        GW_DEPRECATED
        inline bool renameBranch(const QString& oldName, const QString& newName, bool force, Git::Result& result)
        {
            return renameBranch( result, oldName, newName, force );
        }

        /**
         * @brief           Deprecated: Repository::lookupRef
         * @deprecated      Use @ref Repository::reference(Result& result, const QString& refName , bool dwim = false) instead.
         */
        GW_DEPRECATED
        inline Reference lookupRef(Result& result, const QString& refName , bool dwim = false)
        {
            return reference( result, refName, dwim );
        }

        // -- DEPRECATED FUNCTIONS END --<8
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

#endif
