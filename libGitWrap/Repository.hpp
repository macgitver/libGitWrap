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
#include "libGitWrap/Submodule.hpp"
#include "libGitWrap/Remote.hpp"

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
    public:
        typedef Internal::RepositoryPrivate Private;

    public:
        explicit Repository(Internal::RepositoryPrivate& _d);
        Repository();
        Repository( const Repository& other );
        Repository& operator=( const Repository& other );

    public:
        ~Repository();

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
        void setHEAD(Result& result, const QString& branchName);
        void setHEAD(Result& result, const Commit& commit);

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

        bool renameBranch( const QString& oldName, const QString& newName, bool force /* = false */,
                           Result& result );

        Index index( Result& result );

        Git::StatusFlags status(Result &result, const QString &fileName) const;
        Git::StatusHash status(Result &result) const;

        Reference HEAD( Result& result ) const;
        Reference lookupRef(Result& result, const QString& refName , bool dwim = false);
        ObjectId resolveRef( Result& result, const QString& refName );

        Object lookup(Result& result, const ObjectId& id, ObjectType ot /* = otAny */);

        Commit lookupCommit( Result& result, const ObjectId& id );
        Tree lookupTree( Result& result, const ObjectId& id );
        Blob lookupBlob( Result& result, const ObjectId& id );
        Tag lookupTag( Result& result, const ObjectId& id );

        Object lookup(Result& result, const QString& refName, ObjectType ot /* = otAny */);

        Commit lookupCommit( Result& result, const QString& refName );
        Tree lookupTree( Result& result, const QString& refName );
        Blob lookupBlob( Result& result, const QString& refName );
        Tag lookupTag( Result& result, const QString& refName );

        bool shouldIgnore( Result& result, const QString& filePath ) const;

        RevisionWalker newWalker( Result& result );

        QStringList allRemoteNames( Result& result ) const;
        Remote::List allRemotes(Result& result) const;
        Remote remote( Result& result, const QString& remoteName ) const;
        Remote createRemote( Result& result, const QString& remoteName, const QString& url,
                             const QString& fetchSpec );

        DiffList diffCommitToCommit(Result& result, Commit oldCommit, Commit newCommit);

        DiffList diffTreeToTree( Result& result, Tree oldTree,
                                 Tree newTree);

        DiffList diffIndexToTree( Result& result, Tree oldTree );

        DiffList diffTreeToWorkingDir( Result& result, Tree oldTree );
        DiffList diffIndexToWorkingDir( Result& result );

        SubmoduleList submodules( Result& result );
        Submodule submodule( Result& result, const QString& name );
    };

}

Q_DECLARE_METATYPE( Git::Repository )

#endif
