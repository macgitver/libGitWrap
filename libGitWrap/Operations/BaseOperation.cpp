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

#include "libGitWrap/Operations/BaseOperation.hpp"

#include "libGitWrap/Operations/Private/BaseOperationPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        BaseOperationPrivate::BaseOperationPrivate(BaseOperation* owner)
            : mOwner( owner )
        {
            mBackgroundMode = false;
            mThread = NULL;
        }

        BaseOperationPrivate::~BaseOperationPrivate()
        {
        }

    }

    BaseOperation::BaseOperation(Private& _d, QObject* parent)
        : QObject( parent )
        , mData( &_d )
    {
    }

    BaseOperation::~BaseOperation()
    {
    }

    void BaseOperation::setBackgroundMode( bool backgroundMode )
    {
        mData->mBackgroundMode = backgroundMode;
    }

    bool BaseOperation::backgroundMode() const
    {
        return mData->mBackgroundMode;
    }

    Result BaseOperation::execute()
    {
        if (mData->mBackgroundMode) {
            Q_ASSERT(!mData->mThread);

            mData->mThread = new Internal::WorkerThread(this, mData);

            connect(mData->mThread, SIGNAL(finished()), this, SLOT(workerFinished()));

            mData->mThread->start();
            return Result();
        }
        else
        {
            mData->run();
            return mData->mResult;
        }
    }

    bool BaseOperation::isRunning() const
    {
        return mData->mThread;
    }

    void BaseOperation::workerFinished()
    {
        delete mData->mThread;
        mData->mThread = NULL;
        emit finished();
    }

    Result BaseOperation::result() const
    {
        return mData->mResult;
    }

}
