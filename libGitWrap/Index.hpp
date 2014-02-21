/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
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

#ifndef GIT_INDEX_H
#define GIT_INDEX_H

#include "libGitWrap/RepoObject.hpp"
#include "libGitWrap/Operations/Providers.hpp"

namespace Git
{

    class CommitOperation;

    namespace Internal
    {
        class IndexPrivate;
    }

    class GITWRAP_API Index : public RepoObject
    {
        GW_PRIVATE_DECL(Index, RepoObject, public)

    public:
        enum Stages {
            StageDefault    = 0,
            StageFrom       = 1,
            StageOurs       = 2,
            StageTheirs     = 3
        };

    public:
        static Index createInMemory();
        static Index openPath(Result& result, const QString& path);

    public:
        operator TreeProviderPtr() const;

    public:
        bool isBare() const;

        void read(Result& result, bool force = true);
        void write(Result& result);
        void clear();
        void readTree(Result& result, Tree& tree);
        Tree writeTree(Result& result);
        Tree writeTreeTo(Result& result, Repository& repo);

        // Methods to access and change entries
        int count( Result& result ) const;
        IndexEntry getEntry(Result &result, int n) const;
        IndexEntry getEntry(Result &result, const QString &path, Stages stage = StageDefault) const;
        void updateEntry(Result &result, const IndexEntry& entry);

        // Index-Entry methods working on a path
        void addFile(Result &result, const QString &path);
        void removeFile(Result &result, const QString &path);
        void resetFiles( Result &result, const QStringList &path );

        // Methods that operate on a glob (set of files)
        void checkoutFiles( Result &result, const QStringList &paths );

        bool hasConflicts() const;
        IndexConflicts conflicts() const;

        IndexEntry operator[](int index) const;
        IndexEntry operator[](const QString& path) const;
    };


    class GITWRAP_API IndexTreeProvider : public TreeProvider
    {
    public:
        IndexTreeProvider( const Index& index );

    public:

        // INTERFACE REALIZATION

        Tree tree(Result &result);

        Repository repository() const;

    private:
        Index   mIndex;
    };

}

Q_DECLARE_METATYPE( Git::Index )

#endif
