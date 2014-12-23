/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
 * (C) Cunz RaD Ltd.
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

#include "libGitWrap/RefLog.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/RefLogPrivate.hpp"
#include "libGitWrap/Private/RefLogEntryPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        //-- RefLogEntryPrivate -->8

        RefLogEntryPrivate::RefLogEntryPrivate(const git_reflog_entry* entry)
            : mEntry( entry )
        {
        }

        RefLogEntryPrivate::~RefLogEntryPrivate()
        {
        }


        //-- RefLogPrivate -->8

        RefLogPrivate::RefLogPrivate( RepositoryPrivate *repo, git_reflog *_reflog )
            : RepoObjectPrivate( repo )
            , reflog( _reflog )
        {
        }

        RefLogPrivate::RefLogPrivate( const RepositoryPrivate::Ptr& repo, git_reflog* _reflog )
            : RepoObjectPrivate(repo)
        {
            reflog = _reflog;
        }

        RefLogPrivate::~RefLogPrivate()
        {
            if (reflog) {
                git_reflog_free(reflog);
            }
        }

    }


    //-- RefLogEntry -->8

    GW_PRIVATE_IMPL(RefLogEntry, Base)

    Signature RefLogEntry::committer() const
    {
        GW_CD(RefLogEntry);
        const git_signature * sig = git_reflog_entry_committer( d->mEntry );
        return Internal::git2Signature( sig );
    }

    QString RefLogEntry::message() const
    {
        GW_CD(RefLogEntry);
        return GW_StringToQt( git_reflog_entry_message( d->mEntry ) );
    }

    ObjectId RefLogEntry::oidOld() const
    {
        GW_CD(RefLogEntry);
        const git_oid *id = git_reflog_entry_id_old( d->mEntry );
        return id ? ObjectId::fromRaw( id->id ) : ObjectId();
    }

    ObjectId RefLogEntry::oidNew() const
    {
        GW_CD(RefLogEntry);
        const git_oid *id = git_reflog_entry_id_new( d->mEntry );
        return id ? ObjectId::fromRaw( id->id ) : ObjectId();
    }


    //-- RefLog -->8

    GW_PRIVATE_IMPL(RefLog, RepoObject)

    int RefLog::count()
    {
        GW_CD(RefLog);
        return git_reflog_entrycount( d->reflog );
    }

    RefLogEntry RefLog::at(int index) const
    {
        GW_CD( RefLog );

        const git_reflog_entry *entry = git_reflog_entry_byindex( d->reflog, index);
        Q_ASSERT( entry );

        return new RefLogEntry::Private(entry);
    }

    RefLog RefLog::read( Result &result, const Repository& repo, const QString& refName )
    {
        GW_CHECK_RESULT(result, RefLog());

        Repository::Private* rp = Private::dataOf<Repository>(repo);

        git_reflog *out = NULL;
        result = git_reflog_read( &out, rp->mRepo, GW_StringFromQt(refName) );
        GW_CHECK_RESULT( result, RefLog() );

        return new RefLog::Private( rp, out );
    }

}
