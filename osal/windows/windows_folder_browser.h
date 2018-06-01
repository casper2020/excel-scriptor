#pragma once
/**
 * @file windows_folder_browser.h
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

#ifndef NRS_OSAL_WINDOWS_WINDOWS_FOLDER_BOWSER_H
#define NRS_OSAL_WINDOWS_WINDOWS_FOLDER_BOWSER_H

#include "osal/osal_types.h"
#include <windows.h>
#include "osal/windows/windows_time.h"

namespace osal {
    namespace windows {

        class FileInfo
        {
            friend class FolderBrowser;

        protected:
            const char* short_file_name_;
            char  full_file_name_[MAX_PATH];


            void BuildShortFileName ();
        public:
            FileInfo ();
            ~FileInfo ();

            virtual uint32_t    size () const = 0;
            virtual int32_t     access_time () const = 0;

            const char* FileInfo::full_file_name ()  const { return full_file_name_;}
            const char* short_file_name ()  const { return short_file_name_; }
        };

        class FileInfoW  : public FileInfo
        {
            friend class FolderBrowser;
        protected:
            WIN32_FIND_DATAW    data_;

            void BuildData (const WCHAR* a_directory);

        public:
            uint32_t size () const { return data_.nFileSizeLow; }

            int32_t access_time ()  const{ return osal::windows::Time::GetUTC(data_.ftLastAccessTime); }
        };

        class FileInfoA  : public FileInfo
        {
            friend class FolderBrowser;
        protected:
            WIN32_FIND_DATAA    data_;

            void BuildData (const char* a_directory);

        public:
            uint32_t size () const { return data_.nFileSizeLow; }

            int32_t access_time ()  const{ return osal::windows::Time::GetUTC(data_.ftLastAccessTime); }
        };



        class FolderBrowser
        {
            friend class FileInfo;
            friend class FileInfoW;
            friend class FileInfoA;

            bool    abort_;
        public:
            virtual bool BrowseFolder (const char* a_directory, const char* a_pattern, bool a_browse_subtree);
            bool         BrowseFolder (const char* a_directory, bool a_browse_subtree);

            virtual bool BrowseFolderWCHAR (const char* a_directory, const char* a_pattern, bool a_browse_subtree);

            FolderBrowser ();
            virtual ~FolderBrowser();
            void Abort ();

            virtual void FileCallBack (const FileInfo& a_file_info) = 0 ;

        protected:
            void Clean ();
            bool BrowseFolderW (const WCHAR* a_directory, const WCHAR* a_pattern, bool a_browse_subtree);
            static const WCHAR* AppendFileName (const WCHAR* a_file_name, const WCHAR* a_path, WCHAR* o_out);

            bool BrowseFolderA (const char* a_directory, const char* a_pattern, bool a_browse_subtree);
            static const char* AppendFileName (const char* a_file_name, const char* a_path, char* o_out);
        };

    } // windows
} // osal

#endif // NRS_OSAL_WINDOWS_WINDOWS_FOLDER_BOWSER_H
