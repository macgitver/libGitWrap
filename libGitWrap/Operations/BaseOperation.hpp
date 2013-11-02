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

#ifndef GITWRAP_BASE_OPERATION_HPP
#define GITWRAP_BASE_OPERATION_HPP

#include <QObject>

#include "libGitWrap/GitWrap.hpp"

namespace Git
{

    class Result;

    namespace Internal
    {

        class BaseOperationPrivate;
    }

    class GITWRAP_API BaseOperation : public QObject
    {
        Q_OBJECT
    public:
        typedef Internal::BaseOperationPrivate Private;

    public:
        BaseOperation(Private& _d, QObject* parent = 0);
        ~BaseOperation();

    public:
        Result execute();

    public slots:
        void setBackgroundMode(bool backgroundMode);

    public:
        bool backgroundMode() const;
        bool isRunning() const;
        Result result() const;

    signals:
        void finished();

    private slots:
        void workerFinished();

    protected:
        QExplicitlySharedDataPointer<Private> mData;
    };

}

#endif
