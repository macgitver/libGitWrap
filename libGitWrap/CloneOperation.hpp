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

#ifndef GITWRAP_CLONE_OPERATION_HPP
#define GITWRAP_CLONE_OPERATION_HPP

#include "IFetchEvents.hpp"

namespace Git
{

    class Result;
    class CredentialRequest;

    namespace Internal
    {
        class CloneOperationPrivate;
    }

    class GITWRAP_API CloneOperation : public QObject, public IFetchEvents
    {
        Q_OBJECT
    public:
        CloneOperation( QObject* parent = 0 );
        ~CloneOperation();

    public:
        Result execute();

    public slots:
        void setBackgroundMode( bool backgroundMode );
        void setUrl( const QString& url );
        void setPath( const QString& path );
        void setBare( bool bare );
        void setRemoteName( const QByteArray& remoteName );
        void setFetchSpec( const QByteArray& fetchSpec );
        void setPushSpec( const QByteArray& pushSpec );
        void setPushUrl( const QByteArray& pushUrl );

    public:
        QString url() const;
        QString path() const;
        bool bare() const;
        bool backgroundMode() const;
        bool isRunning() const;

        QByteArray remoteName() const;
        QByteArray fetchSpec() const;
        QByteArray pushSpec() const;
        QByteArray pushUrl() const;

        Result result() const;

    signals:
        void askCredentials( CredentialRequest& request );
        void transportProgress( unsigned int totalObjects,
                                unsigned int indexedObjects,
                                unsigned int receivedObjects,
                                size_t receivedBytes );
        void doneDownloading();
        void doneIndexing();
        void error();
        void remoteMessage( const QString& message );
        void updateTip( const QString& branchName,
                        const Git::ObjectId& from,
                        const Git::ObjectId& to );
        void finished();

    private slots:
        void workerFinished();

    private:
        Internal::CloneOperationPrivate* d;
    };

}

#endif
