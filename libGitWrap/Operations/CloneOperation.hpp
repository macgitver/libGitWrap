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

#pragma once

#include "libGitWrap/Events/IGitEvents.hpp"

#include "libGitWrap/Operations/RemoteOperations.hpp"

namespace Git
{

    namespace Internal
    {
        class CloneOperationPrivate;
    }

    class CheckoutNotify;


    class GITWRAP_API CloneOperation : public BaseRemoteOperation, public ICheckoutEvents
    {
        Q_OBJECT
    public:
        typedef Internal::CloneOperationPrivate Private;

    public:
        CloneOperation( QObject* parent = 0 );

    public slots:
        void setUrl(const QString& url);
        void setPath(const QString& path);
        void setBare(bool bare);
        void setDepth(uint depth);
        void setReference(const QString& refName);

    public:
        QString url() const;
        QString path() const;
        bool bare() const;
        uint depth() const;
        QString reference() const;

    signals:
        void checkoutNotify( const CheckoutNotify& why,
                             const QString& path,
                             const DiffFile& baseline,
                             const DiffFile& target,
                             const DiffFile& workdir );
        void checkoutProgress( const QString& path,
                               quint64 total,
                               quint64 completed );
        void doneCheckout();
    };

}
