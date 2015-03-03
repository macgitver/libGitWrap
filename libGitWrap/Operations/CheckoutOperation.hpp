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

#include "libGitWrap/Operations/BaseOperation.hpp"
#include "libGitWrap/Operations/Providers.hpp"


namespace Git
{

    class Result;

    namespace Internal
    {
        class CheckoutBaseOperationPrivate;
        class CheckoutCommitOperationPrivate;
        class CheckoutIndexOperationPrivate;
        class CheckoutReferenceOperationPrivate;
        class CheckoutTreeOperationPrivate;
    }

    class GITWRAP_API CheckoutBaseOperation : public BaseOperation, public ICheckoutEvents
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
        void setStrategy(unsigned int strategy);
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
                               quint64 total,
                               quint64 completed );
        void doneCheckout();
    };

    class GITWRAP_API CheckoutIndexOperation : public CheckoutBaseOperation
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

    class GITWRAP_API CheckoutTreeOperation : public CheckoutBaseOperation
    {
    public:
        typedef Internal::CheckoutTreeOperationPrivate Private;

    public:
        CheckoutTreeOperation(QObject* parent = 0);
        CheckoutTreeOperation(const Repository& repo, QObject* parent = 0);
        CheckoutTreeOperation(TreeProviderPtr tp, QObject* parent = 0);

    protected:
        explicit CheckoutTreeOperation(CheckoutTreeOperation::Private& _d, TreeProviderPtr tp, QObject* parent );

    public:
        void setTreeProvider(TreeProviderPtr tp);
        TreeProviderPtr treeProvider() const;
    };

    class GITWRAP_API CheckoutCommitOperation : public CheckoutTreeOperation
    {
    public:
        typedef Internal::CheckoutCommitOperationPrivate Private;

    public:
        explicit CheckoutCommitOperation(const Commit& commit, QObject* parent = 0);

    public:
        Commit commit();
    };

    class GITWRAP_API CheckoutReferenceOperation : public CheckoutTreeOperation
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
