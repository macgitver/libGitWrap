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

#include "GitWrapPrivate.hpp"
#include "ObjectId.hpp"


namespace Git
{


IndexEntry::IndexEntry()
{
}

IndexEntry::IndexEntry( const IndexEntry& other )
    : d( other.d )
{
}

IndexEntry::IndexEntry( const git_index_entry *entry )
    : d( new git_index_entry )
{
    Q_ASSERT(entry);

    d->ctime = entry->ctime;
    d->mtime = entry->mtime;

    d->dev   = entry->dev;
    d->ino   = entry->ino;
    d->mode  = entry->mode;
    d->uid   = entry->uid;
    d->gid   = entry->gid;
    d->file_size = entry->file_size;
    d->oid   = entry->oid;
    d->flags = entry->flags;
    d->flags_extended = entry->flags_extended;

    QByteArray a(entry->path);
    int len = a.size();
    d->path = new char(len);
    memcpy( d->path, a.data(), len );
}

IndexEntry::~IndexEntry()
{
    if (d != NULL)
    {
        delete d->path;
        delete d;
    }
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
    return QString::fromUtf8(d->path);
}

ObjectId IndexEntry::blobSha() const
{
    return ObjectId::fromRaw( d->oid.id );
}

unsigned int IndexEntry::mode() const
{
    return d->mode;
}

unsigned int IndexEntry::ino() const
{
    return d->ino;
}

unsigned int IndexEntry::uid() const
{
    return d->uid;
}

unsigned int IndexEntry::gid() const
{
    return d->gid;
}

QTime IndexEntry::cTime() const
{
    git_index_time t = d->ctime;
    return QTime( 0, 0, t.seconds );
}

QTime IndexEntry::mTime() const
{
    git_index_time t = d->mtime;
    return QTime(0, 0, t.seconds );
}

int IndexEntry::stage() const
{
    return git_index_entry_stage( d );
}

}
