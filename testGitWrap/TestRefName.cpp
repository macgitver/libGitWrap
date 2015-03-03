/*
 * MacGitver
 * Copyright (C) 2012-2015 The MacGitver-Developers <dev@macgitver.org>
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

#include "gtest/gtest.h"

#include "Infra/Fixture.hpp"
#include "libGitWrap/RefName.hpp"

TEST(RefName, AnalyzeLocalBranch) {
    Git::RefName rn = Git::RefName(QStringLiteral("refs/heads/master"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_EQ(0, rn.namespaces().count());
    EXPECT_EQ(0, rn.scopes().count());

    EXPECT_STREQ("master", qUtf8Printable(rn.name()));
    EXPECT_STREQ("master", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("master", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("master", qUtf8Printable(rn.shorthand()));
}

TEST(RefName, Namespaces) {
    Git::RefName rn = Git::RefName(QStringLiteral("refs/namespaces/foo/refs/heads/master"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_TRUE (rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_STREQ("master", qUtf8Printable(rn.name()));
    EXPECT_STREQ("master", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("master", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("", qUtf8Printable(rn.shorthand()));

    ASSERT_EQ   (1, rn.namespaces().count());
    EXPECT_STREQ("foo", qUtf8Printable(rn.namespaces()[0]));
    EXPECT_STREQ("foo", qUtf8Printable(rn.namespaceName()));

    EXPECT_EQ(rn.scopes().count(), 0);
}

TEST(RefName, NestedNamespaces) {
    Git::RefName rn = Git::RefName(QStringLiteral(
        "refs/namespaces/bar/refs/namespaces/foo/refs/heads/master"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_TRUE (rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_STREQ("master", qUtf8Printable(rn.name()));
    EXPECT_STREQ("master", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("master", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("", qUtf8Printable(rn.shorthand()));

    ASSERT_EQ(rn.namespaces().count(), 2);
    EXPECT_STREQ("bar", qUtf8Printable(rn.namespaces()[0]));
    EXPECT_STREQ("foo", qUtf8Printable(rn.namespaces()[1]));
    EXPECT_STREQ("bar/foo", qUtf8Printable(rn.namespaceName()));

    EXPECT_EQ(rn.scopes().count(), 0);
}


TEST(RefName, LocalScopedBranch) {
    Git::RefName rn = Git::RefName(QStringLiteral("refs/heads/feature/cool"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_TRUE (rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_STREQ("cool", qUtf8Printable(rn.name()));
    EXPECT_STREQ("feature/cool", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("feature/cool", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.namespaceName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("feature/cool", qUtf8Printable(rn.shorthand()));

    EXPECT_EQ(rn.namespaces().count(), 0);
    ASSERT_EQ(rn.scopes().count(), 1);
    EXPECT_STREQ("feature", qUtf8Printable(rn.scopes()[0]));
    EXPECT_STREQ("feature", qUtf8Printable(rn.scopeName()));
}

TEST(RefName, LocalNestedScopedBranch) {
    Git::RefName rn = Git::RefName(QStringLiteral("refs/heads/feature/new/cool"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_TRUE (rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_STREQ("cool", qUtf8Printable(rn.name()));
    EXPECT_STREQ("feature/new/cool", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("feature/new/cool", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.namespaceName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("feature/new/cool", qUtf8Printable(rn.shorthand()));

    ASSERT_EQ(2, rn.scopes().count());
    EXPECT_STREQ("feature", qUtf8Printable(rn.scopes()[0]));
    EXPECT_STREQ("new", qUtf8Printable(rn.scopes()[1]));
    EXPECT_STREQ("feature/new", qUtf8Printable(rn.scopeName()));
    EXPECT_EQ(0, rn.namespaces().count());
}

TEST(RefName, ScopedTag) {
    Git::RefName rn = Git::RefName(QStringLiteral("refs/tags/feature/cool"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_TRUE (rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_TRUE (rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_STREQ("cool", qUtf8Printable(rn.name()));
    EXPECT_STREQ("", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("feature/cool", qUtf8Printable(rn.tagName()));
    EXPECT_STREQ("feature/cool", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("feature/cool", qUtf8Printable(rn.shorthand()));

    EXPECT_EQ(rn.namespaces().count(), 0);
    ASSERT_EQ(rn.scopes().count(), 1);
    EXPECT_STREQ("feature", qUtf8Printable(rn.scopes()[0]));
    EXPECT_STREQ("feature", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.namespaceName()));
}

TEST(RefName, NestedScopedTag) {
    Git::RefName rn = Git::RefName(QStringLiteral("refs/tags/feature/new/cool"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_TRUE (rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_TRUE (rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_STREQ("cool", qUtf8Printable(rn.name()));
    EXPECT_STREQ("", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("feature/new/cool", qUtf8Printable(rn.tagName()));
    EXPECT_STREQ("feature/new/cool", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("feature/new/cool", qUtf8Printable(rn.shorthand()));

    ASSERT_EQ(2, rn.scopes().count());
    EXPECT_STREQ("feature", qUtf8Printable(rn.scopes()[0]));
    EXPECT_STREQ("new", qUtf8Printable(rn.scopes()[1]));
    EXPECT_STREQ("feature/new", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.namespaceName()));
    EXPECT_EQ(0, rn.namespaces().count());
}

TEST(RefName, Specials_Stage) {
    Git::RefName rn = Git::RefName(QStringLiteral("refs/stage"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_TRUE (rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_TRUE (rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_STREQ("", qUtf8Printable(rn.name()));
    EXPECT_STREQ("", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("", qUtf8Printable(rn.tagName()));
    EXPECT_STREQ("", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.namespaceName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("", qUtf8Printable(rn.shorthand()));

    ASSERT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());
}

TEST(RefName, Specials_Head) {
    Git::RefName rn = Git::RefName(QStringLiteral("HEAD"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_TRUE (rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_TRUE (rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_STREQ("HEAD", qUtf8Printable(rn.name()));
    EXPECT_STREQ("", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("", qUtf8Printable(rn.tagName()));
    EXPECT_STREQ("HEAD", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.namespaceName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("HEAD", qUtf8Printable(rn.shorthand()));   // special case, shorthand() will give `HEAD`

    ASSERT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());
}

TEST(RefName, Specials_MergeHead) {
    Git::RefName rn = Git::RefName(QStringLiteral("MERGE_HEAD"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_TRUE (rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_TRUE (rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_STREQ("", qUtf8Printable(rn.name()));
    EXPECT_STREQ("", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("", qUtf8Printable(rn.tagName()));
    EXPECT_STREQ("", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.namespaceName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("", qUtf8Printable(rn.shorthand()));

    ASSERT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());
}

TEST(RefName, Specials_CommitNotes) {
    Git::RefName rn = Git::RefName(QStringLiteral("refs/notes/commit"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_TRUE (rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_TRUE (rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_STREQ("commit", qUtf8Printable(rn.name()));
    EXPECT_STREQ("", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("", qUtf8Printable(rn.tagName()));
    EXPECT_STREQ("commit", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.namespaceName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("", qUtf8Printable(rn.shorthand()));

    ASSERT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());
}


TEST(RefName, Peculiars) {
    Git::RefName rn = Git::RefName(QStringLiteral("refs/foobar/commit"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_TRUE (rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_STREQ("", qUtf8Printable(rn.name()));
    EXPECT_STREQ("", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("", qUtf8Printable(rn.tagName()));
    EXPECT_STREQ("", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.namespaceName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("", qUtf8Printable(rn.shorthand()));

    ASSERT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());
}

TEST(RefName, RemoteBranch) {
    Git::RefName rn = Git::RefName(QStringLiteral("refs/remotes/home/new/branch"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_TRUE (rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_TRUE (rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_TRUE (rn.isRemote());

    EXPECT_STREQ("branch", qUtf8Printable(rn.name()));
    EXPECT_STREQ("new/branch", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("", qUtf8Printable(rn.tagName()));
    EXPECT_STREQ("new", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.namespaceName()));
    EXPECT_STREQ("home", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("home/new/branch", qUtf8Printable(rn.shorthand()));

    ASSERT_EQ(1, rn.scopes().count());
    EXPECT_STREQ("new", qUtf8Printable(rn.scopes()[0]));

    EXPECT_EQ(0, rn.namespaces().count());
}

TEST(RefName, RemoteHead) {
    Git::RefName rn = Git::RefName(QStringLiteral("refs/remotes/home/HEAD"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_TRUE (rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_TRUE (rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_TRUE (rn.isRemote());

    EXPECT_STREQ("HEAD", qUtf8Printable(rn.name()));
    EXPECT_STREQ("", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("HEAD", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.tagName()));
    EXPECT_STREQ("", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.namespaceName()));
    EXPECT_STREQ("home", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("home", qUtf8Printable(rn.shorthand()));

    EXPECT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());
}

TEST(RefName, CustomRule) {
    QRegExp re(QStringLiteral("^refs\\/pull\\/([1-9][0-9]*)\\/head$"));
    int id = Git::RefName::registerExpression((void*)0xAABBCCDD, re);

    Git::RefName rn = Git::RefName(QStringLiteral("refs/pull/550/head"));
    EXPECT_TRUE (rn.isValid());

    EXPECT_FALSE(rn.isBranch());
    EXPECT_FALSE(rn.isSpecial());
    EXPECT_FALSE(rn.isTag());
    EXPECT_FALSE(rn.isCommitNote());
    EXPECT_FALSE(rn.isHead());
    EXPECT_FALSE(rn.isMergeHead());
    EXPECT_FALSE(rn.isNamespaced());
    EXPECT_FALSE(rn.isScoped());
    EXPECT_FALSE(rn.isPeculiar());
    EXPECT_FALSE(rn.isStage());
    EXPECT_FALSE(rn.isRemote());

    EXPECT_TRUE (rn.isCustom());
    EXPECT_TRUE (rn.matchesCustomRule(id));

    EXPECT_STREQ("", qUtf8Printable(rn.name()));
    EXPECT_STREQ("", qUtf8Printable(rn.branchName()));
    EXPECT_STREQ("", qUtf8Printable(rn.tagName()));
    EXPECT_STREQ("", qUtf8Printable(rn.localName()));
    EXPECT_STREQ("", qUtf8Printable(rn.scopeName()));
    EXPECT_STREQ("", qUtf8Printable(rn.namespaceName()));
    EXPECT_STREQ("", qUtf8Printable(rn.remote()));
    EXPECT_STREQ("", qUtf8Printable(rn.shorthand()));

    ASSERT_EQ(0, rn.scopes().count());
    EXPECT_EQ(0, rn.namespaces().count());

    rn = Git::RefName(QStringLiteral("refs/heads/master"));
    EXPECT_TRUE (rn.isValid());
    EXPECT_FALSE(rn.isCustom());
    EXPECT_FALSE(rn.matchesCustomRule(id));

    ASSERT_EQ(Git::RefName::expressionData(id), (void*)0xAABBCCDD);

    Git::RefName::unregisterExpression(id);

    ASSERT_NE(Git::RefName::expressionData(id), (void*)0xAABBCCDD);
}

