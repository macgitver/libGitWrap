/*
 * libGitWrap - A Qt wrapper library for libgit2
 * Copyright (C) 2013-2014 The MacGitver-Developers <dev@macgitver.org>
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
#ifndef GITWRAP_COMMIT_OPERATION_HPP
#define GITWRAP_COMMIT_OPERATION_HPP

#include "libGitWrap/Operations/BaseOperation.hpp"

namespace Git
{
    class Result;
    class Signature;

    namespace Internal
    {
        class CommitOperationPrivate;

        class CommitOperationProvider : public BaseOperationProvider
        {
        public:
            typedef QExplicitlySharedDataPointer<CommitOperationProvider> Ptr;

        public:
            virtual ~CommitOperationProvider() {}

        public:
            virtual Tree commitOperationTree(Result &result) = 0;
            virtual ObjectIdList commitOperationParents(Result &result) const = 0;
            virtual Repository commitOperationRepository() const = 0;
        };
    }

    class GITWRAP_API CommitOperation : public BaseOperation
    {
        Q_OBJECT
        #if QT_VERSION < 0x050000
        friend class Internal::CommitOperationPrivate;
        #endif
    public:
        typedef Internal::CommitOperationPrivate Private;
    public:
        CommitOperation( QObject* parent = 0 );
        ~CommitOperation();

    signals:
        void progress(CommitOperation *owner, const QString& pathName, quint32 completed, quint32 total);

    public:
        Internal::CommitOperationProvider::Ptr operationProvider() const;
        void setOperationProvider(const Internal::CommitOperationProvider::Ptr& p );

        QString message() const;
        void setMessage(const QString &message);

        Signature author() const;
        void setAuthor(const Signature &value);

        Signature committer() const;
        void setCommitter(const Signature &value);

        void setDefaultSignatures();
    };
}

#endif
