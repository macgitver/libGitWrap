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

#include "libGitWrap/Tag.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/TagPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        TagPrivate::TagPrivate(const RepositoryPrivate::Ptr& repo, git_tag* o)
            : ObjectPrivate(repo, reinterpret_cast<git_object*>(o))
        {
            Q_ASSERT(o);
        }

        TagPrivate::TagPrivate(const RepositoryPrivate::Ptr& repo, git_object* o)
            : ObjectPrivate(repo, o)
        {
            Q_ASSERT(o);
            Q_ASSERT(git_object_type(o) == GIT_OBJ_TAG);
        }

        git_otype TagPrivate::otype() const
        {
            return GIT_OBJ_TAG;
        }

        ObjectType TagPrivate::objectType() const
        {
            return otTag;
        }

    }


    // -- static methods -->8

    /**
     * @ingroup         GitWrap
     *
     * @brief           Create a tag object in the database.
     *
     *                  The created tag can be looked up by the tag´s ObjectId.
     *
     * @param[in,out]   result  a result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       name    the tag name
     * @param[in]       target  the target object, the tag points to
     * @param[in]       tagger  the signature of the tag author
     * @param[in]       message the message is assigned to the tag
     * @param[in]       force   If true, an existing tag matching the @a name will be replaced.
     *
     * @return              The @ref ObjectId of the created tag or an invalid object,
     *                      if creation failed.
     */
    ObjectId Tag::create(Result& result, const QString& name, const Object& target,
                         const Signature& tagger, const QString& message, bool force)
    {
        GW_CHECK_RESULT( result, ObjectId() );

        if ( !target.isValid() ) {
            result.setInvalidObject();
            return ObjectId();
        }

        Internal::ObjectPrivate* op = Internal::BasePrivate::dataOf<Object>( target );

        git_oid tagId;
        result = git_tag_create( &tagId, op->repo()->mRepo,
                                 GW_StringFromQt(name), op->mObj,
                                 Internal::signature2git(result, tagger),
                                 GW_StringFromQt(message), force
                                 );
        GW_CHECK_RESULT( result, ObjectId() );

        return ObjectId::fromRaw( tagId.id );
    }

    /**
     * @ingroup         GitWrap
     *
     * @brief           Create a lightweight tag object in the Git database.
     *
     *                  Unlike normal tags, lightweight tags don't have
     *                  a message and signature.
     *
     *                  The created tag can be looked up by the tag´s ObjectId.
     *
     * @param[in,out]   result  a result object; see @ref GitWrapErrorHandling
     * @param[in]       name    the tag name
     * @param[in]       target  the target object, the tag points to
     * @param[in]       tagger  the signature of the tag author
     * @param[in]       message the message is assigned to the tag
     * @param[in]       force   If true, an existing tag matching the @a name will be replaced.
     *
     * @return              The @ref ObjectId of the created tag or an invalid object,
     *                      if creation failed.
     */
    ObjectId Tag::createLight(Result& result, const QString& name, const Object& target, bool force)
    {
        GW_CHECK_RESULT( result, ObjectId() );

        if ( !target.isValid() ) {
            result.setInvalidObject();
            return ObjectId();
        }

        Internal::ObjectPrivate* op = Internal::BasePrivate::dataOf<Object>( target );

        git_oid tagId;
        result = git_tag_create_lightweight( &tagId, op->repo()->mRepo,
                                             GW_StringFromQt(name),
                                             op->mObj, force
                                             );
        GW_CHECK_RESULT( result, ObjectId() );

        return ObjectId::fromRaw( tagId.id );
    }

    /**
     * @ingroup                 GitWrap
     *
     * @brief                   Lookup an existing tag by the tag´s @ref ObjectId.
     *
     * @param[in,out]           result  a result object; see @ref GitWrapErrorHandling
     *
     * @param[in]               repo    the repository
     *
     * @param[in]               tagId   the ObjectId of the tag to look up
     *
     * @return                  the found tag object or an invalid @ref Tag object
     */
    Tag Tag::lookup(Result& result, const Repository& repo, const ObjectId& tagId)
    {
        GW_CHECK_RESULT( result, Tag() );

        if ( !repo.isValid() ) {
            result.setInvalidObject();
            return Tag();
        }

        Internal::RepositoryPrivate* rp = Internal::BasePrivate::dataOf<Repository>( repo );
        git_tag* out = NULL;
        git_tag_lookup( &out, rp->mRepo, Internal::ObjectId2git(tagId) );
        GW_CHECK_RESULT( result, Tag() );

        return new Internal::TagPrivate( Repository::PrivatePtr(rp), out );
    }

    GW_PRIVATE_IMPL(Tag, Object)

}
