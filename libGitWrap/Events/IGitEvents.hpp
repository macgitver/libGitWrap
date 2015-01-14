/*
 * MacGitver
 * Copyright (C) 2014 Sascha Cunz <sascha@macgitver.org>
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

#ifndef GITWRAP_IGIT_EVENTS_HPP
#define GITWRAP_IGIT_EVENTS_HPP
#pragma once

#include "libGitWrap/GitWrap.hpp"

namespace Git
{

    class CheckoutNotify;
    class CredentialRequest;
    class DiffFile;

    class GITWRAP_API IRemoteEvents
    {
    public:
        virtual ~IRemoteEvents();

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

    class GITWRAP_API ICheckoutEvents
    {
    public:
        virtual ~ICheckoutEvents();

    public:
        virtual void checkoutNotify( const CheckoutNotify& why,
                                     const QString& path,
                                     const DiffFile& baseline,
                                     const DiffFile& target,
                                     const DiffFile& workdir ) = 0;
        virtual void checkoutProgress( const QString& path,
                                       quint32 total,
                                       quint32 completed ) = 0;
        virtual void doneCheckout() = 0;
    };
}

#endif
