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

#include "libGitWrap/ObjectCommit.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Reference.hpp"
#include "libGitWrap/RefName.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/ObjectPrivate.hpp"
#include "libGitWrap/Private/ReferencePrivate.hpp"

namespace Git
{

    namespace Internal
    {

        ReferencePrivate::ReferencePrivate(RepositoryPrivate* repo, git_reference* ref)
            : RepoObjectPrivate(repo)
            , mRef(ref)
        {
            Q_ASSERT(ref);
        }

        ReferencePrivate::~ReferencePrivate()
        {
            git_reference_free(mRef);
        }

    }

    Reference::Reference()
    {
    }

    Reference::Reference(Internal::ReferencePrivate& _d)
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
     */
    int Reference::compare(const Reference &other) const
    {
        GW_CD(Reference);
        Private* od = Private::dataOf<Reference>(other);
        if (!d) {
            return od ? -1 : 0;
        }

        if (!od) {
            return 1;
        }

        return git_reference_cmp(d->mRef, od->mRef);
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

        Repository::Private* repop = Private::dataOf<Repository>(repo);

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

        return *new Private(repop, ref);
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
                                const QString& name, const ObjectCommit& commit)
    {
        if (!commit.isValid()) {
            return Reference();
        }

        ObjectId sha = commit.id(result);
        return create(result, repo, name, sha);
    }

    /**
     * @brief       This reference's name
     *
     * @return      Returns the reference name (fully qualified)
     */
    QString Reference::name() const
    {
        GW_CD(Reference);

        if (!d) {
            GitWrap::lastResult().setInvalidObject();
            return QString();
        }

        return QString::fromUtf8( git_reference_name( d->mRef ) );
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

        if (!d) {
            GitWrap::lastResult().setInvalidObject();
            return QString();
        }

        return QString::fromUtf8( git_reference_shorthand( d->mRef ) );
    }

    Reference::Type Reference::type( Result& result ) const
    {
        GW_CD_CHECKED(Reference, Invalid, result)

        switch( git_reference_type( d->mRef ) )
        {
        case GIT_REF_SYMBOLIC:  return Symbolic;
        case GIT_REF_OID:       return Direct;
        default:
        case GIT_REF_INVALID:   return Invalid;
        }
    }

    ObjectId Reference::objectId( Result& result ) const
    {
        GW_CD_CHECKED(Reference, ObjectId(), result);

        if (type(result) != Direct)
        {
            return ObjectId();
        }

        return ObjectId::fromRaw( git_reference_target( d->mRef )->id );
    }

    QString Reference::target( Result& result ) const
    {
        GW_CD_CHECKED(Reference, QString(), result)

        if (!type(result) == Symbolic) {
            return QString();
        }

        return QString::fromUtf8( git_reference_symbolic_target( d->mRef ) );
    }

    Reference Reference::resolved( Result& result ) const
    {
        GW_CD_CHECKED(Reference, Reference(), result)

        git_reference* ref;
        result = git_reference_resolve( &ref, d->mRef );
        if( !result )
        {
            return Reference();
        }

        return *new Internal::ReferencePrivate( d->repo(), ref );
    }

    ObjectId Reference::resolveToObjectId( Result& result ) const
    {
        Reference resolvedRef = resolved( result );
        if (!result) {
            return ObjectId();
        }

        return resolvedRef.objectId( result );
    }

    bool Reference::isCurrentBranch() const
    {
        GW_CD(Reference);
        return d && git_branch_is_head( d->mRef );
    }

    bool Reference::isBranch() const
    {
        GW_CD(Reference);
        return d && git_reference_is_branch(d->mRef);
    }

    bool Reference::isLocal() const
    {
        GW_CD(Reference);

        if (!d) {
            GitWrap::lastResult().setInvalidObject();
            return false;
        }

        return git_reference_is_branch( d->mRef );
    }

    bool Reference::isRemote() const
    {
        GW_CD(Reference);

        if (!d) {
            GitWrap::lastResult().setInvalidObject();
            return false;
        }

        return git_reference_is_remote( d->mRef );
    }

    Object Reference::peeled(Result& result, ObjectType ot) const
    {
        GW_CD_CHECKED(Reference, Object(), result);

        git_object* o = NULL;
        result = git_reference_peel(&o, d->mRef, Internal::objectType2gitotype(ot));

        if (!result) {
            return Object();
        }

        return *new Object::Private(d->repo(), o);
    }

    void Reference::checkout(Result &result, bool force, bool updateHEAD,
                             const QStringList &paths) const
    {
        GW_CD_CHECKED_VOID(Reference, result);

        Object o = peeled(result, otTree);
        if (!result) {
            return;
        }

        git_checkout_opts opts = GIT_CHECKOUT_OPTS_INIT;
        opts.checkout_strategy = force ? GIT_CHECKOUT_FORCE : GIT_CHECKOUT_SAFE;
        Internal::StrArray(opts.paths, paths);

        result = git_checkout_tree(d->repo()->mRepo, Private::objectOf(o), &opts);

        if (updateHEAD) {
            this->updateHEAD(result);
        }
    }

    void Reference::destroy(Result& result)
    {
        GW_D_CHECKED_VOID(Reference, result);
        result = git_reference_delete( d->mRef );
    }

    void Reference::move(Result &result, const ObjectCommit &target)
    {
        GW_D_CHECKED_VOID(Reference, result);

        const ObjectId &targetId = target.id(result);
        if ( !result || targetId.isNull() ) return;

        git_reference* newRef = NULL;
        result = git_reference_set_target( &newRef, d->mRef, Internal::ObjectId2git_oid( targetId ) );
        if ( result && (newRef != d->mRef) )
        {
            git_reference_free( d->mRef );
            d->mRef = newRef;
        }
    }

    void Reference::rename(Result &result, const QString &newName, bool force)
    {
        GW_D_CHECKED_VOID(Reference, result);

        git_reference* newRef = NULL;
        result = git_reference_rename( &newRef, d->mRef, newName.toUtf8().constData(), force );
        if ( result && (newRef != d->mRef) )
        {
            git_reference_free( d->mRef );
            d->mRef = newRef;
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
    void Reference::setAsHEAD(Result& result)
    {
        GW_D_CHECKED_VOID(Reference, result);

        if (isBranch()) {
            if (isLocal()) {
                repository().setHEAD(result, name());
            }
            else {
                result.setError("Cannot set remote branch as HEAD.", GIT_ERROR);
            }
        }
        else {
            repository().setHEAD(result, peeled(result, otCommit).asCommit(result));
        }
    }

    void Reference::updateHEAD(Result &result) const
    {
        GW_D_CHECKED_VOID(Reference, result);

        if (git_reference_is_branch(d->mRef)) {
            // reference is a local branch
            result = git_repository_set_head(
                        d->repo()->mRepo,
                        git_reference_name(d->mRef));
        }
        else {
            // reference is detached
            result = git_repository_set_head_detached(
                        d->repo()->mRepo,
                        git_reference_target(d->mRef) );
        }
    }

}
