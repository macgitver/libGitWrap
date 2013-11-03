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

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/ObjectPrivate.hpp"
#include "libGitWrap/Private/ReferencePrivate.hpp"

#define CHECK_DELETED(returns, result) \
    if(d->wasDeleted) { \
        result.setError("Tried to access a destroyed reference.", GIT_ERROR); \
        return returns; \
    }

#define CHECK_DELETED_VOID(result) \
    if(d->wasDeleted) { \
        result.setError("Tried to access a destroyed reference.", GIT_ERROR); \
        return; \
    }

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

        ReferencePrivate::ReferencePrivate(const RepositoryPrivate::Ptr& repo, git_reference* ref)
            : RepoObjectPrivate(repo)
            , wasDeleted(false)
            , reference(ref)
        {
            Q_ASSERT(reference);
        }

        ReferencePrivate::~ReferencePrivate()
        {
            // We have to free the reference, no matter whether it was deleted or not.
            git_reference_free(reference);
        }

    }

    Reference::Reference()
    {
    }

    Reference::Reference(const PrivatePtr& _d)
        : RepoObject(_d)
    {
    }

    Reference::Reference( const Reference& other )
        : RepoObject(other)
    {
    }

    Reference::~Reference()
    {
    }


    Reference& Reference::operator=( const Reference& other )
    {
        RepoObject::operator=(other);
        return * this;
    }

    bool Reference::operator==(const Reference &other) const
    {
        return RepoObject::operator==(other);
    }

    bool Reference::operator!=(const Reference &other) const
    {
        return !( *this == other );
    }

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
     * Creates a new reference named @a name that is pointing to the SHA1 @a sha in the repository
     * @a repo.
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
     * @return          The created reference object or an invalid reference object, if the an error
     *                  occured while creating the reference.
     *
     * @note    This method does not test whether the repository really has an object with the id
     *          @a sha.
     *
     */
    Reference Reference::create(Result& result, Repository repo,
                                const QString& name, const ObjectId& sha)
    {
        if (!result) {
            return Reference();
        }

        if (!repo.isValid()) {
            result.setInvalidObject();
            return Reference();
        }

        Repository::PrivatePtr repop(Private::dataOf<Repository>(repo));

        git_reference* ref = NULL;
        QByteArray utf8Name = name.toUtf8();

        result = git_reference_create(
                    &ref,
                    repop->mRepo,
                    utf8Name.constData(),
                    Private::sha(sha),
                    0);

        if (!result) {
            return Reference();
        }

        return PrivatePtr(new Private(repop, ref));
    }


    /**
     * @brief           Create a new reference
     *
     * Creates a new reference named @a name that is pointing to the commit @a commit in the
     * repository @a repo.
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
     * @return          The created reference object or an invalid reference object, if the an error
     *                  occured while creating the reference.
     *
     */
    Reference Reference::create(Result& result, Repository repo,
                                const QString& name, const Commit& commit)
    {
        if (!commit.isValid()) {
            result.setInvalidObject();
            return Reference();
        }
        return create(result, repo, name, commit.id());
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
            GitWrap::lastResult().setInvalidObject();
            return QString();
        }

        return QString::fromUtf8(git_reference_name(d->reference));
    }

    /**
     * @brief       Reference analyzer for this reference
     *
     * @return      A new RefName object for this reference's name.
     *
     */
    RefName Reference::nameAnalyzer() const
    {
        return RefName(name());
    }

    /**
     * @brief   Returns the name prefix for the reference.
     *
     * @return  the name prefix or an empty string
     */
    QString Reference::prefix() const
    {
        const QString tmpName = name();
        return tmpName.left( tmpName.length() - shorthand().length() );
    }

    QString Reference::shorthand() const
    {
        GW_CD(Reference);

        if (!d || d->wasDeleted) {
            GitWrap::lastResult().setInvalidObject();
            return QString();
        }

        return QString::fromUtf8(git_reference_shorthand(d->reference));
    }

    Reference::Type Reference::type( Result& result ) const
    {
        GW_CD_CHECKED(Reference, Invalid, result);
        CHECK_DELETED(Invalid, result);

        switch (git_reference_type(d->reference)) {
        default:
        case GIT_REF_INVALID:   return Invalid;
        case GIT_REF_SYMBOLIC:  return Symbolic;
        case GIT_REF_OID:       return Direct;
        }
    }

    ObjectId Reference::objectId( Result& result ) const
    {
        GW_CD_CHECKED(Reference, ObjectId(), result);
        CHECK_DELETED(ObjectId(), result);

        if (type(result) != Direct)
        {
            return ObjectId();
        }

        return ObjectId::fromRaw(git_reference_target(d->reference)->id);
    }

    QString Reference::target( Result& result ) const
    {
        GW_CD_CHECKED(Reference, QString(), result);
        CHECK_DELETED(QString(), result);

        if (!type(result) == Symbolic) {
            return QString();
        }

        return QString::fromUtf8(git_reference_symbolic_target(d->reference));
    }

    Reference Reference::resolved( Result& result ) const
    {
        GW_CD_CHECKED(Reference, Reference(), result);
        CHECK_DELETED(Reference(), result);

        git_reference* ref;
        result = git_reference_resolve(&ref, d->reference);
        if (!result) {
            return Reference();
        }

        return PrivatePtr(new Private(d->repo(), ref));
    }

    ObjectId Reference::resolveToObjectId( Result& result ) const
    {
        Reference resolvedRef = resolved(result);
        if (!result) {
            return ObjectId();
        }

        return resolvedRef.objectId( result );
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
            GitWrap::lastResult().setInvalidObject();
            return false;
        }

        return git_reference_is_branch(d->reference);
    }

    bool Reference::isRemote() const
    {
        GW_CD(Reference);

        if (!d || d->wasDeleted) {
            GitWrap::lastResult().setInvalidObject();
            return false;
        }

        return git_reference_is_remote(d->reference);
    }

    Object Reference::peeled(Result& result, ObjectType ot) const
    {
        GW_CD_CHECKED(Reference, Object(), result);
        CHECK_DELETED(Object(), result);

        git_object* o = NULL;
        result = git_reference_peel(&o, d->reference, Internal::objectType2gitotype(ot));

        if (!result) {
            return Object();
        }

        return Object::Private::create(d->repo(), o);
    }

    void Reference::checkout(Result &result,
                             CheckoutOptions opts,
                             const QStringList &paths) const
    {
        GW_CD_CHECKED_VOID(Reference, result);
        CHECK_DELETED_VOID(result);

        peeled<Tree>(result).checkout(result, opts.testFlag(CheckoutForce), paths);

        if (!result) {
            return;
        }

        if (opts.testFlag(CheckoutUpdateHEAD)) {
            setAsHEAD(result);
        }
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
        GW_D_CHECKED_VOID(Reference, result);
        CHECK_DELETED_VOID(result);

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
        GW_D_CHECKED_VOID(Reference, result);
        CHECK_DELETED_VOID(result);

        ObjectId targetId = target.id();
        if (targetId.isNull()) {
            return;
        }

        git_reference* newRef = NULL;
        result = git_reference_set_target(&newRef, d->reference, Private::sha(targetId));

        if (result && (newRef != d->reference)) {
            git_reference_free(d->reference);
            d->reference = newRef;
        }
    }

    void Reference::rename(Result &result, const QString &newName, bool force)
    {
        GW_D_CHECKED_VOID(Reference, result);
        CHECK_DELETED_VOID(result);

        git_reference* newRef = NULL;
        result = git_reference_rename(&newRef, d->reference, newName.toUtf8().constData(), force);

        if (result && (newRef != d->reference)) {
            git_reference_free(d->reference);
            d->reference = newRef;
        }
    }

    /**
     * @brief       Point the repository's HEAD here
     *
     * If this reference is a local branch, make HEAD a symbolic reference to the branch. If this
     * reference is a remote branch, fail. In all other cases, set HEAD detached to where this
     * reference points to.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     */
    void Reference::setAsHEAD(Result& result) const
    {
        GW_CD_CHECKED_VOID(Reference, result);
        CHECK_DELETED_VOID(result);

        if (isBranch()) {
            if (isLocal()) {
                repository().setHEAD(result, name());
            }
            else {
                result.setError("Cannot set remote branch as HEAD.", GIT_ERROR);
            }
        }
        else {
            peeled<Commit>(result).setAsHEAD(result);
        }
    }

    void Reference::updateHEAD(Result &result) const
    {
        GW_CD_CHECKED_VOID(Reference, result);
        CHECK_DELETED_VOID(result);

        if (git_reference_is_branch(d->reference)) {
            // reference is a local branch
            result = git_repository_set_head(
                        d->repo()->mRepo,
                        git_reference_name(d->reference));
        }
        else {
            // reference is detached
            result = git_repository_set_head_detached(
                        d->repo()->mRepo,
                        git_reference_target(d->reference));
        }
    }

}
