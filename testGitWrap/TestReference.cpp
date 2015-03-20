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

#include "gtest/gtest.h"

#include "libGitWrap/Result.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Reference.hpp"

#include "Infra/Fixture.hpp"
#include "Infra/TempRepo.hpp"

typedef Fixture ReferenceFixture;

TEST_F(ReferenceFixture, CanLookup)
{
    Git::Result r;
    TempRepoOpener tempRepo(this, "SimpleRepo1", r);
    CHECK_GIT_RESULT(r);
    Git::Repository repo(tempRepo);
    ASSERT_TRUE(repo.isValid());

    Git::Reference ref = repo.reference(r, QStringLiteral("refs/heads/master"));
    CHECK_GIT_RESULT(r);
    ASSERT_TRUE(ref.isValid());
    ASSERT_FALSE(ref.wasDestroyed());

    EXPECT_EQ(Git::ReferenceDirect, ref.type());
    EXPECT_EQ(Git::BranchReference, ref.kind());
    EXPECT_TRUE(ref.isCurrentBranch());
    EXPECT_TRUE(ref.isLocal());
    EXPECT_FALSE(ref.isRemote());

    EXPECT_STREQ("refs/heads/master", qPrintable(ref.name()));

    ref = repo.reference(r, QStringLiteral("master"));
    ASSERT_FALSE(r);
    ASSERT_FALSE(ref.isValid());
    r.clear();
}

TEST_F(ReferenceFixture, CanLookupShorthand)
{
    Git::Result r;
    TempRepoOpener tempRepo(this, "SimpleRepo1", r);
    CHECK_GIT_RESULT(r);
    Git::Repository repo(tempRepo);
    ASSERT_TRUE(repo.isValid());

    Git::Reference ref = repo.reference(r, QStringLiteral("master"), true);
    CHECK_GIT_RESULT(r);
    ASSERT_TRUE(ref.isValid());
    ASSERT_FALSE(ref.wasDestroyed());

    EXPECT_EQ(Git::ReferenceDirect, ref.type());
    EXPECT_EQ(Git::BranchReference, ref.kind());
    EXPECT_TRUE(ref.isCurrentBranch());
    EXPECT_TRUE(ref.isLocal());
    EXPECT_FALSE(ref.isRemote());

    EXPECT_STREQ("refs/heads/master", qPrintable(ref.name()));
}

TEST_F(ReferenceFixture, CanDestroyRef)
{
    Git::Result r;
    TempRepoOpener tempRepo(this, "SimpleRepo1", r);
    CHECK_GIT_RESULT(r);
    Git::Repository repo(tempRepo);
    ASSERT_TRUE(repo.isValid());

    Git::Reference ref = repo.reference(r, QStringLiteral("master"), true);
    ASSERT_FALSE(ref.wasDestroyed());

    ref.destroy(r);
    CHECK_GIT_RESULT(r);
    ASSERT_TRUE(ref.wasDestroyed());

    ASSERT_TRUE(ref.objectId().isNull());

    // counter check
    Git::Repository repo2 = repo.reopen(r);
    CHECK_GIT_RESULT(r);

    QStringList sl = repo2.allBranchNames(r);
    CHECK_GIT_RESULT(r);
    ASSERT_EQ(0, sl.count());
}
