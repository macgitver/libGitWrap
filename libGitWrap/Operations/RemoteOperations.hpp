/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2014 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Nils Fenner <nils@macgitver.org>
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

#include "libGitWrap/Operations/BaseOperation.hpp"

#include "libGitWrap/Events/IGitEvents.hpp"

namespace Git
{

    namespace Internal
    {
        class BaseRemoteOperationPrivate;
        class FetchOperationPrivate;
        class PushOperationPrivate;
    }


    class GITWRAP_API BaseRemoteOperation : public BaseOperation, public IRemoteEvents
    {
        Q_OBJECT

    public:
        typedef Internal::BaseRemoteOperationPrivate Private;

    public:
        explicit BaseRemoteOperation(Private& _d, QObject* parent);
        virtual ~BaseRemoteOperation();

    signals:
        void askCredentials(CredentialRequest& request);
        void transportProgress(quint32 totalObjects, quint32 indexedObjects, quint32 receivedObjects, quint64 receivedBytes);
        void doneDownloading();
        void doneIndexing();
        void error();
        void remoteMessage(const QString& message);
        void updateTip(const QString& branchName, const ObjectId& from, const ObjectId& to);

    public:
        const Repository& repository() const;

        QString remoteAlias() const;
        void setRemoteAlias(const QString& alias);

        const QStringList& refSpecs() const;
        void setRefSpecs(const QStringList& refSpecs);

        const Signature& signature() const;
        void setSignature(const Signature& sig);

        QString refLogMessage() const;
        void setRefLogMessage(const QString& msg);

    protected:
        inline void setRepository(const Repository& repo);
    };


    class GITWRAP_API FetchOperation : public BaseRemoteOperation
    {
        Q_OBJECT

    public:
        typedef Internal::FetchOperationPrivate Private;

    public:
        explicit FetchOperation(const Repository& repo, QObject* parent = 0);
    };


    class GITWRAP_API PushOperation : public BaseRemoteOperation
    {
        Q_OBJECT

    public:
        typedef Internal::PushOperationPrivate Private;

    public:
        explicit PushOperation(const Repository& repo, QObject* parent = 0);

    public:
        unsigned int pbParallellism() const;
        void setPBParallelism(unsigned int maxThreads);
    };
}
