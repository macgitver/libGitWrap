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

#include <QStringList>

#include "Submodule.hpp"

namespace Git
{

    class Reference;
    class DiffList;
    class Index;
    class Object;
    class ObjectCommit;
    class ObjectTag;
    class ObjectBlob;
    class ObjectTree;
    class ObjectId;
    class RevisionWalker;
    class Remote;

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
    class GITWRAP_API Repository
    {
    public:
        explicit Repository( const Internal::GitPtr< Internal::RepositoryPrivate >& _d );
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

        bool isValid() const;
        bool isBare() const;

        QString basePath() const;
        QString gitPath() const;

        QString name() const;

        QStringList allReferences( Result& result );
        QStringList allBranches( Result& result );
        QString currentBranch( Result& result );
        QStringList branches( bool local, bool remote, Result& result );
        QStringList allTags( Result& result );

        ResolvedRefs allResolvedRefs( Result& result );

        bool renameBranch( const QString& oldName, const QString& newName, bool force /* = false */,
                           Result& result );

        Index index( Result& result );

        Git::StatusFlags status(const QString &fileName, Result &result) const;
        Git::StatusHash status(Result &result) const;

        Reference HEAD( Result& result ) const;
        Reference lookupRef( const QString& refName, Result& result );
        ObjectId resolveRef( const QString& refName, Result& result );

        Object lookup( const ObjectId& id, ObjectType ot /* = otAny */,
                       Result& result );

        ObjectCommit lookupCommit( const ObjectId& id, Result& result );
        ObjectTree lookupTree( const ObjectId& id, Result& result );
        ObjectBlob lookupBlob( const ObjectId& id, Result& result );
        ObjectTag lookupTag( const ObjectId& id, Result& result );

        Object lookup( const QString& refName, ObjectType ot /* = otAny */,
                       Result& result );

        ObjectCommit lookupCommit( const QString& refName, Result& result );
        ObjectTree lookupTree( const QString& refName, Result& result );
        ObjectBlob lookupBlob( const QString& refName, Result& result );
        ObjectTag lookupTag( const QString& refName, Result& result );

        bool shouldIgnore( const QString& filePath, Result& result ) const;

        RevisionWalker newWalker( Result& result );

        QStringList allRemotes( Result& result ) const;
        Remote remote( const QString& remoteName, Result& result ) const;
        Remote createRemote( const QString& remoteName, const QString& url,
                             const QString& fetchSpec, Result& result );

        DiffList diffCommitToCommit( ObjectCommit oldCommit, ObjectCommit newCommit,
                                     Result& result );

        DiffList diffTreeToTree( ObjectTree oldTree, ObjectTree newTree,
                                 Result& result);

        DiffList diffIndexToTree( ObjectTree oldTree, Result& result );

        DiffList diffTreeToWorkingDir( ObjectTree oldTree, Result& result );
        DiffList diffIndexToWorkingDir( Result& result );

        SubmoduleList submodules( Result& result );
        Submodule submodule( const QString& name, Result& result );

    private:
        Internal::GitPtr< Internal::RepositoryPrivate > d;
    };

}

Q_DECLARE_METATYPE( Git::Repository )

#endif
