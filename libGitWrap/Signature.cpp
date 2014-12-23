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

#include "libGitWrap/Signature.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/RepositoryPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        Signature git2Signature( const git_signature* gitsig )
        {
            Q_ASSERT( gitsig );

            QDateTime dt = QDateTime::fromTime_t( gitsig->when.time );
            dt.setUtcOffset( gitsig->when.offset * 60 );

            return Signature(
                GW_StringToQt( gitsig->name ),
                GW_StringToQt( gitsig->email ),
                dt );
        }

        git_signature* signature2git(Result& result, const Signature& sig)
        {
            if( !result )
            {
                return NULL;
            }

            git_signature* gitsig = 0;

            result = git_signature_new( &gitsig,
                                        GW_StringFromQt(sig.name()),
                                        GW_StringFromQt(sig.email()),
                                        sig.when().toTime_t(),
                                        sig.when().utcOffset() / 60 );

            return gitsig;
        }

    }

    /**
     * @brief           Creates a default signature from the Git configuration.
     *
     * @param[in,out]   result  a result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       repo    the repository (may be invalid)
     *
     * @return          the default signature object on success or an invalid object
     */
    Signature Signature::defaultSignature(Result &result, const Repository &repo)
    {
        if (!result) return Signature();
        if (!repo.isValid())
        {
            result.setInvalidObject();
            return Signature();
        }

        Internal::RepositoryPrivate* rp = Repository::Private::dataOf<Repository>(repo);
        git_signature* gitsig = 0;
        result = git_signature_default( &gitsig, rp->mRepo );
        if (!result) return Signature();

        return Internal::git2Signature( gitsig );
    }

}

QDebug operator<<( QDebug debug, const Git::Signature& sig )
{
    return debug << "Sig(" << sig.toNaturalString() << ")";
}
