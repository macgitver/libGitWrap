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
#ifndef GITWRAP_COMMIT_OPERATION_HPP
#define GITWRAP_COMMIT_OPERATION_HPP

#include "libGitWrap/Operations/BaseOperation.hpp"
#include "libGitWrap/Operations/Providers.hpp"

namespace Git
{
    class Result;
    class Signature;

    namespace Internal
    {
        class CommitOperationPrivate;
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
        CommitOperation( ParentProviderPtr pp, TreeProviderPtr tp, const QString& msg, QObject* parent = 0 );
        ~CommitOperation();

    signals:
        void progress(CommitOperation *owner, const QString& pathName, quint32 completed, quint32 total);

    public:
        TreeProviderPtr treeProvider() const;
        void setTreeProvider(const TreeProviderPtr& p );

        ParentProviderPtr parentProvider() const;
        void setParentProvider(const ParentProviderPtr& p );

        QString message() const;
        void setMessage(const QString &message);

        Signature author() const;
        void setAuthor(const Signature &value);

        Signature committer() const;
        void setCommitter(const Signature &value);
    };
}

#endif
