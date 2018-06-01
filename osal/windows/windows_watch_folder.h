#pragma once
/**
 * @file watch_folder.h
 *
 * Based on code originally developed for NDrive S.A.
 *
 * Copyright (c) 2010-2016 Neto Ranito & Seabra LDA. All rights reserved.
 *
 * This file is part of nrs-osal.
 *
 * nrs-osal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * nrs-osal is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with osal.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NRS_OSAL_WINDOWS_WINDOWS_WATCH_FOLDER_H
#define NRS_OSAL_WINDOWS_WINDOWS_WATCH_FOLDER_H

#include "osal/worker.h"
#include <windows.h>

namespace osal {
    namespace windows {

        class WatchFolder: protected osal::Worker
        {
            WCHAR*              directory_;
            bool                abort_;
            bool                watch_subtree_;
            HANDLE              change_handles_[1];

        public:
            WatchFolder (const char* a_directory);
            virtual ~WatchFolder ();

            virtual void StartWatchFolder (bool a_watch_subtree) ;
            virtual void StopWatchFolder () ;

            virtual void FolderChanged      () = 0;
            virtual void WatchFolderFailed  () = 0;

        private:
            virtual void  WorkerFunction       ();
        };

    } // windows
} // osal

#endif // NRS_OSAL_WINDOWS_WINDOWS_WATCH_FOLDER_H
