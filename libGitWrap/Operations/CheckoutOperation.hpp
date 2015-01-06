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

#ifndef GITWRAP_CHECKOUT_OPERATION_HPP
#define GITWRAP_CHECKOUT_OPERATION_HPP
#pragma once

#include "libGitWrap/Events/IGitEvents.hpp"

#include "libGitWrap/Operations/BaseOperation.hpp"
#include "libGitWrap/Operations/Providers.hpp"


namespace Git
{

    class Result;

    namespace Internal
    {
        class CheckoutBaseOperationPrivate;
        class CheckoutIndexOperationPrivate;
        class CheckoutTreeOperationPrivate;
        class CheckoutReferenceOperationPrivate;
    }

    class GITWRAP_API CheckoutBaseOperation : public BaseOperation, public ICheckoutEvents
    {
        Q_OBJECT
        #if QT_VERSION < 0x050000
        friend class Internal::CheckoutBaseOperationPrivate;
        #endif
    public:
        typedef Internal::CheckoutBaseOperationPrivate Private;

    protected:
        CheckoutBaseOperation(Private& _d, QObject* parent = 0);

    public:
        ~CheckoutBaseOperation();

    public:
        void setRepository(const Repository& repo);
        void setMode(CheckoutMode mode);
        void setStrategy(CheckoutFlags strategy);
        void setTargetDirectory(const QString& path);
        void setCheckoutPaths(const QStringList& paths);
        void setBaseline(const Tree& baseline);

        void setCancel(bool cancel);

    public:
        Repository repository() const;
        CheckoutMode mode() const;
        CheckoutFlags strategy() const;
        QString targetDirectory() const;
        QStringList checkoutPaths() const;
        Tree baseline() const;

    signals:
        // ICheckoutEvents interface
        void checkoutNotify( const CheckoutNotify& why,
                             const QString& path,
                             const DiffFile& baseline,
                             const DiffFile& target,
                             const DiffFile& workdir );
        void checkoutProgress( const QString& path,
                               quint32 total,
                               quint32 completed );
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
        CheckoutTreeOperation(TreeProviderPtr tp, QObject* parent = 0);

    public:
        void setTreeProvider(TreeProviderPtr tp);
        TreeProviderPtr treeProvider() const;
    };

    class CheckoutReferenceOperation : public CheckoutBaseOperation
    {
    public:
        typedef Internal::CheckoutReferenceOperationPrivate Private;

    public:
        explicit CheckoutReferenceOperation(const Reference& branch, QObject* parent = 0);

    public:
        Reference branch() const;

    private:
        inline void setBranch(const Reference& ref);
    };

}

#endif
