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

#include "Result.hpp"
#include "Index.hpp"
#include "Remote.hpp"
#include "Repository.hpp"
#include "Reference.hpp"
#include "DiffList.hpp"
#include "Object.hpp"
#include "ObjectTag.hpp"
#include "ObjectTree.hpp"
#include "ObjectBlob.hpp"
#include "ObjectCommit.hpp"
#include "RevisionWalker.hpp"

#include "Private/GitWrapPrivate.hpp"
#include "Private/IndexPrivate.hpp"
#include "Private/RemotePrivate.hpp"
#include "Private/RepositoryPrivate.hpp"
#include "Private/ReferencePrivate.hpp"
#include "Private/DiffListPrivate.hpp"
#include "Private/ObjectPrivate.hpp"
#include "Private/RevisionWalkerPrivate.hpp"

#include <QDir>

namespace Git
{

    namespace Internal
    {

        RepositoryPrivate::RepositoryPrivate( git_repository* repo )
            : mRepo( repo )
            , mIndex( NULL )
        {
        }

        RepositoryPrivate::~RepositoryPrivate()
        {
            Q_ASSERT( mRepo );

            // This assert may not look right in the first place, but it IS:
            // mIndex is of type IndexPrivate* and will get a value as soon as Repository::index()
            // is called for the first time. IndexPrivate is a RepoObject and as such it increases
            // the reference counter on the Repository object. In fact, this means the Repository
            // will never be deleted unless the Index has gone _before_ - When the Index is deleted
            // (due to the refCount dropping to zero) it will set mIndex to NULL.
            //
            // I'm documenting this mainly for one reason: Last week I spent 3 hours trying to fix
            // a race-condition in libgit2, which - as I later understood - is not at all there
            // because outer constraints - like the above - prohibited the race to happen.
            Q_ASSERT( !mIndex );

            git_repository_free( mRepo );
        }

        static int statusHashCB( const char* fn, unsigned int status, void* rawSH )
        {
            #if 0
            qDebug( "%s - %s", qPrintable( QString::number( status, 2 ) ), fn );
            #endif

            Git::StatusHash* sh = (Git::StatusHash*) rawSH;
            sh->insert( QString::fromUtf8( fn ), convertFileStatus( status ) );

            return GIT_OK;
        }

        struct cb_append_reference_data
        {
            RepositoryPrivate* ptr;
            ReferenceList refs;
        };

        static int cb_append_reference( git_reference *reference, void *payload )
        {
            cb_append_reference_data *data = (cb_append_reference_data *)payload;

            data->refs.append(*new ReferencePrivate(data->ptr, reference));

            return 0;
        }

    }

    /**
     * @internal
     * @brief       Create a Repository object
     * @param[in]   _d  Pointer to private data.
     */
    Repository::Repository(Internal::RepositoryPrivate& _d )
        : Base(_d)
    {
    }

    /**
     * @brief       Copy Constructor
     *
     * Creates a Repository object that represents the same repository as @a o. If @a o is an
     * invalid Repository object, this will become an invalid one too.
     *
     * @param[in]   o   An existing Repository object
     *
     */
    Repository::Repository(const Repository& other)
        : Base(other)
    {
    }

    /**
     * @brief       Create an invalid Repository object
     */
    Repository::Repository()
    {
    }

    /**
     * @brief       Destructor
     */
    Repository::~Repository()
    {
    }

    /**
     * @brief       Assignment operator
     *
     * @param[in]   o   An existing Repository object
     *
     * @return      A reference to this repository object.
     */
    Repository& Repository::operator=(const Repository& other)
    {
        Base::operator=(other);
        return * this;
    }

    /**
     * @brief       Create a new repository
     *
     * A new git repository will be created in @a path. The path pointed to by @a path must either
     * be empty or not exist at all.
     *
     * Capabilities of the current operating system and the file system at @a path will be checked
     * and the repository's config will be setup accordingly.
     *
     * @param[in]       path    The path where the new repository will be created.
     * @param[in]       bare    If `true`, a bare repository will be created. If `false` a working
     *                          tree will be setup.
     * @param[in,out]	result  A Result object; see @ref GitWrapErrorHandling
     *
     * @return  If successful, a `Repository` object for the newly created repostiory will be
     *          returned. Otherwise an invalid `Repository` object will be returned and the Result
     *          object is filled with the error.
     *
     * @see	Repository::open()
     */
    Repository Repository::create( const QString& path,
                                   bool bare,
                                   Result& result )
    {
        if( !result )
        {
            return Repository();
        }

        git_repository* repo = NULL;
        result = git_repository_init( &repo, path.toUtf8().constData(), bare );

        if( !result )
        {
            return Repository();
        }

        return Repository(*new Internal::RepositoryPrivate(repo));
    }

    /**
     * @brief Lookup a git repository by walking parent directories starting from startPath
     *
     * The lookup ends when the first repository is found or when reaching one of the @a ceilingDirs
     * directories.
     *
     * The method will automatically detect if the repository is bare (if there is a repository).
     *
     * @param[in] startPath
     * The base path where the lookup starts.
     *
     * @param[in] acrossFs
     * If `true`, then the lookup will not stop when a filesystem change is detected
     * while exploring parent directories.
     *
     * @param[in] ceilingDirs
     * A list of absolute paths (not symbolic links). The lookup will stop when one of these
     * paths is reached and no repository was found.
     *
     * @param[in,out] result
     * A result object; see @ref GitWrapErrorHandling
     *
     * @return the path of the found repository or an empty QString
     *
     * @see Repository::open(), Repository::create()
     */
    QString Repository::discover( const QString& startPath,
                                  bool acrossFs,
                                  const QStringList& ceilingDirs,
                                  Result& result )
    {
        if( !result )
        {
            return QString();
        }

        QByteArray repoPath(GIT_PATH_MAX, Qt::Uninitialized);
        QByteArray joinedCeilingDirs = ceilingDirs.join(QChar::fromLatin1(GIT_PATH_LIST_SEPARATOR)).toUtf8();

        result = git_repository_discover( repoPath.data(), repoPath.length(),
                                          startPath.toUtf8().constData(), acrossFs,
                                          joinedCeilingDirs.constData() );

        return result ? QString::fromUtf8(repoPath.constData()) : QString();
    }

    /**
     * @brief       Open an existing repository
     *
     * Opens the repository at @a path. The repository may be bare or have a working tree.
     *
     * This method will not try to discover a repository, if there is no repository found at
     * @a path.
     *
     * @param[in]       path    The path of the repository to open.
     * @param[in,out]   result  A result object; see @ref GitWrapErrorHandling
     *
     * @return  If successful, a `Repository` object for the opened repostiory will be returned.
     *          Otherwise an invalid `Repository` object will be returned and the Result object
     *          is filled with the error.
     *
     * @sa	Repository::discover(), Repository::create()
     */
    Repository Repository::open( const QString& path,
                                 Result& result )
    {
        if( !result )
        {
            return Repository();
        }
        git_repository* repo = NULL;


        result = git_repository_open( &repo, path.toUtf8().constData() );
        if( !result )
        {
            return Repository();
        }

        return Repository(*new Internal::RepositoryPrivate(repo));
    }

    /**
     * @brief       Check if the repository is bare
     *
     * @return      `true`, if the repository is bare and `false` if not.
     *
     * This method will return `true` if this Repository object is invalid. It will also set the
     * per-thread Result to "Invalid object".
     */
    bool Repository::isBare() const
    {
        GW_CD(Repository);
        if( d )
        {
            return git_repository_is_bare( d->mRepo );
        }
        else
        {
            GitWrap::lastResult().setInvalidObject();
            return true;
        }
    }

    /**
     * @brief       Check whether the repository's HEAD is detached
     *
     * @return      @c true if this Repository is valid and its HEAD branch points to a commit
     *              rather than to another reference. @c false if the Repository is either invalid
     *              or its HEAD points to another reference.
     */
    bool Repository::isHeadDetached() const
    {
        GW_CD(Repository);
        return d && git_repository_head_detached(d->mRepo);
    }

    /**
     * @brief           Access the repository's index object
     *
     * @param[in,out]   result	A result object; see @ref GitWrapErrorHandling
     *
     * @return          The repository's index object or an invalid Index object, if either the
     *                  repository isBare() or an invalid repository object.
     *
     * If this repository object is invalid, the @a result object will be set to
     * "Invalid object".
     */
    Index Repository::index( Result& result )
    {
        GW_D_CHECKED(Repository, Index(), result);

        if( isBare() ) {
            return Index();
        }

        if (!d->mIndex) {
            git_index* index = NULL;

            result = git_repository_index(&index, d->mRepo);

            if (!result) {
                return Index();
            }

            d->mIndex = new Internal::IndexPrivate(d, index);
        }

        return Index(*d->mIndex);
    }

    /**
     * @brief           get the status of a single file
     *
     * @param[in,out]   result      A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       fileName    A path (relative to the repository) for which to get the
     *                              current status.
     *
     * @return                      the current file status
     *
     *                  The file status is a combination of worktree, index and repository HEAD.
     */
    Git::StatusFlags Repository::status(Result &result, const QString &fileName) const
    {
        unsigned int status = GIT_STATUS_CURRENT;
        GW_CD_CHECKED(Repository, FileInvalidStatus, result);

        result = git_status_file( &status, d->mRepo, fileName.toUtf8().data() );
        if (!result) {
            return FileInvalidStatus;
        }

        return Internal::convertFileStatus( status );
    }

    StatusHash Repository::status(Result &result) const
    {
        StatusHash sh;
        GW_CD_CHECKED(Repository, StatusHash(), result);

        git_status_options opt = GIT_STATUS_OPTIONS_INIT;

        opt.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED
                  | GIT_STATUS_OPT_INCLUDE_IGNORED
                  | GIT_STATUS_OPT_INCLUDE_UNMODIFIED
                  | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;

        result = git_status_foreach_ext( d->mRepo, &opt, &Internal::statusHashCB, (void*) &sh );
        if( !result )
        {
            return Git::StatusHash();
        }

        return sh;
    }

    /**
     * @brief           List all references
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @return  A QStringList with all references of this repository.
     *
     */
    QStringList Repository::allReferenceNames( Result& result )
    {
        GW_D_CHECKED(Repository, QStringList(), result);

        git_strarray arr;
        result = git_reference_list( &arr, d->mRepo );
        if( !result )
        {
            return QStringList();
        }

        return Internal::slFromStrArray( &arr );
    }

    ReferenceList Repository::allReferences(Result &result)
    {
        GW_D_CHECKED(Repository, ReferenceList(), result);

        Internal::cb_append_reference_data data = { d };
        result = git_reference_foreach( d->mRepo,
                                        &Internal::cb_append_reference,
                                        &data );
        if (!result) return ReferenceList();

        return data.refs;
    }

    namespace Internal
    {

        struct cb_enum_resolvedrefs_data
        {
            Result*         result;
            ResolvedRefs    refs;
            git_repository* repo;
        };

        static int cb_enum_resolvedrefs( git_reference* ref, void* payload )
        {
            cb_enum_resolvedrefs_data* d = (cb_enum_resolvedrefs_data*) payload;

            git_oid oid;

            const char *refName = git_reference_name(ref);
            int rc = git_reference_name_to_id( &oid, d->repo, refName );

            d->result->setError( rc );
            if( rc < 0 )
            {
                return -1;
            }

            d->refs.insert( QString::fromUtf8( refName ), ObjectId::fromRaw( oid.id ) );

            return 0;
        }

    }

    ResolvedRefs Repository::allResolvedRefs( Result& result )
    {
        GW_CD_CHECKED(Repository, ResolvedRefs(), result);

        Internal::cb_enum_resolvedrefs_data data;
        data.repo = d->mRepo;
        data.result = &result;

        Result tmp( git_reference_foreach( d->mRepo, &Internal::cb_enum_resolvedrefs, &data ) );

        if (tmp.errorCode() == GIT_EUSER) {
            // correct error is already in result.
            return ResolvedRefs();
        }

        if (!tmp) {
            result = tmp;
            return ResolvedRefs();
        }

        return data.refs;
    }

    QStringList Repository::allBranchNames( Result& result )
    {
        return branchNames( result, true, true );
    }

    QString Repository::currentBranch(Result &result)
    {
        Reference refHEAD = HEAD( result );
        if ( !refHEAD.isValid() ) return QString();

        return refHEAD.shorthand();
    }

    namespace Internal
    {

        static int listBranches( const char* branchName, git_branch_t brachType, void* payload )
        {
            QStringList* sl = (QStringList*) payload;
            sl->append( QString::fromUtf8( branchName ) );
            return 0;
        }

    }

    QStringList Repository::branchNames(Result& result, bool local, bool remote)
    {
        GW_CD_CHECKED(Repository, QStringList(), result);

        QStringList sl;
        result = git_branch_foreach( d->mRepo,
                                     ( local ? GIT_BRANCH_LOCAL : 0 ) |
                                     ( remote ? GIT_BRANCH_REMOTE : 0 ),
                                     &Internal::listBranches,
                                     (void*) &sl );
        if( !result )
        {
            return QStringList();
        }

        return sl;
    }

    bool Repository::renameBranch( const QString& oldName, const QString& newName, bool force,
                                   Result& result )
    {
        GW_CD_CHECKED(Repository, false, result);

        git_reference* ref = NULL;

        result = git_branch_lookup( &ref, d->mRepo, oldName.toUtf8().constData(),
                                    GIT_BRANCH_LOCAL );

        if( result.errorCode() == GITERR_REFERENCE )
        {
            result = git_branch_lookup( &ref, d->mRepo, oldName.toUtf8().constData(),
                                        GIT_BRANCH_REMOTE );
        }

        if( !result )
        {
            return false;
        }

        git_reference* refOut = NULL;
        result = git_branch_move( &refOut, ref, newName.toUtf8().constData(), force );

        if( result )
        {
            git_reference_free( refOut );
        }

        git_reference_free( ref );

        return result;
    }

    QStringList Repository::allTagNames( Result& result )
    {
        GW_CD_CHECKED(Repository, QStringList(), result);

        git_strarray arr;
        result = git_tag_list( &arr, d->mRepo );

        if( !result )
        {
            return QStringList();
        }

        return Internal::slFromStrArray( &arr );
    }

    QString Repository::basePath() const
    {
        GW_CD(Repository);
        if( !d )
        {
            GitWrap::lastResult().setInvalidObject();
            return QString();
        }

        return QString::fromUtf8( git_repository_workdir( d->mRepo ) );
    }

    QString Repository::gitPath() const
    {
        GW_CD(Repository);
        if( !d )
        {
            GitWrap::lastResult().setInvalidObject();
            return QString();
        }

        return QString::fromUtf8( git_repository_path( d->mRepo ) );
    }

    /**
     * @brief   Get the name of the Repository
     *
     * The name of a repository is the directory where the repository is located. If the trailing
     * `.git` is present, it will be stripped.
     *
     * @return  Name of the repository
     */
    QString Repository::name() const
    {
        QString path( isBare() ? gitPath() : basePath() );
        if( path.endsWith( QChar( L'/') ) )
        {
            path = path.left( path.length() - 1 );
        }

        QFileInfo fi( path );

        if( fi.suffix() == QLatin1String( "git" ) )
        {
            return fi.completeBaseName();
        }

        return fi.fileName();
    }

    Reference Repository::HEAD( Result& result ) const
    {
        GW_D_CHECKED(Repository, Reference(), result);

        git_reference* refHead = NULL;

        result = git_repository_head( &refHead, d->mRepo );
        if( !result )
        {
            return Reference();
        }

        return *new Internal::ReferencePrivate(d, refHead);
    }

    Object Repository::lookup( Result& result, const ObjectId& id, ObjectType ot )
    {
        GW_D_CHECKED(Repository, Object(), result);

        git_object* obj = NULL;
        git_otype gitObjType;

        switch( ot )
        {
        case otAny:     gitObjType = GIT_OBJ_ANY;       break;
        case otCommit:  gitObjType = GIT_OBJ_COMMIT;    break;
        case otTree:    gitObjType = GIT_OBJ_TREE;      break;
        case otTag:     gitObjType = GIT_OBJ_TAG;       break;
        case otBlob:    gitObjType = GIT_OBJ_BLOB;      break;
        default:        Q_ASSERT( false ); return Object();
        }

        result = git_object_lookup( &obj, d->mRepo, (git_oid*) id.raw(), gitObjType );
        if( !result )
        {
            return Object();
        }

        return *new Internal::ObjectPrivate(d, obj);
    }

    ObjectCommit Repository::lookupCommit(Result& result, const ObjectId& id)
    {
        return lookup( result, id, otCommit ).asCommit( result );
    }

    ObjectTree Repository::lookupTree(Result& result, const ObjectId& id)
    {
        return lookup( result, id, otTree ).asTree( result );
    }

    ObjectBlob Repository::lookupBlob(Result& result, const ObjectId& id)
    {
        return lookup( result, id, otBlob ).asBlob( result );
    }

    ObjectTag Repository::lookupTag(Result& result, const ObjectId& id)
    {
        return lookup( result, id, otTag ).asTag( result );
    }

    Object Repository::lookup( Result& result, const QString& refName, ObjectType ot )
    {
        return lookup( result, lookupRef( result, refName ).resolveToObjectId( result ), ot );
    }

    ObjectCommit Repository::lookupCommit(Result& result, const QString& refName)
    {
        return lookupCommit( result, lookupRef( result, refName ).resolveToObjectId( result ) );
    }

    ObjectTree Repository::lookupTree(Result& result, const QString& refName)
    {
        return lookupTree( result, lookupRef( result, refName ).resolveToObjectId( result ) );
    }

    ObjectBlob Repository::lookupBlob(Result& result, const QString& refName)
    {
        return lookupBlob( result, lookupRef( result, refName ).resolveToObjectId( result ) );
    }

    ObjectTag Repository::lookupTag(Result& result, const QString& refName)
    {
        return lookupTag( result, lookupRef( result, refName ).resolveToObjectId( result ) );
    }

    RevisionWalker Repository::newWalker( Result& result )
    {
        GW_D_CHECKED(Repository, RevisionWalker(), result);
        git_revwalk* walker = NULL;

        result = git_revwalk_new( &walker, d->mRepo );
        if( !result )
        {
            return RevisionWalker();
        }

        return *new Internal::RevisionWalkerPrivate(d, walker);
    }

    bool Repository::shouldIgnore(Result& result, const QString& filePath) const
    {
        GW_CD_CHECKED(Repository, false, result);

        int ignore = 0;

        result = git_status_should_ignore( &ignore, d->mRepo, filePath.toUtf8().constData() );
        if( !result )
        {
            return false;
        }

        return ignore;
    }

    /**
     * @brief           Get a list of all remotes
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @return          A list of Git::Remote objects containing all remotes of this repository.
     *
     */
    Remote::List Repository::allRemotes(Result& result) const
    {
        GW_CD_CHECKED(Repository, Remote::List(), result);

        git_strarray arr;
        result = git_remote_list( &arr, d->mRepo );
        if (!result) {
            return Remote::List();
        }

        Remote::List remotes;
        for (size_t i = 0; i < arr.count; i++) {
            git_remote* remote = NULL;
            result = git_remote_load(&remote, d->mRepo, arr.strings[i]);
            if (!result) {
                return Remote::List();
            }
            Remote rm = *new Internal::RemotePrivate(const_cast< Internal::RepositoryPrivate* >(d),
                                                     remote);
            remotes.append(rm);
        }

        return remotes;
    }

    /**
     * @brief           Get a list of all remotes names
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @return          A QStringList containing all names of the remotes of this repository.
     *
     */
    QStringList Repository::allRemoteNames( Result& result ) const
    {
        GW_CD_CHECKED(Repository, QStringList(), result);

        git_strarray arr;
        result = git_remote_list( &arr, d->mRepo );
        if( !result )
        {
            return QStringList();
        }

        return Internal::slFromStrArray( &arr );
    }

    /**
     * @brief           Find a remote by its name
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @param[in]       remoteName  The name of the remote to find.
     *
     * @return          The Git::Remote that was found or an invalid Git::Remote object in case of
     *                  any error.
     *
     */
    Remote Repository::remote(Result& result, const QString& remoteName) const
    {
        GW_CD_CHECKED(Repository, Remote(), result);

        git_remote* remote = NULL;
        result = git_remote_load( &remote, d->mRepo, remoteName.toUtf8().constData() );

        if( !result )
        {
            return Remote();
        }

        return *new Internal::RemotePrivate(const_cast<Internal::RepositoryPrivate*>(d), remote);
    }

    Remote Repository::createRemote(Result& result, const QString& remoteName, const QString& url,
                                    const QString& fetchSpec)
    {
        GW_D_CHECKED(Repository, Remote(), result);

        git_remote* remote = NULL;
        result = git_remote_create( &remote, d->mRepo, remoteName.toUtf8().constData(),
                                    url.toUtf8().constData() );
        if( !result )
        {
            return Remote();
        }

        Remote remo = *new Internal::RemotePrivate(d, remote);

        if( !fetchSpec.isEmpty() )
        {
            remo.addFetchSpec( result, fetchSpec );
            if( !result )
            {
                return Remote();
            }
        }

        return remo;
    }

    DiffList Repository::diffCommitToCommit( Result& result, ObjectCommit oldCommit,
                                             ObjectCommit newCommit )
    {
        return diffTreeToTree( result, oldCommit.tree( result ), newCommit.tree( result ) );
    }

    DiffList Repository::diffTreeToTree(Result& result, ObjectTree oldTree, ObjectTree newTree)
    {
        return oldTree.diffToTree( result, newTree );
    }

    DiffList Repository::diffIndexToTree(Result& result, ObjectTree oldTree)
    {
        return oldTree.diffToIndex( result );
    }

    DiffList Repository::diffTreeToWorkingDir(Result& result, ObjectTree oldTree)
    {
        return oldTree.diffToWorkingDir( result );
    }

    DiffList Repository::diffIndexToWorkingDir( Result& result )
    {
        GW_D_CHECKED(Repository, DiffList(), result);

        git_diff_list* diffList = NULL;
        result = git_diff_index_to_workdir( &diffList, d->mRepo, NULL, NULL );
        if( !result )
        {
            return DiffList();
        }

        return DiffList(*new Internal::DiffListPrivate(d, diffList));
    }

    namespace Internal
    {

        struct cb_enum_submodules_t
        {
            RepositoryPrivate*  repo;
            Submodule::List     subs;
        };

        static int cb_enum_submodules( git_submodule* sm, const char* name, void* payload )
        {
            cb_enum_submodules_t* d = static_cast< cb_enum_submodules_t* >( payload );
            Q_ASSERT( d && name );

            d->subs.append( Submodule( d->repo, QString::fromUtf8( name ) ) );
            return 0;
        }

    }

    Submodule::List Repository::submodules( Result& result )
    {
        GW_D_CHECKED(Repository, Submodule::List(), result);
        Internal::cb_enum_submodules_t data = { d };

        result = git_submodule_foreach( d->mRepo, &Internal::cb_enum_submodules, &data );
        if( !result )
        {
            return Submodule::List();
        }

        return data.subs;
    }

    Submodule Repository::submodule(Result& result, const QString& name)
    {
        GW_D_CHECKED(Repository, Submodule(), result);
        return Submodule(d, name);
    }

    Reference Repository::lookupRef(Result& result, const QString& refName, bool dwim)
    {
        GW_D_CHECKED(Repository, Reference(), result);

        git_reference* ref = NULL;
        if ( dwim )
            result = git_reference_dwim( &ref, d->mRepo, refName.toUtf8().constData() );
        else
            result = git_reference_lookup( &ref, d->mRepo, refName.toUtf8().constData() );

        if( !result )
        {
            return Reference();
        }

        return *new Internal::ReferencePrivate( d, ref );
    }

    /**
     * @brief           Detach the current HEAD
     *
     * Detachs the HEAD. If HEAD is already detached, this is a no-op and successful. If HEAD points
     * to a commit, it will be detached at that commit. If it points to a tag, the tag will be
     * peeled into a commit and HEAD is detached at the commit. It is an error, if HEAD points
     * somewhere else.
     *
     * @param[in,out]   result  A Result object; see @ref GitWrapErrorHandling
     *
     * @return          `true` on success, `false` otherwise.
     *
     */
    bool Repository::detachHead(Result& result)
    {
        GW_D_CHECKED(Repository, false, result);

        result = git_repository_detach_head(d->mRepo);
        return result;
    }

}
