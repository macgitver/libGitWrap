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

#include "Status.hpp"

#include "StatusPrivate.hpp"

#include "Repository.hpp"
#include "RepositoryPrivate.hpp"


namespace Git
{

namespace Internal
{

    static int statusHashCB( const char* fn, unsigned int status, void* rawSH )
    {
        #if 0
        qDebug( "%s - %s", qPrintable( QString::number( status, 2 ) ), fn );
        #endif

        Git::FileStatusHash* sh = (FileStatusHash*) rawSH;
        sh->insert( QString::fromUtf8( fn ), FileStatusFlags( status ) );

        return GIT_OK;
    }

    StatusPrivate::StatusPrivate(const GitPtr<RepositoryPrivate> &repo)
        : RepoObject( repo )
    {
    }

    StatusPrivate::~StatusPrivate()
    {
        Q_ASSERT( mRepo );
        if( mRepo->mStatus == this )
        {
            mRepo->mStatus = NULL;
        }
    }

}


Status::Status()
{
}

Status::~Status()
{
}

Status::Status(const Internal::GitPtr<Internal::StatusPrivate> &_d)
    : d( _d )
{
}

Status::Status(const Status &other)
{
    *this = other;
}

Status &Status::operator =(const Status &other)
{
    d = other.d;
    return *this;
}

FileStatusFlags Status::fileStatus(QString &fileName, Result &result) const
{
    unsigned int status = GIT_STATUS_CURRENT;

    if ( !d )
    {
        result.setInvalidObject();
        return (FileStatusFlags)GIT_STATUS_CURRENT;
    }

    result = git_status_file( &status, d->repo()->mRepo, fileName.toUtf8().data() );

    return static_cast<FileStatusFlags>( status );
}

FileStatusHash Status::all(Result &result) const
{
    if( !result )
    {
        return FileStatusHash();
    }

    if( !d )
    {
        qDebug( "ERR: NO-STATUS" );
        result.setInvalidObject();
        return FileStatusHash();
    }
    qDebug( "1" );

    FileStatusHash sh;

    git_status_options opt = GIT_STATUS_OPTIONS_INIT;

    opt.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED
              | GIT_STATUS_OPT_INCLUDE_IGNORED
              | GIT_STATUS_OPT_INCLUDE_UNMODIFIED
              | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;

    result = git_status_foreach_ext( d->repo()->mRepo, &opt, &Internal::statusHashCB, (void*) &sh );
    if( !result )
    {
        return FileStatusHash();
    }

    qDebug( "OK" );
    return sh;
}

}
