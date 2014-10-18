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

#include "libGitWrap/IndexEntry.hpp"
#include "libGitWrap/ObjectId.hpp"

#include "libGitWrap/Private/IndexEntryPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        IndexEntryPrivate::IndexEntryPrivate( const git_index_entry* entry )
            : mEntry( *entry )
        {
            mEntry.path = strdup( entry->path );
        }

        IndexEntryPrivate::~IndexEntryPrivate()
        {
            free( const_cast<char*>(mEntry.path) );
        }

    }

    /**
     * @class       IndexEntry
     * @ingroup     GitWrap
     * @brief       Information about an entry of a git index
     *
     * An IndexEntry object is a very short lived data container for an index' entry.
     */

    GW_PRIVATE_IMPL(IndexEntry, Base)

    QString IndexEntry::path() const
    {
        GW_CD(IndexEntry);
        return GW_StringToQt( d->mEntry.path );
    }

    ObjectId IndexEntry::blobSha() const
    {
        GW_CD(IndexEntry);
        return ObjectId::fromRaw( d->mEntry.id.id );
    }

    unsigned int IndexEntry::mode() const
    {
        GW_CD(IndexEntry);
        return d->mEntry.mode;
    }

    unsigned int IndexEntry::ino() const
    {
        GW_CD(IndexEntry);
        return d->mEntry.ino;
    }

    unsigned int IndexEntry::uid() const
    {
        GW_CD(IndexEntry);
        return d->mEntry.uid;
    }

    unsigned int IndexEntry::gid() const
    {
        GW_CD(IndexEntry);
        return d->mEntry.gid;
    }

    QDateTime IndexEntry::cTime() const
    {
        GW_CD(IndexEntry);
        git_index_time t = d->mEntry.ctime;
        return QDateTime::fromTime_t( t.seconds );
    }

    QDateTime IndexEntry::mTime() const
    {
        GW_CD(IndexEntry);
        git_index_time t = d->mEntry.mtime;
        return QDateTime::fromTime_t( t.seconds );
    }

    int IndexEntry::stage() const
    {
        GW_CD(IndexEntry);
        return git_index_entry_stage( &(d->mEntry) );
    }

}
