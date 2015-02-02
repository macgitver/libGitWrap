/*
 * MacGitver
 * Copyright (C) 2012-2013 The MacGitver-Developers <dev@macgitver.org>
 *
 * (C) Sascha Cunz <sascha@macgitver.org>
 * (C) Cunz RaD Ltd.
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

#include <QVector>
#include <QStringBuilder>

#include "libGitWrap/RefName.hpp"
#include "libGitWrap/Reference.hpp"
#include "libGitWrap/TagRef.hpp"
#include "libGitWrap/BranchRef.hpp"
#include "libGitWrap/NoteRef.hpp"

#include "libGitWrap/Private/BasePrivate.hpp"
#include "libGitWrap/Private/RefNamePrivate.hpp"
#include "libGitWrap/Private/ReferencePrivate.hpp"
#include "libGitWrap/Private/TagRefPrivate.hpp"
#include "libGitWrap/Private/BranchRefPrivate.hpp"
#include "libGitWrap/Private/NoteRefPrivate.hpp"
#include "libGitWrap/Private/GitWrapPrivate.hpp"

namespace Git
{

    /**
     * @class       RefName
     * @brief       Analyze a reference's full qualified name
     *
     * References in git follow a complex to describe schemata. This class applies several rules to
     * a reference's fully qualified name (FQRN), in order to figure out its meaning. This results
     * in a set of parameters that can be used to qualify the reference using only the 'type' and the
     * parameters.
     *
     * For this purpose, we create the following definitions and logic to test for them:
     *
     * -   _Specials_
     *
     *     A special reference is a reference with a very special meaning inside git. We will detect:
     *     `MERGE_HEAD`, `HEAD` (wherever it occurs), `refs/stage` and the `refs/notes/commit` as
     *     such a special reference. In this case, isSpecial() will return `true`. Each of these
     *     references also has a getter method on its own: isMergeHead(), isHead(), isStage() and
     *     isCommitNote(). With the exception of `HEAD` unless it is the fully qualified reference
     *     name, further detection stops if we detect a special reference.
     *
     *     For consistency reasons, if isHead() returns `true`, isBranch() will also return `true`
     *     and name() will return `HEAD`.
     *
     * -   _Custom_
     *
     *     Any regular expression can be registered with the name matcher using the
     *     registerExpression() method. If a reference name matches that expression, isCustom() will
     *     return `true` and customExpression() will return the `void*` given when the expression
     *     was registered. No further matching is performed. This mechanism can be used to match
     *     situation dependant special references, like GitHub's special references for the head of
     *     a pull request, which follow the regular expression:
     *
     *         ^refs\/pull\/([1-9][0-9]*)\/head$
     *
     * -   _Namespaced_
     *
     *     A reference is namespaced if the regular expression:
     *
     *         ^refs\/namespaces\/([^\/]+)\/(.+)$
     *
     *     maches at least once. If the regexp matches, `\1` will be appended to the namespaces
     *     array and the reference is replaced with `\2` and the namespace test executed again. In
     *     this case isNamespaced() will return `true`, namespaces() will return the namespace array
     *     and namespaceName() will return a string formed by joining all members of the namespace
     *     array with `/`.
     *
     * -   _Remote_
     *
     *     A reference is remote, if its FQRN starts with the `refs/remote` segments. In this case,
     *     isRemote() will return `true` and remote() will return the 3rd segment of the FQRN. The
     *     reference must match against the regular expression:
     *
     *         ^refs\/remotes\/([^\/]+)\/(.+)$
     *
     *     If it matches, remote() is set to `\1` and `\2` is used to perform the _Remote-Branch_
     *     test.
     *
     * -   _Remote Branch_
     *
     *     A reference is a remote branch, if isRemote() returns `true` and `\2` from the _Remote_
     *     case does not evaluate to `HEAD`. In this case, the Test _Scoped_ is applied to `\2`.
     *     isBranch() will return `true`. If `\2` is `HEAD`, the reference specifies the remote's
     *     default-branch-to-checkout, thus, isBranch() will return `false` but isHead() will return
     *     `true`.
     *
     * -   _Local Branch_
     *
     *     A reference is a local branch, if it starts with `refs/heads/` and the _Remote_ test did
     *     not match. In this case, isBranch() will return `true` and the _Scoped_ test will be
     *     applied using the remainder of the reference name as input.
     *
     * -   _Tag_
     *
     *     A reference is a tag, if it starts with `refs/tags/` and the _Remote_ test did not match.
     *     In this case, isTag() will return `true` and the _Scoped_ test will be applied using the
     *     remainder of the reference name as input.
     *
     * -   _Scoped_
     *
     *     The remainder as given by the _Remote_, _Local Branch_ or _Tag_ tests, is scoped, if it
     *     matches the regular expression:
     *
     *         ^([^\/]+)\/(.+)$
     *
     *     at least once. In this case, `\1` will be appended to the scopes array and the _Scoped_
     *     test will be repeated with `\2`. If the regular expression does not match a single time,
     *     name() will be set to the input of the regular expression. Thus, name() will be the whole
     *     input or the string after the last `/`. If the regular expression matched at least once,
     *     isScoped() will return `true`, scopes() will return the scope array and scopeName() will
     *     return a string formed by joining all members of the scope array with `/`.
     *
     * -   _Salvatory rule_
     *
     *     If none of the above rules and tests matched the reference, we call it _peculiar_. In
     *     that case, isPeculiar() will return `true`.
     *
     * -   Additional stuff
     *
     *     -   fullName() will always return the fully qualified reference name as it was set in
     *         the constructor.
     *
     *     -   If either isBranch() or isTag() returns `true`, localName() will return name() if
     *         isScoped() returns `false` and scopeName() + '/' + name() otherwise.
     *
     *     -   scopePrefix() will return the beginning of the fully qualified reference name up to
     *         the point where the localName() starts. If scopeName() is empty, scopePrefix() will
     *         return the fully qualified reference name.
     *
     *         _Note_ that this is the only method that might return a trailing slash (`/`).
     *         scopePrefix() + localName() is always equal to the fullName().
     *
     *     -   If isTag() returns `true`, then tagName() will return localName(). It will return an
     *         empty string if isTag() returns `false`.
     *
     *     -   If isBranch() returns `true`, then branchName() will return localName(). It will
     *         return an empty string if isBranch() returns `false`.
     *
     * Examples:
     *
     * -   `refs/heads/master` will result in:
     *
     *     -   `isBranch() == true`
     *     -   `name() == "master"`
     *     -   `scopeName() == ""`
     *     -   `namespaceName() == ""`
     *     -   `branchName() == "master"`
     *     -   `localName() == "master"`
     *     -   `scopePrefix() == "refs/heads/"`
     *
     * -   `refs/remotes/origin/feature/cool` will result in:
     *
     *     -   `isRemote() == true`
     *     -   `isBranch() == true`
     *     -   `isScoped() == true`
     *     -   `name() == "cool"`
     *     -   `scopeName() == "feature"`
     *     -   `namespaceName() == ""`
     *     -   `branchName() == "feature/cool"`
     *     -   `localName() == "feature/cool"`
     *     -   `scopePrefix() == "refs/remotes/origin/"`
     *
     * -   `refs/namespaces/foo/refs/namespaces/bar/refs/tags/releases/1.0` will result in:
     *
     *     -   `isNamespaced() == true`
     *     -   `isTag() == true`
     *     -   `isScoped() == true`
     *     -   `name() == "1.0"`
     *     -   `scopeName() == "releases"`
     *     -   `namespaceName() == "foo/bar"`
     *     -   `tagName() == "releases/1.0"`
     *     -   `localName() == "releases/1.0"`
     *     -   `scopePrefix() == "refs/namespaces/foo/refs/namespaces/bar/refs/tags/"`
     *
     * -   `HEAD` will result in:
     *
     *     -   `isSpecial() == true`
     *     -   `isHead() == true`
     *     -   `isBranch() == true`
     *     -   `scopeName() == ""`
     *     -   `name() == "HEAD"`
     *     -   `localName() == "HEAD"`
     *     -   `scopePrefix() == "HEAD"`
     *
     */

    namespace Internal
    {

        RefNameMatches* RefNameMatches::sSelf = NULL;

        RefNameMatches& RefNameMatches::self()
        {
            if (!sSelf) {
                sSelf = new RefNameMatches;
            }
            return *sSelf;
        }

        RefNameMatches::RefNameMatches()
            : reNamespaces(QLatin1String("^refs\\/namespaces\\/([^\\/]+)\\/(.+)$"))
            , reRemote(QLatin1String("^refs\\/remotes\\/([^\\/]+)\\/(.+)$"))
            , reScopes(QLatin1String("^([^\\/]+)\\/(.+)$"))
            , nextId(0)
        {
        }

        // -- RefNamePrivate -------------------------------------------------------------------- >8

        RefNamePrivate::RefNamePrivate()
            : RepoObjectPrivate()
            , isAnalyzed(false)
        {
        }

        RefNamePrivate::RefNamePrivate(const RefNamePrivate* refName)
            : RepoObjectPrivate(refName->repo())
            , isAnalyzed(refName->isAnalyzed)
            , isStage(refName->isStage)
            , isBranch(refName->isBranch)
            , isTag(refName->isTag)
            , isNote(refName->isNote)
            , isHead(refName->isHead)
            , isMergeHead(refName->isMergeHead)
            , isCommitNote(refName->isCommitNote)
            , isPecuiliar(refName->isPecuiliar)
            , fqrn(refName->fqrn)
            , remote(refName->remote)
            , name(refName->name)
            , scopes(refName->scopes)
            , namespaces(refName->namespaces)
            , customMatches(refName->customMatches)
        {
        }

        RefNamePrivate::RefNamePrivate(RepositoryPrivate* repo)
            : RepoObjectPrivate(repo)
            , isAnalyzed(false)
        {
        }

        RefNamePrivate::RefNamePrivate(RepositoryPrivate* repo, const QString& name)
            : RepoObjectPrivate(repo)
            , isAnalyzed(false)
            , fqrn(name)
        {
        }

        bool RefNamePrivate::isRealReference() const
        {
            return false;
        }

        void RefNamePrivate::ensureAnalyzed()
        {
            if (isAnalyzed) {
                return;
            }

            analyze();
        }

        bool RefNamePrivate::analyzeCustom()
        {
            for (int i = 0; i < RefNameMatches::self().customMatches.count(); i++) {
                const CustomMatches& cm = RefNameMatches::self().customMatches[i];
                if (fqrn.indexOf(cm.regExp) != -1) {
                    customMatches.append(cm.id);
                }
            }
            return customMatches.count() > 0;
        }

        void RefNamePrivate::scopeTest(QString sub)
        {
            const QRegExp& reScp = RefNameMatches::self().reScopes;

            while (reScp.indexIn(sub) != -1) {
                scopes.append(reScp.cap(1));
                sub = reScp.cap(2);
            }

            name = sub;
        }

        void RefNamePrivate::analyze()
        {
            remote = name = QString();
            scopes = namespaces = QStringList();
            isNote = isStage = isBranch = isTag = isHead =
            isMergeHead = isCommitNote = isPecuiliar = false;
            customMatches.clear();

            if (fqrn == QLatin1String("refs/stage")) {
                isStage = true;
            }
            else if (fqrn == QLatin1String("MERGE_HEAD")) {
                isMergeHead = true;
            }
            else if (fqrn == QLatin1String("HEAD")) {
                name = fqrn;
                isBranch = true;
                isHead = true;
            }
            else {
                const QRegExp& reNS = RefNameMatches::self().reNamespaces;
                const QRegExp& reRem = RefNameMatches::self().reRemote;

                QString match = fqrn;

                analyzeCustom();

                while (reNS.indexIn(match) != -1) {
                    namespaces.append(reNS.cap(1));
                    match = reNS.cap(2);
                }

                if (reRem.exactMatch(match)) {
                    remote = reRem.cap(1);
                    match = reRem.cap(2);

                    isHead = (match == QLatin1String("HEAD"));
                    isBranch = true;

                    scopeTest(match);
                }
                else if (match.startsWith(QLatin1String("refs/notes/"))) {
                    isNote = true;
                    isCommitNote = (match == QLatin1String("refs/notes/commit"));
                    scopeTest(match.mid(11));
                }
                else if (match.startsWith(QLatin1String("refs/heads/"))) {
                    isBranch = true;
                    scopeTest(match.mid(11));
                }
                else if (match.startsWith(QLatin1String("refs/tags/"))) {
                    isTag = true;
                    scopeTest(match.mid(10));
                }
                else {
                    isPecuiliar = customMatches.count() == 0;
                }
            }

            isAnalyzed = true;
        }

        ReferencePrivate* RefNamePrivate::cloned(git_reference* lgo)
        {
            ensureAnalyzed();

            if (isBranch) {
                return cloneAs<BranchRef>(lgo);
            }

            if (isTag) {
                return cloneAs<TagRef>(lgo);
            }

            if (isNote) {
                return cloneAs<NoteRef>(lgo);
            }

            return cloneAs<Reference>(lgo);
        }

        /**
         * @internal
         * @brief       Factory to create the correct ReferencePrivate
         *
         * @param[in]   repo    The repository for which to create the reference object.
         *
         * @param[in]   name    A fully qualified reference name.
         *
         * @param[in]   lgo     The libgit2 git_reference object. This is optional and defaults to
         *                      `NULL`. If given, the new ReferencePrivate will point to it. If not
         *                      given, we have to look it up first.
         *
         * @return      A new ReferencePrivate object capable of holding a reference type that
         *              matches @a name.
         *
         * This works relatively efficient and reliable: We create a RefNamePrivate (on the stack,
         * not the heap) and let it clone itself. Before cloning, it has to analyze itself in order
         * to decide what reference private object to create.
         *
         * The cloning mechanism will then move over all the (already) analyzed data from the stack
         * based RefNamePrivate.
         *
         * If no @a lgo is given and it cannot be looked up in @a repo under the @a name, `NULL`
         * will be returned, as the resulting ReferencePrivate would be invalid and we cannot
         * express that state in the private object.
         *
         */
        ReferencePrivate* RefNamePrivate::createRefObject(Result& result, Repository::Private* repo,
                                                          const QString& name, git_reference* lgo)
        {
            GW_CHECK_RESULT( result, NULL );
            Q_ASSERT( repo );

            if ( !lgo ) {
                result = git_reference_lookup(&lgo, repo->mRepo, GW_StringFromQt(name));
                GW_CHECK_RESULT( result, NULL );
            }

            return RefNamePrivate(repo, name).cloned(lgo);
        }

    }

    GW_PRIVATE_IMPL(RefName, RepoObject)

    /**
     * @brief       Create a RefName (without Repository association)
     *
     * @param[in]   refName     Fully qualified reference name to analyze.
     *
     * Some kind of analyzations can only be performed when a repository is given.
     *
     */
    RefName::RefName(const QString& refName)
        : RepoObject(new Private)
    {
        GW_D(RefName);
        d->fqrn = refName;
    }

    /**
     * @brief       Create a RefName (with Repository association)
     *
     * @param[in]   refName     Fully qualified reference name to analyze.
     *
     * Some kind of analyzations can only be performed when a repository is given.
     *
     */
    RefName::RefName(const Repository& repo, const QString& refName)
        : RepoObject(new Private(Private::dataOf<Repository>(repo),refName))
    {
    }

    /**
     * @brief       Is this a remote reference?
     *
     * @return      `true`, if this is a remote reference.
     *
     */
    bool RefName::isRemote()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), !d->remote.isEmpty() : false;
    }

    /**
     * @brief       Is this a tag?
     *
     * @return      `true`, if this is a tag
     *
     */
    bool RefName::isTag()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), d->isTag : false;
    }

    /**
     * @brief       Is this a note?
     *
     * @return      `true`, if this is a note
     *
     */
    bool RefName::isNote()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), d->isNote : false;
    }

    /**
     * @brief       Is this a branch?
     *
     * @return      `true`, if this is a branch.
     *
     */
    bool RefName::isBranch()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), d->isBranch : false;
    }

    bool RefName::isStage()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), d->isStage : false;
    }

    bool RefName::isHead()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), d->isHead : false;
    }

    bool RefName::isMergeHead()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), d->isMergeHead : false;
    }

    bool RefName::isCommitNote()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), d->isCommitNote : false;
    }

    bool RefName::isSpecial()
    {
        return isHead() || isMergeHead() || isCommitNote() || isStage();
    }

    bool RefName::isPeculiar()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), d->isPecuiliar : false;
    }

    bool RefName::isScoped()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), (d->scopes.count() > 0) : false;
    }

    bool RefName::isNamespaced()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), (d->namespaces.count() > 0) : false;
    }

    bool RefName::isCustom()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), (d->customMatches.count() > 0) : false;
    }

    QStringList RefName::namespaces()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), d->namespaces : QStringList();
    }

    QStringList RefName::scopes()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), d->scopes : QStringList();
    }

    QString RefName::remote()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), d->remote : QString();
    }

    QString RefName::name()
    {
        GW_D(RefName);
        return d ? d->ensureAnalyzed(), d->name : QString();
    }

    /**
     * @brief       Get the full qualified reference name
     *
     * @return      The fully qualified reference name as set in the constructor.
     *
     */
    QString RefName::fullName()
    {
        GW_D(RefName);
        return d ? d->fqrn : QString();
    }

    /**
     * @brief       Get the local name of the reference
     *
     * @return      If the reference is either a tag, a note, a branch or HEAD, the scopeName() and
     *              name() are joined via a `/` and the result is returned.
     *
     */
    QString RefName::localName()
    {
        if (isBranch() || isTag() || isNote()) {
            return isScoped() ? scopeName() % QChar(L'/') % name() : name();
        }
        return QString();
    }

    /**
     * @brief       Get the prefix that has to be prepended to the scopes
     *
     * @return      Returns the beginning part of the full qualified reference name up to the point
     *              where the scope begins. This might include a trailing slash (`/`).
     *
     * If scopeName() is empty, this method returns the full qualified reference name.
     *
     */
    QString RefName::scopePrefix()
    {
        GW_D(RefName);
        if (d) {
            int l = scopeName().length();
            return d->fqrn.left(d->fqrn.count() - l);
        }
        return QString();
    }

    /**
     * @brief       Get the name of the branch if this reference is a branch
     *
     * @return      If isBranch() returns `true` then return localName() else an empty string.
     *
     * Works for local and remote branches and also for special references where isHead() returns
     * `true`.
     *
     */
    QString RefName::branchName()
    {
        return isBranch() ? localName() : QString();
    }

    /**
     * @brief       Get the name of the tag if this reference is a tag.
     *
     * @return      If isTag() returns `true` then return localName() else an empty string.
     *
     */
    QString RefName::tagName()
    {
        return isTag() ? localName() : QString();
    }

    /**
     * @brief       Get the name of the note if this reference is a note.
     *
     * @return      If isNote() returns `true` then return localName() else an empty string.
     *
     */
    QString RefName::noteName()
    {
        return isNote() ? localName() : QString();
    }

    /**
     * @brief       Get the joined namespaces
     *
     * @return      a QString containing all namespaces() joined with a `/`
     *
     * Note that this is not a part of the reference name. A nested namespace that would return
     * `foo/bar` from this method actually begins with a reference name like
     * `refs/namespaces/foo/refs/namespaces/bar`.
     *
     */
    QString RefName::namespaceName()
    {
        return namespaces().join(QChar(L'/'));
    }

    /**
     * @brief       Get the joined scopes
     *
     * @return      a QString containing all scopes() joined with a `/`
     *
     */
    QString RefName::scopeName()
    {
        return scopes().join(QChar(L'/'));
    }

    /**
     * @brief       Did the name match a registered custom expression
     *
     * @param[in]   id      The id of the expression as returned by registerExpression().
     *
     * @return      `true` or `false` telling whether the given expression matched or not.
     *
     * Note that all custom expressions are matched at once.
     *
     */
    bool RefName::matchesCustomRule(int id)
    {
        GW_D(RefName);
        if (!d) {
            return false;
        }

        d->ensureAnalyzed();

        return d->customMatches.contains(id);
    }

    /**
     * @brief       Register a custom expression to match against
     *
     * @param[in]   payload A user defined value that is not used internally but yet associated with
     *                      the expression
     *
     * @param[in]   regExp  A regular expression used to match against the full qualified reference
     *                      name. If it matches, further processing is stopped and the match
     *                      recorded. In this case, isCustom() will return `true` and
     *                      matchesCustomRule() with the return value of this method will return
     *                      `true`, too.
     *
     * @return      A unique id that can be used to refer to this registered expression.
     *
     */
    int RefName::registerExpression(void* payload, const QRegExp& regExp)
    {
        Internal::CustomMatches cm(regExp, Internal::RefNameMatches::self().nextId++, payload);
        Internal::RefNameMatches::self().customMatches.append(cm);
        return cm.id;
    }

    /**
     * @brief       Unregister a custom expression
     *
     * @param[in]   id      The id of the expression as returned by registerExpression().
     *
     */
    void RefName::unregisterExpression(int id)
    {
        for (int i = 0; i < Internal::RefNameMatches::self().customMatches.count(); i++) {
            if (Internal::RefNameMatches::self().customMatches[i].id == id) {
                Internal::RefNameMatches::self().customMatches.remove(id);
                return;
            }
        }
    }

    /**
     * @brief       Find data for a registered custom expression
     *
     * @param[in]   id      The id of the expression as returned by registerExpression().
     *
     * @return      Either `NULL` if @a id is not a valid expression or the payload that was
     *              registered with the expression.
     *
     */
    void* RefName::expressionData(int id)
    {
        for (int i = 0; i < Internal::RefNameMatches::self().customMatches.count(); i++) {
            if (Internal::RefNameMatches::self().customMatches[i].id == id) {
                return Internal::RefNameMatches::self().customMatches[i].payload;
            }
        }

        return NULL;
    }

    /**
     * @brief       Get the 'shorthand' name for the reference
     *
     * A short hand name for a reference is a fully qualified reference name with the line noise
     * removed. This works for all branches and tags, even remote ones.
     *
     * It also works for HEAD. The short hand name for the local current branch is `HEAD` and the
     * short hand name for the remote `farfarawawy`'s default branch is just `farfaraway` (the FQRN
     * would be `refs/remotes/farfaraway/HEAD`).
     *
     * Note that this deliberately doesn't work with `refs/notes/` references.
     *
     * @return      The short hand name for the reference. The short hand name contains the segments
     *              consisting of remote() if present, all the scopes() and finally name().
     *
     * @see         `man git-rev-parse`
     */
    QString RefName::shorthand()
    {
        QStringList segments;

        if (isNamespaced()) {
            return QString();
        }

        if (isHead()) {

            if (isRemote()) {
                return remote();
            }

            return QLatin1String("HEAD");
        }

        if (isBranch() || isTag()) {

            if (isRemote()) {
                segments << remote();
            }

            if (isScoped()) {
                segments << scopes();
            }

            segments << name();

        }

        return segments.join(QChar(L'/'));
    }

    /**
     * @brief       Get the Reference this RefName was created from
     *
     * @return      If this RefName object was created from a Reference, return that reference
     *              otherwise an invalid Reference object.
     */
    Reference RefName::reference() const
    {
        GW_CD_EX(Reference);

        if (d && d->isRealReference()) {
            return Reference(d);
        }

        return Reference();
    }

}
