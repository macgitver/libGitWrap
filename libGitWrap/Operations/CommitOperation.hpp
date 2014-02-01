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
        class CommitBaseOperationPrivate;
    }


    class GITWRAP_API BaseOperationProvider : public QSharedData
    {
    public:
        virtual ~BaseOperationProvider() {}

    public:
        virtual bool prepare() { return true; }
        virtual bool finalize(const ObjectId& commitId) { return true; }
    };


    class GITWRAP_API CommitTreeProvider : public BaseOperationProvider
    {
    public:
        typedef QExplicitlySharedDataPointer<CommitTreeProvider> Ptr;

    public:
        virtual Tree tree() const = 0;
    };


    class GITWRAP_API CommitParentProvider : public BaseOperationProvider
    {
    public:
        typedef QExplicitlySharedDataPointer<CommitParentProvider> Ptr;

    public:
        virtual ObjectIdList parents() const = 0;
    };


    class GITWRAP_API CommitOperation : public BaseOperation
    {
        Q_OBJECT
        #if QT_VERSION < 0x050000
        friend class Internal::CommitBaseOperationPrivate;
        #endif
    public:
        typedef Internal::CommitBaseOperationPrivate Private;
    public:
        CommitOperation( QObject* parent = 0 );
        ~CommitOperation();

    signals:
        void progress(CommitOperation *owner, const QString& pathName, quint32 completed, quint32 total);

    public:
        CommitParentProvider::Ptr parentProvider() const;
        void setParentProvider(CommitParentProvider::Ptr p );

        CommitTreeProvider::Ptr treeProvider() const;
        void setTreeProvider(CommitTreeProvider::Ptr p );

        QString message() const;
        void setMessage(const QString &message);

        Signature author() const;
        void setAuthor(const Signature &value);

        Signature committer() const;
        void setCommitter(const Signature &value);
    };
}

#endif
