/**
 * @file windows_watch_folder.cc - windows
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

#include "osal/windows/windows_watch_folder.h"
#include "osal/windows/windows_file.h"
#include "osal/debug_trace.h"

using osal::windows::WatchFolder;

WatchFolder::WatchFolder (const char* a_directory) : osal::Worker("WatchFolder")
{
	abort_			= false;
	watch_subtree_	= false;
	directory_		= File::Utf8ToWstr(a_directory);
}

WatchFolder::~WatchFolder ()
{
	if (directory_ != NULL)
	{
		delete [] directory_;
		directory_ = NULL;
	}
}

void WatchFolder::StartWatchFolder (bool a_watch_subtree)
{
	abort_			= false;
	watch_subtree_	= a_watch_subtree;

	Worker::StartWorkerThread();

}
void WatchFolder::StopWatchFolder ()
{
	abort_ = true;
	FindCloseChangeNotification(change_handles_[0]);
}


void  WatchFolder::WorkerFunction ()
{
	DWORD dwWaitStatus;

	if ( directory_ != NULL)
	{
		// Watch the directory for file creation and deletion.
		change_handles_[0] = FindFirstChangeNotification(
			directory_,                         // directory to watch
			watch_subtree_,                         // do not watch subtree
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME ); // watch file name changes

		if (change_handles_[0] == INVALID_HANDLE_VALUE)
		{
			DEBUGTRACEX("watch_folder", "WatchFolderFailed:  (GetLastError()=%d)\n", GetLastError());
			WatchFolderFailed();
			return ;
		}

		// Make a final validation check on our handles.
		if ( change_handles_[0] == NULL )
		{
			DEBUGTRACEX("watch_folder", "WatchFolderFailed:  (GetLastError()=%d)\n", GetLastError());
			WatchFolderFailed();
			return ;
		}

		// Change notification is set. Now wait on both notification
		// handles and refresh accordingly.

		while (abort_ == false)
		{
			// Wait for notification.

			dwWaitStatus = WaitForMultipleObjects(1, change_handles_, FALSE, INFINITE);

			switch (dwWaitStatus)
			{
			case WAIT_OBJECT_0:

				// A file was created, renamed, or deleted in the directory.
				// Refresh this directory and restart the notification.
				if (abort_ == false )
				{
					FolderChanged();
					if ( FindNextChangeNotification(change_handles_[0]) == FALSE )
					{
						DEBUGTRACEX("watch_folder", "WatchFolderFailed:  (GetLastError()=%d)\n", GetLastError());
						WatchFolderFailed();
					}
				}
				break;

			default:
				DEBUGTRACEX("watch_folder", "WatchFolderFailed:  (GetLastError()=%d)\n", GetLastError());
				WatchFolderFailed();
				break;
			}
		}

	}
}


#if 0
// Teste only
void WatchFolder::FolderChanged		()
{
	printf("forder changed\n");
}

void WatchFolder::WatchFolderFailed	()
{

	printf("WatchFolderFailed\n");
}

#endif