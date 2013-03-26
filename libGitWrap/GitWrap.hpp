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

#ifndef GITWRAP_GITWRAP_H
#define GITWRAP_GITWRAP_H

#include <QString>
#include <QHash>
#include <QMetaType>
#include <QVector>

/** @defgroup GitWrap Qt-based wrapper for Git revision control featuring libgit2
 * Wrapper for the LibGit2 library based on Qt4/Qt5.
 */

#ifdef GitWrap_EXPORTS
#	define GITWRAP_API Q_DECL_EXPORT
#else
#	define GITWRAP_API Q_DECL_IMPORT
#endif

#define GITWRAP_DEFAULT_TLSRESULT /* Nothing */

namespace Git
{

    namespace Internal
    {

        template< class T >
        class GitPtr
        {
        public:
            GitPtr();
            GitPtr( const GitPtr< T >& o );
            GitPtr( T* o );
            ~GitPtr();

            GitPtr< T >& operator=( const GitPtr< T >& o );
            bool operator==( const GitPtr< T >& o ) const;
            bool operator==( T* o ) const;

            T* operator->();
            const T* operator->() const;

            T* operator*();
            const T* operator*() const;

            operator bool() const;
            operator T*();
            operator const T*() const;

            T* data() const;

        private:
            T* d;
        };

    }

    enum ObjectType
    {
        otTree,
        otCommit,
        otBlob,
        otTag,

        otAny = -1
    };

    enum TreeEntryAttributes
    {
        UnkownAttr          = 0,
        TreeAttr            = 0040000,
        FileAttr            = 0100644,
        FileExecutableAttr  = 0100755,
        GitLinkAttr         = 0120000,
        SubmoduleAttr       = 0160000
    };

    namespace File
    {

        /**
         * @enum        File::Status
         * @ingroup     GitWrap
         * @brief       Represents the modified state for a single file.
         *              The file status is combination of the current HEAD commit,
         *              the repository's index and the working tree.
         *
         * @var         Status::Unchanged
         *              The file is completely unchanged.
         *
         * @var         Status::IndexNew
         *              The file was added to the index and untracked before.
         *
         * @var         Status::IndexModified
         *              The file is modified within the index.
         *
         * @var         Status::IndexDeleted
         *              The file was removed from the index.
         *
         * @var         Status::Renamed
         *              The file was moved or renamed.
         *
         * @var         Status::IndexTypeChange
         *              The file type changed in the index.
         *
         * @var         Status::WorkingTreeNew
         *              Marks a new, untracked file in the working tree.
         *
         * @var         Status::WorkingTreeModified
         *              The file is modified.
         *
         * @var         Status::WorkingTreeDeleted
         *              The tracked file was deleted in the working tree.
         *
         * @var         Status::WorkingTreeTypeChange
         *              The file type changed in the working tree.
         *
         * @var         Status::Ignored
         *              The file is marked as ignored (i.e. it is filtered in .gitignore).
         *
         * @var         Status::InHead
         *              The superproject's HEAD commit contains the submodule.
         *
         * @var         Status::InIndex
         *              The superproject's contains an entry for the submodule.
         *
         * @var         Status::InConfig
         *              The superproject's .gitmodules file contains
         *              an entry for the submodule.
         *
         * @var         Status::InWorkingTree
         *              The superproject's working tree contains
         *              a folder matching the submodule's name.
         *
         * @var         Status::IndexModified
         *              The superproject's submodule entries in HEAD and index don't match.
         *
         * @var         Status::WorkingTreeUninitialized
         *              The folder for the submodule is empty.
         *
         * @var         Status::WorkingTreeIndexModified
         *              The submodule's index contains entries
         *              (submodule is "dirty").
         *
         * @var         Status::WorkingTreeWtModified
         *              The submodule's working tree contains modified files
         *              (submodule is dirty).
         *
         * @var         Status::WorkingTreeUntracked
         *              There are untracked files within the submodule's working tree.
         */
        enum Status
        {
            Unchanged             = 0,

            IndexNew              = (1u << 0),
            IndexModified         = (1u << 1),
            IndexDeleted          = (1u << 2),
            IndexRenamed          = (1u << 3),
            IndexTypeChange       = (1u << 4),

            WorkingTreeNew        = (1u << 7),
            WorkingTreeModified   = (1u << 8),
            WorkingTreeDeleted    = (1u << 9),
            WorkingTreeTypeChange = (1u << 10),

            Ignored               = (1u << 14),

            SubmoduleInHead                     = (1u << 16),
            SubmoduleInIndex                    = (1u << 17),
            SubmoduleInConfig                   = (1u << 18),
            SubmoduleInWorkingTree              = (1u << 19),
            SubmoduleIndexModified              = (1u << 21),
            SubmoduleWorkingTreeUninitialized   = (1u << 22),
            SubmoduleWorkingTreeIndexModified   = (1u << 26),
            SubmoduleWorkingTreeWtModified      = (1u << 27),
            SubmoduleWorkingTreeUntracked       = (1u << 28)
        };
        Q_DECLARE_FLAGS ( StatusFlags, Status )

        typedef QHash< QString, StatusFlags > StatusHash;

    }

    class Result;

    class GITWRAP_API GitWrap
    {
    public:
        GitWrap();
        ~GitWrap();

    public:
        static Result& lastResult();
    };
}

#endif
