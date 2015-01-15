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

#include "libGitWrap/Commit.hpp"
#include "libGitWrap/ObjectId.hpp"
#include "libGitWrap/Repository.hpp"
#include "libGitWrap/Result.hpp"
#include "libGitWrap/Tree.hpp"

#include "Infra/Fixture.hpp"
#include "Infra/TempRepo.hpp"

typedef Fixture CommitFixture;

TEST_F(CommitFixture, CanCreateFirstCommitFromTree)
{
    Git::Result r;
    Git::Repository repo( TempRepoOpener(this, "SimpleRepo1", r) );
    CHECK_GIT_RESULT(r);
    ASSERT_TRUE(repo.isValid());

    Git::Signature sig = Git::Signature::defaultSignature(r, repo);
    CHECK_GIT_RESULT(r);
    Git::Tree tree;
    Git::Commit commit = Git::Commit::create( r, repo, tree, QString::fromUtf8("Initial Commit"), sig, sig, Git::ObjectIdList() );
    CHECK_GIT_RESULT(r);
    ASSERT_TRUE(commit.isValid());

    Git::Commit lookedUp = repo.lookupCommit( r, commit.id() );
    CHECK_GIT_RESULT(r);
    ASSERT_TRUE(lookedUp.isValid());
    ASSERT_EQ( commit, lookedUp );
    ASSERT_EQ( commit.id(), lookedUp.id() );
}
