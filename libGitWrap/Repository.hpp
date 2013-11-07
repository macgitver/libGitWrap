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

namespace Git
{

    namespace Internal
    {
        class RepositoryPrivate;
    }

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
        static Repository create( const QString& path,
                                  bool bare,
                                  Result& result );

        static QString discover( const QString& startPath,
                                 bool acrossFs /* = false */,
                                 const QStringList& ceilingDirs /* = QStringList() */,
                                 Result& result );

        static Repository open( const QString& path,
                                Result& result );

        Repository reopen(Result& result) const;

        bool isBare() const;
        bool isHeadDetached() const;

        bool detachHead(Result& result);
        void setDetachedHEAD(Result& result, const ObjectId& sha);
        void setDetachedHEAD(Result& result, const Commit& commit);

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

        // ### move to BranchRef
        bool renameBranch( const QString& oldName, const QString& newName, bool force /* = false */,
                           Result& result );

        Repository superproject() const;
        Submodule superprojectSubmodule() const;

        Index index( Result& result );

        Git::StatusFlags status(Result &result, const QString &fileName) const;
        Git::StatusHash status(Result &result) const;

        Reference HEAD( Result& result ) const;

        GW_DEPRECATED Reference lookupRef(Result& result, const QString& refName , bool dwim = false);
        ObjectId resolveRef(Result& result, const QString& refName);

        Reference reference(Result& result, const QString& refName, bool dwim = false);
        BranchRef branchRef(Result& result, const QString& branchName);
        TagRef tagRef(Result& result, const QString& tagName);
        NoteRef noteRef(Result& result, const QString& noteName);

        // ### Deprecate and name these: commit, tree, blob and tag.
        // ### Figure out what the QString-overload should be useful for!
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

        // ### Figure out, what this overload could be useful for!
        template< class T >
        T lookup(Result& result, const QString& refName);

        bool shouldIgnore( Result& result, const QString& filePath ) const;

        GW_DEPRECATED
        RevisionWalker newWalker(Result& result);

        QStringList allRemoteNames( Result& result ) const;
        Remote::List allRemotes(Result& result) const;
        Remote remote( Result& result, const QString& remoteName ) const;

        GW_DEPRECATED
        Remote createRemote(Result& result, const QString& remoteName, const QString& url,
                            const QString& fetchSpec);

        DiffList diffCommitToCommit(Result& result, Commit oldCommit, Commit newCommit);

        DiffList diffTreeToTree( Result& result, Tree oldTree,
                                 Tree newTree);

        DiffList diffIndexToTree( Result& result, Tree oldTree );

        DiffList diffTreeToWorkingDir( Result& result, Tree oldTree );
        DiffList diffIndexToWorkingDir( Result& result );

        SubmoduleList submodules(Result& result);
        QStringList submoduleNames(Result& result) const;
        Submodule submodule(Result& result, const QString& name) const;
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
