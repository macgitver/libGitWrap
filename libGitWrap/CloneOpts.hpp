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

#ifndef GITWRAP_CLONEOPTS_H
#define GITWRAP_CLONEOPTS_H

#include "GitWrap.hpp"
#include "ObjectId.hpp"

namespace Git
{

    class Repository;
    class CredentialRequest;
    class CloneOpts;

    namespace Internal
    {
        class CloneOptsPrivate;
    }

    class GITWRAP_API CloneEvents : public QObject
    {
        Q_OBJECT
    public:
        CloneEvents( QObject* parent = 0 );

    public:
        CloneOpts options() const;

    // to avoid moc complaining that signals may not be virtual.
    // They may indeed be virtual. It's just that they will only
    // be emitted if you overwrite them and miss to call the base
    // class. However, this can be desired, also.
    // i.e. a deviat of CloneEvents might implement all events and
    // not want signals being emitted.
    #ifdef Q_MOC_RUN
        #define V_SIGNAL
    #else
        #define V_SIGNAL virtual
    #endif

    signals:
        V_SIGNAL void askCredentials( CredentialRequest& request );
        V_SIGNAL void transportProgress( unsigned int totalObjects,
                                         unsigned int indexedObjects,
                                         unsigned int receivedObjects,
                                         size_t receivedBytes );
        V_SIGNAL void doneDownloading();
        V_SIGNAL void doneIndexing();
        V_SIGNAL void error();
        V_SIGNAL void remoteMessage( const QString& message );
        V_SIGNAL void updateTip( const QString& branchName,
                                 const Git::ObjectId& from,
                                 const Git::ObjectId& to );
    #undef V_SIGNAL

    private:
        friend class Internal::CloneOptsPrivate;
        Internal::CloneOptsPrivate*     mOpts;
    };

    class GITWRAP_API CloneOpts
    {
    public:
        CloneOpts( Internal::CloneOptsPrivate* _d );
        CloneOpts();
        CloneOpts( const CloneOpts& other );
        ~CloneOpts();
        CloneOpts& operator=( const CloneOpts& other );

        bool operator==( const CloneOpts& other ) const;

    public:
        void setEvents( CloneEvents* events );
        CloneEvents* events() const;

    public:
        void setUrl( const QByteArray& url );
        void setPath( const QByteArray& path );

    public:
        void setBare( bool bare );
        void setRemoteName( const QByteArray& remoteName );
        void setFetchSpec( const QByteArray& fetchSpec );
        void setPushSpec( const QByteArray& pushSpec );
        void setPushUrl( const QByteArray& pushUrl );

    public:
        QByteArray url() const;
        QByteArray path() const;
        bool bare() const;

        QByteArray remoteName() const;
        QByteArray fetchSpec() const;
        QByteArray pushSpec() const;
        QByteArray pushUrl() const;

    private:
        friend class Repository;
        operator const Internal::CloneOptsPrivate*() const
        {
            return d;
        }

    private:
        Internal::GitPtr< Internal::CloneOptsPrivate > d;
    };

}

#endif
