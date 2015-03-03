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

#include "libGitWrap/Result.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"

namespace Git
{

    /**
     * @internal
     */
    Result::Result( int resultCode )
        : mCode( 0 )
        , mClass( 0 )
    {
        setError( resultCode );
    }

    /**
     * @internal
     */
    void Result::setInvalidObject()
    {
        if ( mCode )
        {
            // result is already set
            return;
        }

        mCode = -255;
        mClass = -1;
        mText = QStringLiteral( "An invalid GitWrap object was used." );
    }

    /**
     * @internal
     * @brief       _Import_ an error from `libgit2`
     *
     * @param[in]   resultCode	The result code as returned from `libgit2`
     *
     */
    void Result::setError( int resultCode )
    {
        if ( mCode )
        {
            // result is already set
            return;
        }

        mCode = resultCode;
        if( mCode < 0 )
        {
            const git_error* err = giterr_last();
            if( err )
            {
                mClass = err->klass;
                mText = GW_StringToQt( err->message );
            }
            else
            {
                qDebug() << "Notice: LibGit2 returned" << mCode << "but did not supply a "
                            "suitable error message.";
            }
        }
    }

    /**
     * @internal
     * @brief       Set an error text.
     *
     * Sets @a szErrorText as text and @a code as error code. Sets the class to -1, which cannot be
     * confused with libgit2 error classes.
     *
     * @param[in]   szErrorText     The text to set as error code.
     *
     * @param[in]   code            An error code to use. (Default to GIT_ERROR)
     *
     */
    void Result::setError(const char* szErrorText, int code)
    {
        if ( mCode )
        {
            // result is already set
            return;
        }

        mClass = -1;
        mCode = code;
        mText = GW_StringToQt(szErrorText);
    }

    /**
     * @internal
     * @brief       Assignment operator
     *
     * @param[in]   resultCode  The result code as returned from `libgit2`
     *
     * @return      A reference to this result object
     *
     * Just a short-cut for setError()
     */
    Result& Result::operator =( int resultCode )
    {
        setError( resultCode );
        return *this;
    }

    /**
     * @brief       Clear this result
     *
     * The result will become _valid_ again.
     *
     */
    void Result::clear()
    {
        mCode = 0;
    }

}
