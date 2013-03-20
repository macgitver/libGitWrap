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

#include "IndexEntry.hpp"
#include "IndexEntryPrivate.hpp"

#include "ObjectId.hpp"


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
        free( mEntry.path );
    }

}


IndexEntry::IndexEntry()
{
}

IndexEntry::IndexEntry( const IndexEntry& other )
    : d( other.d )
{
}

IndexEntry::IndexEntry(Internal::IndexEntryPrivate *_d )
    : d( _d )
{
    Q_ASSERT(d);
}

IndexEntry::~IndexEntry()
{
}

IndexEntry &IndexEntry::operator =(const IndexEntry &other)
{
    d = other.d;
    return *this;
}

bool IndexEntry::isValid() const
{
    return d;
}

QString IndexEntry::path() const
{
    return QString::fromUtf8( d->mEntry.path );
}

ObjectId IndexEntry::blobSha() const
{
    return ObjectId::fromRaw( d->mEntry.oid.id );
}

unsigned int IndexEntry::mode() const
{
    return d->mEntry.mode;
}

unsigned int IndexEntry::ino() const
{
    return d->mEntry.ino;
}

unsigned int IndexEntry::uid() const
{
    return d->mEntry.uid;
}

unsigned int IndexEntry::gid() const
{
    return d->mEntry.gid;
}

QTime IndexEntry::cTime() const
{
    git_index_time t = d->mEntry.ctime;
    return QTime( 0, 0, t.seconds );
}

QTime IndexEntry::mTime() const
{
    git_index_time t = d->mEntry.mtime;
    return QTime(0, 0, t.seconds );
}

int IndexEntry::stage() const
{
    return git_index_entry_stage( &(d->mEntry) );
}

}
