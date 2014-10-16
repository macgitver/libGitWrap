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

#include "libGitWrap/ChangeListConsumer.hpp"
#include "libGitWrap/PatchConsumer.hpp"
#include "libGitWrap/DiffList.hpp"
#include "libGitWrap/Repository.hpp"

#include "libGitWrap/Private/GitWrapPrivate.hpp"
#include "libGitWrap/Private/DiffListPrivate.hpp"
#include "libGitWrap/Private/RepositoryPrivate.hpp"

namespace Git
{

    namespace Internal
    {

        class DiffListConsumer : public ChangeListConsumer
        {
        public:
            bool startFileChange(const Git::ChangeListEntry &entry);

            const Git::ChangeList &changeList() const;

        private:
            Git::ChangeList      mChanges;
        };

        bool DiffListConsumer::startFileChange(const Git::ChangeListEntry &entry)
        {
            mChanges.append( entry );

            return true;
        }

        const ChangeList &DiffListConsumer::changeList() const
        {
            return mChanges;
        }

        DiffListPrivate::DiffListPrivate(const RepositoryPrivate::Ptr& repo, git_diff* diff)
            : RepoObjectPrivate(repo)
            , mDiff(diff)
        {
            Q_ASSERT(diff);
        }

        DiffListPrivate::~DiffListPrivate()
        {
            Q_ASSERT(mRepo);

            if(mDiff) {
                git_diff_free(mDiff);
            }
        }

        static int patchFileCallBack( const git_diff_delta* delta, float, void* cb_data )
        {
            PatchConsumer* pc = (PatchConsumer*) cb_data;

            ChangeListEntry entry =
            {
                QString::fromUtf8( delta->old_file.path )
                , QString::fromUtf8( delta->new_file.path )
                , PatchConsumer::Type( delta->status )
                , delta->similarity
                , ( delta->flags & GIT_DIFF_FLAG_BINARY ) != 0
            };

            if( pc->startFileChange( entry ) )
            {
                return GIT_OK;
            }

            return GIT_ERROR;
        }

        static int patchHunkCallBack(const git_diff_delta* delta,
                                     const git_diff_hunk* hunk,
                                     void* cb_data )
        {
            PatchConsumer* pc = (PatchConsumer*) cb_data;
            QString header;

            if (hunk->header_len) {
                header = QString::fromUtf8(hunk->header, int(hunk->header_len));
            }

            if (pc->startHunkChange(hunk->new_start, hunk->new_lines,
                                    hunk->old_start, hunk->old_lines, header)) {
                return GIT_OK;
            }

            return GIT_ERROR;
        }

        static int patchDataCallBack( const git_diff_delta* delta,
                                      const git_diff_hunk* hunk,
                                      const git_diff_line* line,
                                      void* cb_data )
        {
            Q_UNUSED(delta);
            Q_UNUSED(hunk);
            PatchConsumer* pc = (PatchConsumer*) cb_data;

            QString ct;
            if(line->content && line->content_len) {

                int len = int(line->content_len);
                if(line->content[len-1] == '\n') {
                    --len;
                }

                ct = QString::fromUtf8(line->content, len);
            }

            switch(line->origin) {
            case GIT_DIFF_LINE_CONTEXT:
                if (!pc->appendContext(ct)) {
                    return GIT_ERROR;
                }
                break;

            case GIT_DIFF_LINE_ADDITION:
                if (!pc->appendAddition(ct)) {
                    return GIT_ERROR;
                }
                break;

            case GIT_DIFF_LINE_DELETION:
                if (!pc->appendDeletion(ct)) {
                    return GIT_ERROR;
                }
                break;

            default:
                qDebug() << "Foo: t=" << int(line->origin);
            }
            return GIT_OK;
        }

        static int changeListCallBack( const git_diff_delta* delta, float, void* cb_data )
        {
            ChangeListConsumer* consumer = (ChangeListConsumer*) cb_data;

            ChangeListEntry change =
            {
                QString::fromUtf8( delta->old_file.path )
                , QString::fromUtf8( delta->new_file.path )
                , ChangeListConsumer::Type( delta->status )
                , delta->similarity
                , ( delta->flags & GIT_DIFF_FLAG_BINARY ) != 0
            };

            if( consumer->startFileChange( change ) )
            {
                return GIT_OK;
            }

            return GIT_ERROR;
        }

    }

    GW_PRIVATE_IMPL(DiffList, RepoObject)

    bool DiffList::mergeOnto(Result& result, DiffList onto) const
    {
        GW_CD_CHECKED(DiffList, false, result)

        if (!onto.isValid())
        {
            result.setInvalidObject();
            return false;
        }

        DiffList::Private* ontoP = Private::dataOf<DiffList>(onto);
        result = git_diff_merge(ontoP->mDiff, d->mDiff);
        return result;
    }

    bool DiffList::consumePatch(Result& result, PatchConsumer* consumer) const
    {
        GW_CD_CHECKED(DiffList, false, result)

        if (!consumer) {
            result.setInvalidObject();
            return false;
        }

        result = git_diff_foreach(d->mDiff,
                                  &Internal::patchFileCallBack,
                                  &Internal::patchHunkCallBack,
                                  &Internal::patchDataCallBack,
                                  consumer );

        return result;
    }

    bool DiffList::consumeChangeList(Result& result, ChangeListConsumer* consumer) const
    {
        GW_CD_CHECKED(DiffList, false, result)

        if (!consumer) {
            result.setInvalidObject();
            return false;
        }

        result = git_diff_foreach(d->mDiff,
                                  &Internal::changeListCallBack,
                                  NULL,
                                  NULL,
                                  consumer );

        return result;
    }

    ChangeList DiffList::changeList(Result &result) const
    {
        Internal::DiffListConsumer consumer;
        consumeChangeList(result, &consumer);

        return consumer.changeList();
    }

    /**
     * @brief   Try to find renames
     *
     * @param[in,out] result
     *
     * @return        `true` on success, otherwise `false`.
     */
    bool DiffList::findRenames( Result& result )
    {
        GW_CD_CHECKED(DiffList, false, result)

        git_diff_find_options opts = GIT_DIFF_FIND_OPTIONS_INIT;
        result = git_diff_find_similar(d->mDiff, &opts);

        return result;
    }

}
