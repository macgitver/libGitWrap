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

#ifndef GITWRAP_IFETCH_EVENTS_HPP
#define GITWRAP_IFETCH_EVENTS_HPP

#include "libGitWrap/GitWrap.hpp"
#include "libGitWrap/ObjectId.hpp"

namespace Git
{

    class CredentialRequest;

    class GITWRAP_API IFetchEvents
    {
    public:
        virtual ~IFetchEvents();

    public:
        virtual void askCredentials( CredentialRequest& request ) = 0;
        virtual void transportProgress( quint32 totalObjects,
                                        quint32 indexedObjects,
                                        quint32 receivedObjects,
                                        quint64 receivedBytes ) = 0;
        virtual void doneDownloading() = 0;
        virtual void doneIndexing() = 0;
        virtual void error() = 0;
        virtual void remoteMessage( const QString& message ) = 0;
        virtual void updateTip( const QString& branchName,
                                 const Git::ObjectId& from,
                                 const Git::ObjectId& to ) = 0;
    };

}

#endif
