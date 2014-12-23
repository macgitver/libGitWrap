#ifndef GITWRAP_REFLOGENTRY_HPP
#define GITWRAP_REFLOGENTRY_HPP

#include "libGitWrap/Private/BasePrivate.hpp"

namespace Git
{
    namespace Internal
    {

        /**
         * @internal
         * @ingroup     GitWrap
         *
         * @brief       Internal wrapper for git_reflog_entry from libgit2.
         */
        class RefLogEntryPrivate : public BasePrivate
        {
        public:
            explicit RefLogEntryPrivate(const git_reflog_entry *entry);
            ~RefLogEntryPrivate();

        public:
            const git_reflog_entry *    mEntry;
        };
    }
}

#endif // REFLOGENTRY_HPP

