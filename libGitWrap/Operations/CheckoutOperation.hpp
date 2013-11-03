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

#ifndef GITWRAP_CHECKOUT_OPERATION_HPP
#define GITWRAP_CHECKOUT_OPERATION_HPP
#pragma once

#include "libGitWrap/Operations/BaseOperation.hpp"
#include "libGitWrap/FileInfo.hpp"  // Required for moc, only

namespace Git
{

    class Result;

    namespace Internal
    {

        class CheckoutBaseOperationPrivate;
        class CheckoutIndexOperationPrivate;
        class CheckoutTreeOperationPrivate;

    }

    class GITWRAP_API CheckoutBaseOperation : public BaseOperation
    {
        Q_OBJECT
    public:
        typedef Internal::CheckoutBaseOperationPrivate Private;

    protected:
        CheckoutBaseOperation(Private& _d, QObject* parent = 0);
    public:
        ~CheckoutBaseOperation();

    public:
        void setRepository(const Repository& repo);
        void setMode(CheckoutMode mode);
        void setOptions(CheckoutOptions opts);
        void setTargetDirectory(const QString& path);
        void setCheckoutPaths(const QStringList& paths);
        void setBaseline(const Tree& baseline);

        void setCancel(bool cancel);

    public:
        Repository repository() const;
        CheckoutMode mode() const;
        CheckoutOptions options() const;
        QString targetDirectory() const;
        QStringList checkoutPaths() const;
        Tree baseline() const;

    signals:
        void conflict  (const CheckoutBaseOperation* opertation,
                        const QString& path,
                        const FileInfo& baseline,
                        const FileInfo& target,
                        const FileInfo& workTree);

        void dirty     (const CheckoutBaseOperation* opertation,
                        const QString& path,
                        const FileInfo& baseline,
                        const FileInfo& target,
                        const FileInfo& workTree);

        void updated   (const CheckoutBaseOperation* opertation,
                        const QString& path,
                        const FileInfo& baseline,
                        const FileInfo& target,
                        const FileInfo& workTree);

        void untracked (const CheckoutBaseOperation* opertation,
                        const QString& path,
                        const FileInfo& baseline,
                        const FileInfo& target,
                        const FileInfo& workTree);

        void ignored   (const CheckoutBaseOperation* opertation,
                        const QString& path,
                        const FileInfo& baseline,
                        const FileInfo& target,
                        const FileInfo& workTree);

        void progress  (const CheckoutBaseOperation* opertation,
                        const QString& path,
                        quint32 completedSteps,
                        quint32 totalSteps);
    };

    class CheckoutIndexOperation : public CheckoutBaseOperation
    {
    public:
        typedef Internal::CheckoutIndexOperationPrivate Private;

    public:
        CheckoutIndexOperation(QObject* parent = 0);
        CheckoutIndexOperation(const Repository& repo, QObject* parent = 0);
        CheckoutIndexOperation(const Index& index, QObject* parent = 0);

    public:
        void setIndex(const Index& index);
        Index index() const;
    };

    class CheckoutTreeOperation : public CheckoutBaseOperation
    {
    public:
        typedef Internal::CheckoutTreeOperationPrivate Private;

    public:
        CheckoutTreeOperation(QObject* parent = 0);
        CheckoutTreeOperation(const Repository& repo, QObject* parent = 0);
        CheckoutTreeOperation(const Tree& tree, QObject* parent = 0);

    public:
        void setTree(const Tree& tree);
        Tree tree() const;
    };

}

#endif
