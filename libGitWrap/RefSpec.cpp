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

#include "libGitWrap/RefSpec.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        RefSpec mkRefSpec( const git_refspec* refspec )
        {
            if( !refspec )
            {
                return RefSpec();
            }

            QString src = GW_StringToQt( git_refspec_src( refspec ) );
            QString dst = GW_StringToQt( git_refspec_dst( refspec ) );
            return RefSpec( src, dst );
        }

    }

    RefSpec::RefSpec()
    {
    }

    RefSpec::RefSpec( const RefSpec& other )
        : mSrc( other.mSrc )
        , mDst( other.mDst )
    {
    }

    RefSpec::RefSpec( const QString& source, const QString& destination )
        : mSrc( source )
        , mDst( destination )
    {
    }

    QString RefSpec::source() const
    {
        return mSrc;
    }

    QString RefSpec::destination() const
    {
        return mDst;
    }

}
