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

#include "GitWrapPrivate.hpp"

#include "RefName.hpp"
#include "BasicObject.hpp"

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
     *     -   If isTag() returns `true`, then tagName() will return scopeName() + '/' + name() if
     *         isScoped() is `true` and will simply return name() if isScoped() is `false`. It will
     *         return an empty string if isTag() returns `false`.
     *
     *     -   If isBranch() returns `true`, then branchName() will return
     *         scopeName() + '/' + name() if isScoped() is `true` and will simply return name() if
     *         isScoped() is `false`. It will return an empty string if isBranch() returns `false`.
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
     */

    namespace Internal
    {

        struct CustomMatches
        {
        public:
            CustomMatches()
                : id(-1)
                , payload(NULL)
            {
            }/*

            CustomMatches(const CustomMatches& o)
                : id(o.id)
                , regExp(o.regExp)
                , payload(o.payload)
            {
            }*/

            CustomMatches(const QRegExp& _re, int _id, void* _payload)
                : id(_id)
                , regExp(_re)
                , payload(_payload)
            {
            }

        public:
            int                 id;
            /*const*/ QRegExp   regExp;
            void*               payload;
        };

        class RefNameMatches
        {
        public:
            static RefNameMatches& self();

        public:
            const QRegExp           reNamespaces;
            const QRegExp           reRemote;
            const QRegExp           reScopes;
            QVector<CustomMatches>  customMatches;
            int                     nextId;

        private:
            static RefNameMatches* sSelf;
            RefNameMatches();
        };

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

        class RefNamePrivate : public BasicObject
        {
        public:
            bool isAnalyzed     : 1;

            bool isStage        : 1;
            bool isBranch       : 1;
            bool isTag          : 1;
            bool isHead         : 1;
            bool isMergeHead    : 1;
            bool isCommitNote   : 1;
            bool isPecuiliar    : 1;

            QString fqrn;

            QString remote;
            QString name;
            QStringList scopes;
            QStringList namespaces;
            QVector<int> customMatches;

        public:
            void ensureAnalyzed();
            void analyze();

        private:
            void scopeTest(QString sub);
            bool analyzeCustom();
        };

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
            isStage = isBranch = isTag = isHead = isMergeHead = isCommitNote = isPecuiliar = false;
            customMatches.clear();

            if (fqrn == QLatin1String("refs/stage")) {
                isStage = true;
            }
            else if (fqrn == QLatin1String("refs/notes/commit")) {
                isCommitNote = true;
            }
            else if (fqrn == QLatin1String("MERGE_HEAD")) {
                isMergeHead = true;
            }
            else if (fqrn == QLatin1String("HEAD")) {
                isHead = true;
            }
            else if (!analyzeCustom()) {
                const QRegExp& reNS = RefNameMatches::self().reNamespaces;
                const QRegExp& reRem = RefNameMatches::self().reRemote;

                QString match = fqrn;

                while (reNS.indexIn(match) != -1) {
                    namespaces.append(reNS.cap(1));
                    match = reNS.cap(2);
                }

                if (reRem.exactMatch(match)) {
                    remote = reRem.cap(1);
                    match = reRem.cap(2);

                    isHead = (match == QLatin1String("HEAD"));
                    isBranch = !isHead;

                    scopeTest(match);
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
                    isPecuiliar = true;
                }
            }

            isAnalyzed = true;
        }

    }

    RefName::RefName()
    {
    }

    RefName::RefName(const RefName& other)
        : d(other.d)
    {
    }

    RefName::RefName(const QString& refName)
    {
        d = new Internal::RefNamePrivate;
        d->fqrn = refName;
        d->isAnalyzed = false;
    }

    RefName::~RefName()
    {
    }

    RefName& RefName::operator=(const RefName& other)
    {
        d = other.d;
        return *this;
    }

    bool RefName::isValid() const
    {
        return d;
    }

    /**
     * @brief       Is this a remote reference?
     * @return      `true`, if this is a remote reference.
     */
    bool RefName::isRemote()
    {
        return d ? d->ensureAnalyzed(), !d->remote.isEmpty() : false;
    }

    /**
     * @brief       Is this a tag?
     * @return      `true`, if this is a tag
     */
    bool RefName::isTag()
    {
        return d ? d->ensureAnalyzed(), d->isTag : false;
    }

    /**
     * @brief       Is this a branch?
     * @return      `true`, if this is a branch.
     */
    bool RefName::isBranch()
    {
        return d ? d->ensureAnalyzed(), d->isBranch : false;
    }

    bool RefName::isStage()
    {
        return d ? d->ensureAnalyzed(), d->isStage : false;
    }

    bool RefName::isHead()
    {
        return d ? d->ensureAnalyzed(), d->isHead : false;
    }

    bool RefName::isMergeHead()
    {
        return d ? d->ensureAnalyzed(), d->isMergeHead : false;
    }

    bool RefName::isCommitNote()
    {
        return d ? d->ensureAnalyzed(), d->isCommitNote : false;
    }

    bool RefName::isSpecial()
    {
        return isHead() || isMergeHead() || isCommitNote() || isStage();
    }

    bool RefName::isPeculiar()
    {
        return d ? d->ensureAnalyzed(), d->isPecuiliar : false;
    }

    bool RefName::isScoped()
    {
        return d ? d->ensureAnalyzed(), (d->scopes.count() > 0) : false;
    }

    bool RefName::isNamespaced()
    {
        return d ? d->ensureAnalyzed(), (d->namespaces.count() > 0) : false;
    }

    bool RefName::isCustom()
    {
        return d ? d->ensureAnalyzed(), (d->customMatches.count() > 0) : false;
    }

    QStringList RefName::namespaces()
    {
        return d ? d->ensureAnalyzed(), d->namespaces : QStringList();
    }

    QStringList RefName::scopes()
    {
        return d ? d->ensureAnalyzed(), d->scopes : QStringList();
    }

    QString RefName::remote()
    {
        return d ? d->ensureAnalyzed(), d->remote : QString();
    }

    QString RefName::name()
    {
        return d ? d->ensureAnalyzed(), d->name : QString();
    }

    QString RefName::fullName()
    {
        return d ? d->fqrn : QString();
    }

    QString RefName::localName()
    {
        if (isBranch() || isTag()) {
            return isScoped() ? scopeName() % QChar(L'/') % name() : name();
        }
        return QString();
    }

    QString RefName::scopePrefix()
    {
        if (d) {
            int l = scopeName().length();
            return d->fqrn.left(d->fqrn.count() - l);
        }
        return QString();
    }

    QString RefName::branchName()
    {
        return isBranch() ? localName() : QString();
    }

    QString RefName::tagName()
    {
        return isTag() ? localName() : QString();
    }

    QString RefName::namespaceName()
    {
        return namespaces().join(QChar(L'/'));
    }

    QString RefName::scopeName()
    {
        return scopes().join(QChar(L'/'));
    }


    bool RefName::matchesCustomRule(int id)
    {
        if (!d) {
            return false;
        }

        d->ensureAnalyzed();

        return d->customMatches.contains(id);
    }

    int RefName::registerExpression(void* data, const QRegExp& regExp)
    {
        Internal::CustomMatches cm(regExp, Internal::RefNameMatches::self().nextId++, data);
        Internal::RefNameMatches::self().customMatches.append(cm);
        return cm.id;
    }

    void RefName::unregisterExpression(int id)
    {
        for (int i = 0; i < Internal::RefNameMatches::self().customMatches.count(); i++) {
            if (Internal::RefNameMatches::self().customMatches[i].id == id) {
                Internal::RefNameMatches::self().customMatches.remove(id);
                return;
            }
        }
    }

    void* RefName::expressionData(int id)
    {
        for (int i = 0; i < Internal::RefNameMatches::self().customMatches.count(); i++) {
            if (Internal::RefNameMatches::self().customMatches[i].id == id) {
                return Internal::RefNameMatches::self().customMatches[i].payload;
            }
        }

        return NULL;
    }

}
