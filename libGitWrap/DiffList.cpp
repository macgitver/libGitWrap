/*
 * MacGitver
 * Copyright (C) 2015 Sascha Cunz <sascha@macgitver.org>
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

#include "libGitWrap/Private/DiffPrivate.hpp"
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


        //-- DiffListPrivate -->8

        DiffListPrivate::DiffListPrivate(RepositoryPrivate* repo, git_diff* diff)
            : RepoObjectPrivate( repo )
            , mDiff( diff )
        {
            // TODO: check, that repo equals diff->repo
            Q_ASSERT(diff);
        }

        DiffListPrivate::~DiffListPrivate()
        {
            git_diff_free(mDiff);
        }


        /**
          * @internal
          * @ingroup GritWrap
          *
          * @class      DiffFile
          *
          * @brief      Private wrapper for git_diff_file.
          */

        /**
         * @brief           DiffFilePrivate::DiffFilePrivate
         *
         * @param           diffFile    the git_diff_file
         */
        DiffFilePrivate::DiffFilePrivate(const git_diff_file* diffFile)
            : mDiffFile( diffFile )
        {
        }

        DiffFilePrivate::~DiffFilePrivate()
        {
            // FIXME: Do we have a mem-leak here for mDiffFile?
        }


        //-- internal callbacks --8>

        /**
         * @brief           patchFileCallBack
         *
         * @param           delta
         *
         * @param           cb_data     the payload
         *
         * @return          the reutrned error value is meatn to be used for @ref Git::Result
         */
        static int patchFileCallBack( const git_diff_delta* delta, float, void* cb_data )
        {
            PatchConsumer* pc = (PatchConsumer*) cb_data;

            ChangeListEntry entry =
            {
                GW_StringToQt( delta->old_file.path )
                , GW_StringToQt( delta->new_file.path )
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
                header = GW_StringToQt(hunk->header, int(hunk->header_len));
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

                ct = GW_StringToQt(line->content);
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
                GW_StringToQt( delta->old_file.path )
                , GW_StringToQt( delta->new_file.path )
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


    //-- DiffFile -->8

    GW_PRIVATE_IMPL( DiffFile, Base )


    //-- DiffList -->8

    GW_PRIVATE_IMPL(DiffList, RepoObject)

    void DiffList::mergeOnto(Result& result, const DiffList& onto) const
    {
        GW_CD_CHECKED(DiffList, void(), result);

        if (!onto.isValid())
        {
            result.setInvalidObject();
            return;
        }

        DiffList::Private* ontoP = Private::dataOf<DiffList>(onto);
        result = git_diff_merge(ontoP->mDiff, d->mDiff);
    }

    void DiffList::consumePatch(Result& result, PatchConsumer* consumer) const
    {
        GW_CD_CHECKED(DiffList, void(), result);

        if (!consumer) {
            result.setInvalidObject();
            return;
        }

        result = git_diff_foreach(d->mDiff,
                                  &Internal::patchFileCallBack,
                                  &Internal::patchHunkCallBack,
                                  &Internal::patchDataCallBack,
                                  consumer );
    }

    void DiffList::consumeChangeList(Result& result, ChangeListConsumer* consumer) const
    {
        GW_CD_CHECKED(DiffList, void(), result);

        if (!consumer) {
            result.setInvalidObject();
            return;
        }

        result = git_diff_foreach(d->mDiff,
                                  &Internal::changeListCallBack,
                                  NULL,
                                  NULL,
                                  consumer );
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
        GW_CD_CHECKED(DiffList, false, result);

        git_diff_find_options opts = GIT_DIFF_FIND_OPTIONS_INIT;
        result = git_diff_find_similar(d->mDiff, &opts);

        return result;
    }

}
