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

#include "libGitWrap/Commit.hpp"
#include "libGitWrap/Tree.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Reference.hpp"
#include "libGitWrap/RefName.hpp"
#include "libGitWrap/BranchRef.hpp"
#include "libGitWrap/TagRef.hpp"
#include "libGitWrap/NoteRef.hpp"

#include "libGitWrap/Operations/CommitOperation.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/ObjectPrivate.hpp"
#include "libGitWrap/Private/ReferencePrivate.hpp"
#include "libGitWrap/Private/BranchRefPrivate.hpp"
#include "libGitWrap/Private/TagRefPrivate.hpp"
#include "libGitWrap/Private/NoteRefPrivate.hpp"

namespace Git
{
    /**
     * @class       Reference
     * @ingroup     GitWrap
     * @brief       Represents a git reference
     *
     */

    namespace Internal
    {

        ReferencePrivate::ReferencePrivate(RepositoryPrivate* repo, git_reference* ref)
            : RefNamePrivate(repo)
            , wasDeleted(false)
            , reference(ref)
        {
            Q_ASSERT(reference);
            fqrn = GW_StringToQt(git_reference_name(reference));
        }

        ReferencePrivate::ReferencePrivate(RepositoryPrivate* repo, const QString& name,
                                           git_reference* ref)
            : RefNamePrivate(repo, name)
            , wasDeleted(false)
            , reference(ref)
        {
            Q_ASSERT(reference);
        }

        ReferencePrivate::ReferencePrivate(git_reference* ref, const RefNamePrivate* refName)
            : RefNamePrivate(refName)
            , wasDeleted(false)
            , reference(ref)
        {
        }

        ReferencePrivate::~ReferencePrivate()
        {
            // We have to free the reference, no matter whether it was deleted or not.
            git_reference_free(reference);
        }

        bool ReferencePrivate::isRealReference() const
        {
            // This is used in RefName to determine if we can safely cast back to a Reference.
            return true;
        }

        ReferenceKinds ReferencePrivate::kind() const
        {
            return UnknownReference;
        }

        bool ReferencePrivate::isValidObject(Result &r) const
        {
            // don't check r here! it will be correctly set.

            if ( wasDeleted ) {
                r.setError("Tried to access a destroyed reference.", GIT_EUSER);
                return false;
            }
            return r;
        }

    }

    GW_PRIVATE_IMPL(Reference, RepoObject)

    /**
     * @brief       Compares two reference objects.
     *
     * @param[in]   other   The reference to compare against
     *
     * @return      `0` if pointing to the same destination, otherwise an ordered value
     *
     * Note: References of different types are considered to be different.
     *
     * This method sorts invalid Reference objects before valid ones. A Reference object that was
     * destroied with the destroy() method will be treated as invalid.
     *
     */
    int Reference::compare(const Reference &other) const
    {
        GW_CD(Reference);
        Private* od = Private::dataOf<Reference>(other);
        if (!d || d->wasDeleted) {
            return od ? -1 : 0;
        }

        if (!od || od->wasDeleted) {
            return 1;
        }

        return git_reference_cmp(d->reference, od->reference);
    }

    /**
     * @brief           Create a new reference
     *
     *                  Creates a new reference named @a name that is pointing
     *                  to the SHA1 @a sha in the repository @a repo.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       repo    The repository in which the reference shall be created.
     *
     * @param[in]       name    The full qualified name of the reference to be created. i.e.
     *                          `refs/heads/foobar` for branch `foobar`.
     *
     * @param[in]       sha     The SHA1 where the reference shall point to.
     *
     * @return          The created reference object or an invalid reference object,
     *                  if the an error occured while creating the reference.
     *
     * @note            This method does not test whether the repository really has
     *                  an object with the id @a sha.
     */
    Reference Reference::create(Result& result, const Repository& repo,
                                const QString& name, const ObjectId& sha)
    {
        GW_CHECK_RESULT( result, Reference() );

        if ( !repo.isValid() ) {
            result.setInvalidObject();
            return Reference();
        }

        Repository::Private* repop = Private::dataOf<Repository>(repo);

        git_reference* ref = nullptr;
        QByteArray utf8Name = GW_EncodeQString(name);

        result = git_reference_create(&ref, repop->mRepo, utf8Name.constData(),
                                      Private::sha(sha), false, nullptr, nullptr);
        GW_CHECK_RESULT( result, Reference() );

        return Private::createRefObject( result, repop, name, ref );
    }


    /**
     * @brief           Create a new reference
     *
     *                  Creates a new reference named @a name that is pointing
     *                  to the commit @a commit in the repository @a repo.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       repo    The repository in which the reference shall be created.
     *
     * @param[in]       name    The full qualified name of the reference to be created. i.e.
     *                          `refs/heads/foobar` for branch `foobar`.
     *
     * @param[in]       commit  The commit object where the new reference shall point to.
     *
     * @return          The created reference object or an invalid reference object,
     *                  if an error occured while creating the reference.
     */
    Reference Reference::create(Result& result, const Repository &repo,
                                const QString& name, const Commit& commit)
    {
        GW_CHECK_RESULT( result, Reference() );
        if ( !commit.isValid() ) {
            result.setInvalidObject();
            return Reference();
        }

        return create(result, repo, name, commit.id());
    }

    /**
     * @brief           Resolves a reference name directly to the target OID.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       repo    the owner repository
     *
     * @param[in]       name    the reference name
     *
     * @return          The resolved OID on success; otherwise an invalid ObjectId.
     */
    ObjectId Reference::nameToId(Git::Result& result, const Git::Repository& repo, const QString& name)
    {
        GW_CHECK_RESULT( result, ObjectId() );

        Repository::Private* rp = Base::Private::dataOf<Repository>( repo );
        if ( !rp )
        {
            result.setInvalidObject();
            return ObjectId();
        }

        git_oid out;
        result = git_reference_name_to_id( &out, rp->mRepo, GW_StringFromQt(name) );
        return ObjectId::fromRaw( out.id );
    }

    /**
     * @brief       This reference's name
     *
     * @return      Returns the reference name (fully qualified)
     */
    QString Reference::name() const
    {
        GW_CD(Reference);

        if (!d || d->wasDeleted) {
            return QString();
        }

        return d->fqrn;
    }

    /**
     * @brief       Reference analyzer for this reference
     *
     * @return      A new RefName object for this reference's name.
     *
     */
    RefName Reference::nameAnalyzer() const
    {
        GW_CD(Reference);
        return const_cast<Internal::ReferencePrivate*>(d);
    }

    /**
     * @brief   Returns the name prefix for the reference.
     *
     * @return  the name prefix or an empty string
     */
    QString Reference::prefix() const
    {
        const QString tmpName = name();
        return tmpName.left(tmpName.length() - shorthand().length());
    }

    QString Reference::shorthand() const
    {
        GW_CD(Reference);

        if (!d || d->wasDeleted) {
            return QString();
        }

        return GW_StringToQt(git_reference_shorthand(d->reference));
    }

    ReferenceTypes Reference::type() const
    {
        GW_CD(Reference);
        if (!d || d->wasDeleted) {
            return ReferenceInvalid;
        }

        switch (git_reference_type(d->reference)) {
        default:
        case GIT_REF_INVALID:   return ReferenceInvalid;
        case GIT_REF_SYMBOLIC:  return ReferenceSymbolic;
        case GIT_REF_OID:       return ReferenceDirect;
        }
    }

    ObjectId Reference::objectId() const
    {
        GW_CD(Reference);
        if (!d || d->wasDeleted) {
            return ObjectId();
        }

        if (type() != ReferenceDirect) {
            return ObjectId();
        }

        return Private::oid2sha(git_reference_target(d->reference));
    }

    QString Reference::target() const
    {
        GW_CD(Reference);
        if (!d || d->wasDeleted) {
            return QString();
        }

        if (type() != ReferenceSymbolic) {
            return QString();
        }

        return GW_StringToQt(git_reference_symbolic_target(d->reference));
    }

    Reference Reference::resolved( Result& result ) const
    {
        GW_CD_CHECKED(Reference, Reference(), result);

        git_reference* ref = nullptr;
        result = git_reference_resolve(&ref, d->reference);
        GW_CHECK_RESULT( result, Reference() );

        return new Private(d->repo(), ref);
    }

    ObjectId Reference::resolveToObjectId( Result& result ) const
    {
        Reference resolvedRef = resolved(result);
        GW_CHECK_RESULT( result, ObjectId() );

        return resolvedRef.objectId();
    }

    bool Reference::isCurrentBranch() const
    {
        GW_CD(Reference);
        return d && !d->wasDeleted && git_branch_is_head(d->reference);
    }

    bool Reference::isBranch() const
    {
        GW_CD(Reference);
        return d && !d->wasDeleted && git_reference_is_branch(d->reference);
    }

    bool Reference::isLocal() const
    {
        GW_CD(Reference);

        if (!d || d->wasDeleted) {
            return false;
        }

        return git_reference_is_branch(d->reference);
    }

    bool Reference::isRemote() const
    {
        GW_CD(Reference);

        if (!d || d->wasDeleted) {
            return false;
        }

        return git_reference_is_remote(d->reference);
    }

    Object Reference::peeled(Result& result, ObjectType ot) const
    {
        GW_CD_CHECKED(Reference, Object(), result);

        git_object* o = nullptr;
        result = git_reference_peel(&o, d->reference, Internal::objectType2git(ot));
        GW_CHECK_RESULT( result, Object() );

        return Object::Private::create(d->repo(), o);
    }

    /**
     * @brief           Delete this reference
     *
     * The reference is deleted from the underlying data store. This does not mean, that the
     * Reference object or any copies of it will become invalid, but when you try to invoke any
     * method on this Reference object, it will fail.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     */
    void Reference::destroy(Result& result)
    {
        GW_D_CHECKED(Reference, void(), result);

        result = git_reference_delete(d->reference);

        if (result) {
            d->wasDeleted = true;
        }
    }

    bool Reference::wasDestroyed() const
    {
        GW_CD(Reference);
        return d && d->wasDeleted;
    }

    void Reference::move(Result &result, const Commit &target)
    {
        GW_D_CHECKED(Reference, void(), result);

        ObjectId targetId = target.id();
        if (targetId.isNull()) {
            return;
        }

        git_reference* newRef = nullptr;
        result = git_reference_set_target(&newRef, d->reference, Private::sha(targetId),
                                          nullptr, nullptr);

        if (result && (newRef != d->reference)) {
            // even though we have a nre d->reference now, the name did not change. So nothing to
            // update down in RefName's data.
            git_reference_free(d->reference);
            d->reference = newRef;
        }
    }

    void Reference::rename(Result &result, const QString &newName, bool force)
    {
        GW_D_CHECKED(Reference, void(), result);

        git_reference* newRef = nullptr;
        result = git_reference_rename(&newRef, d->reference, GW_StringFromQt(newName), force,
                                      nullptr, nullptr);

        if (result && (newRef != d->reference)) {
            git_reference_free(d->reference);

            d->reference    = newRef;
            d->fqrn         = newName;  // Reset RefName's data
            d->isAnalyzed   = false;
        }
    }

    /**
     * @brief       Point the repository's HEAD here
     *
     * Set HEAD detached to where this reference currently points to.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     */
    void Reference::setAsDetachedHEAD(Result& result) const
    {
        GW_CD_CHECKED(Reference, void(), result);

        repository().setDetachedHEAD( result, peeled<Commit>(result).id() );
    }

    void Reference::updateHEAD(Result &result) const
    {
        GW_CD_CHECKED(Reference, void(), result);

        if (git_reference_is_branch(d->reference)) {
            // reference is a local branch
            result = git_repository_set_head(d->repo()->mRepo,
                                             git_reference_name(d->reference),
                                             nullptr, nullptr);
        }
        else {
            // reference is detached
            result = git_repository_set_head_detached(d->repo()->mRepo,
                                                      git_reference_target(d->reference),
                                                      nullptr, nullptr);
        }
    }

    /**
     * @brief Creates a suitable commit operation for the reference type.
     *
     * This method is overridden by inherited classes.
     *
     * @return a CommitOperation object.
     */
    CommitOperation* Reference::commitOperation(Result& result, const TreeProviderPtr& treeProvider, const QString& msg) const
    {
        GW_CD_CHECKED( Reference, nullptr, result );
        return new CommitOperation( *this, treeProvider, msg );
    }

    Reference::operator ParentProviderPtr() const
    {
        return ParentProviderPtr( new ReferenceParentProvider( *this ) );
    }

    Reference::operator TreeProviderPtr() const
    {
        return TreeProviderPtr( new ReferenceTreeProvider( *this ) );
    }

    ReferenceKinds Reference::kind() const
    {
        GW_CD(Reference);

        if (d && !d->wasDeleted) {
            return d->kind();
        }

        return UnknownReference;
    }

    BranchRef Reference::asBranch() const
    {
        if (kind() == BranchReference) {
            GW_CD_EX(BranchRef);
            return BranchRef(d);
        }
        return BranchRef();
    }

    TagRef Reference::asTag() const
    {
        if (kind() == TagReference) {
            GW_CD_EX(TagRef);
            return TagRef(d);
        }
        return TagRef();
    }

    NoteRef Reference::asNote() const
    {
        if (kind() == NoteReference) {
            GW_CD_EX(NoteRef);
            return NoteRef(d);
        }
        return NoteRef();
    }


    //-- ReferenceParentProvider -->8

    ReferenceParentProvider::ReferenceParentProvider(const Reference& ref)
        : mRef( ref )
    {
    }

    ObjectIdList ReferenceParentProvider::parents(Result& result) const
    {
        GW_CHECK_RESULT( result, ObjectIdList() );

        ObjectId oid = mRef.resolveToObjectId(result);
        if ( !result ) {
            return ObjectIdList();
        }

        return ObjectIdList() << oid;
    }

    Repository ReferenceParentProvider::repository() const
    {
        return mRef.repository();
    }


    //-- ReferenceTreeProvider -->8

    ReferenceTreeProvider::ReferenceTreeProvider(const Git::Reference& ref)
        : mRef( ref )
    {
    }

    Repository ReferenceTreeProvider::repository() const
    {
        return mRef.repository();
    }

    Tree ReferenceTreeProvider::tree(Result& result)
    {
        return mRef.peeled(result, otTree).asTree();
    }

}
