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

#ifndef GW_FILEINFO_HPP
#define GW_FILEINFO_HPP
#pragma once

#include <QString>

#include "libGitWrap/ObjectId.hpp"

namespace Git
{

    class GITWRAP_API FileInfo
    {
    public:
        FileInfo(const QString& _fileName, ObjectId _sha1, qint64 _size, FileModes _mode,
                 bool _binary, bool _shaKnown)
            : mSize(_size)
            , mSHA1(_sha1)
            , mFileName(_fileName)
            , mFileMode(_mode)
            , mIsBinary(_binary)
            , mIsSHAKnown(_shaKnown)
            , mIsValid(true)
        {}

        FileInfo()
            : mSize(0), mFileMode(UnkownAttr), mIsBinary(false), mIsSHAKnown(false), mIsValid(false)
        {}

        ~FileInfo()
        {}

        FileInfo(const FileInfo& other)
            : mSize(other.mSize)
            , mSHA1(other.mSHA1)
            , mFileName(other.mFileName)
            , mFileMode(other.mFileMode)
            , mIsBinary(other.mIsBinary)
            , mIsSHAKnown(other.mIsSHAKnown)
            , mIsValid(other.mIsValid)
        {
        }

    public:
        bool isValid() const        { return mIsValid;    }
        bool isBinary() const       { return mIsBinary;   }
        bool isSHAKnwon() const     { return mIsSHAKnown; }
        qint64 size() const         { return mSize;       }
        QString fileName() const    { return mFileName;   }
        FileModes fileMode() const  { return mFileMode;   }
        ObjectId sha() const        { return mSHA1;       }

    private:
        qint64              mSize;
        ObjectId            mSHA1;
        QString             mFileName;
        FileModes           mFileMode;
        bool                mIsBinary       : 1;
        bool                mIsSHAKnown     : 1;
        bool                mIsValid        : 1;
    };

}

#endif
