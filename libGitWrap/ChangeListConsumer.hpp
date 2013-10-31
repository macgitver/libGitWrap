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

#ifndef GIT_CHANGELIST_CONSUMER_H
#define GIT_CHANGELIST_CONSUMER_H

#include "libGitWrap/GitWrap.hpp"

namespace Git
{

    /**
     * @ingroup GitWrap
     * @brief The ChangeListEntry struct keeps information about a single file change.
     */
    struct GITWRAP_API ChangeListEntry
    {
    public:
        enum Type
        {
            FileUnmodified,
            FileAdded,
            FileDeleted,
            FileModified,
            FileRenamed,
            FileCopied,
            FileIgnored,
            FileUntracked
        };

    public:
        QString         oldPath;
        QString         newPath;
        Type            type;
        unsigned int    similarity;
        bool            isBinary;
    };

    /**
     * @ingroup     GitWrap
     * @brief       Callback interface to consume a list of differences
     *
     */
    class GITWRAP_API ChangeListConsumer
    {
    public:
        typedef ChangeListEntry::Type Type; // For compat only

    public:
        ChangeListConsumer();
        virtual ~ChangeListConsumer();

    public:
        virtual bool startFileChange( const ChangeListEntry &entry );
    };

}

#endif
