/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Nils Fenner <nilsfenner@web.de>
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

#ifndef STATUS_H
#define STATUS_H

#include "GitWrap.hpp"

namespace Git
{
    class Repository;

    namespace Internal
    {
        class RepositoryPrivate;
        class StatusPrivate;
    }

    /**
     * Status flags for a single file.
     */
    enum FileStatus
    {
        StatusCurrent               = 0,

        StatusIndexNew              = (1 << 0),
        StatusIndexModified         = (1 << 1),
        StatusIndexDeleted          = (1 << 2),
        StatusIndexRenamed          = (1 << 3),
        StatusIndexTypeChange       = (1 << 4),

        StatusWorkingTreeNew        = (1 << 7),
        StatusWorkingTreeModified   = (1 << 8),
        StatusWorkingTreeDeleted    = (1 << 9),
        StatusWorkingTreeTypeChange = (1 << 10),

        StatusIgnored               = (1 << 14)
    };
    typedef QFlags< FileStatus > FileStatusFlags;

    typedef QHash< QString, FileStatusFlags > FileStatusHash;


    class GITWRAP_API Status
    {
    public:
        Status();
        ~Status();
        explicit Status(const Internal::GitPtr< Internal::StatusPrivate >& _d );
        Status( const Status& other );
        Status& operator=( const Status& other );

    public:
        FileStatusHash all( Result& result GITWRAP_DEFAULT_TLSRESULT ) const;

        /**
         * @brief fileStatus Reads the status of a file.
         * The file status is a combination of worktree, index and repository HEAD.
         * @param repo the parent repository
         * @param fileName the file path relative to the repository
         * @param r the error result
         * @return the current file status
         */
        FileStatusFlags fileStatus(QString &fileName, Result &result GITWRAP_DEFAULT_TLSRESULT) const;

    private:
        Internal::GitPtr< Internal::StatusPrivate > d;
    };

}

#endif // STATUS_H
