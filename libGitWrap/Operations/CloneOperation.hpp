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

#ifndef GITWRAP_CLONE_OPERATION_HPP
#define GITWRAP_CLONE_OPERATION_HPP
#pragma once

#include "libGitWrap/Events/IGitEvents.hpp"

#include "libGitWrap/Operations/BaseOperation.hpp"

namespace Git
{

    namespace Internal
    {
        class CloneOperationPrivate;
    }

    class CheckoutNotify;


    class GITWRAP_API CloneOperation : public BaseOperation, public IRemoteEvents, public ICheckoutEvents
    {
        Q_OBJECT
    public:
        typedef Internal::CloneOperationPrivate Private;

    public:
        CloneOperation( QObject* parent = 0 );
        ~CloneOperation();

    public slots:
        void setUrl(const QString& url);
        void setPath(const QString& path);
        void setBare(bool bare);
        void setDepth(uint depth);
        void setReference(const QString& refName);
        void setRemoteAlias(const QString& alias);

    public:
        QString url() const;
        QString path() const;
        bool bare() const;
        uint depth() const;
        QString reference() const;
        QString remoteAlias() const;

    signals:
        // realization of IRemoteEvents
        void askCredentials( CredentialRequest& request );
        void transportProgress( quint32 totalObjects,
                                quint32 indexedObjects,
                                quint32 receivedObjects,
                                quint64 receivedBytes );
        void doneDownloading();
        void doneIndexing();
        void error();
        void remoteMessage( const QString& message );
        void updateTip( const QString& branchName,
                        const Git::ObjectId& from,
                        const Git::ObjectId& to );

        // realization of ICheckoutEvents
        void checkoutNotify( const CheckoutNotify& why,
                             const QString& path,
                             const DiffFile& baseline,
                             const DiffFile& target,
                             const DiffFile& workdir );
        void checkoutProgress( const QString& path,
                               quint32 total,
                               quint32 completed );
    };

}

#endif
