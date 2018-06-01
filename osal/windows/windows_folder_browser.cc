/**
 * @file windows_folder_browser.cc - windows
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

#include "osal/windows/windows_folder_browser.h"
#include "osal/windows/windows_dir.h"
#include "osal/debug_trace.h"


using osal::windows::FolderBrowser;

FolderBrowser::FolderBrowser ()
{
	abort_			= false;
}

FolderBrowser::~FolderBrowser ()
{
	Clean();
}

void FolderBrowser::Clean ()
{

}

bool FolderBrowser::BrowseFolder (const char* a_directory, bool a_browse_subtree)
{
	return BrowseFolder(a_directory, "*",a_browse_subtree);
}


bool FolderBrowser::BrowseFolder (const char* a_directory, const char* a_pattern, bool a_browse_subtree)
{
	abort_			= false;
	Clean();
	if ( Dir::Exists(a_directory) == osal::windows::Dir::EStatusDoesNotExist ) {
		return false;
	}

	bool rv =  BrowseFolderA(a_directory, a_pattern, a_browse_subtree);

	return rv;
}

bool FolderBrowser::BrowseFolderWCHAR (const char* a_directory, const char* a_pattern, bool a_browse_subtree)
{
	WCHAR* w_pattern = NULL;

	abort_			= false;
	Clean();

	if ( Dir::Exists(a_directory) == osal::windows::Dir::EStatusDoesNotExist ) {
		return false;
	}

	WCHAR w_directory[MAX_PATH];
	File::Utf8ToWstr(a_directory, w_directory);
	if (w_directory == NULL) {
		return false;
	}
	w_pattern = osal::windows::File::Utf8ToWstr(a_pattern);
	if (a_pattern == NULL) {
		return false;
	}

	bool rv =  BrowseFolderW(w_directory, w_pattern, a_browse_subtree);

	delete w_pattern;
	return rv;
}



bool FolderBrowser::BrowseFolderW (const WCHAR* a_directory, const WCHAR* a_pattern, bool a_browse_subtree)
{
	FileInfoW file_info;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WCHAR files_to_search[MAX_PATH*2];
	AppendFileName(a_pattern, a_directory, files_to_search);

	// Find the first file in the directory.
	hFind = FindFirstFileW(files_to_search, &file_info.data_);
	if (INVALID_HANDLE_VALUE != hFind)
	{

		// List all the files in the directory with some info about them.
		do
		{
			if (file_info.data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//skip
			}
			else
			{
				file_info.BuildData(a_directory);
				FileCallBack(file_info);
			}
		}
		while (abort_ == false && FindNextFileW(hFind, &file_info.data_) != 0);

		DWORD dwError=0;
		dwError = GetLastError();
		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	// look into other directories
	if (a_browse_subtree)
	{
		AppendFileName(L"*", a_directory, files_to_search);

		// Find the first file in the directory.
		hFind = FindFirstFileW(files_to_search, &file_info.data_);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return false;
		}

		// List all the files in the directory with some info about them.

		do
		{
			if (file_info.data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{


				if ( wcscmp(file_info.data_.cFileName,L".") != 0 && wcscmp(file_info.data_.cFileName,L"..") != 0 ) {
					WCHAR tmp[MAX_PATH];
					AppendFileName(file_info.data_.cFileName, a_directory, tmp);
					BrowseFolderW(tmp, a_pattern, a_browse_subtree);
				}
			}
			else
			{
				//skip

			}
		}
		while (abort_ == false && FindNextFileW(hFind, &file_info.data_) != 0);

		DWORD dwError=0;
		dwError = GetLastError();
		FindClose(hFind);
	}

	return true;
}



bool FolderBrowser::BrowseFolderA (const char* a_directory, const char* a_pattern, bool a_browse_subtree)
{
	FileInfoA file_info;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char files_to_search[MAX_PATH*2];
	AppendFileName(a_pattern, a_directory, files_to_search);

	// Find the first file in the directory.
	hFind = FindFirstFileA(files_to_search, &file_info.data_);
	if (INVALID_HANDLE_VALUE != hFind)
	{

		// List all the files in the directory with some info about them.
		do
		{
			if (file_info.data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//skip
			}
			else
			{
				file_info.BuildData(a_directory);
				FileCallBack(file_info);
			}
		}
		while (abort_ == false && FindNextFileA(hFind, &file_info.data_) != 0);

		DWORD dwError=0;
		dwError = GetLastError();
		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	// look into other directories
	if (a_browse_subtree)
	{
		AppendFileName("*", a_directory, files_to_search);

		// Find the first file in the directory.
		hFind = FindFirstFileA(files_to_search, &file_info.data_);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return false;
		}

		// List all the files in the directory with some info about them.

		do
		{
			if (file_info.data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{


				if ( strcmp(file_info.data_.cFileName,".") != 0 && strcmp(file_info.data_.cFileName,"..") != 0 ) {
					char tmp[MAX_PATH];
					AppendFileName(file_info.data_.cFileName, a_directory, tmp);
					BrowseFolderA(tmp, a_pattern, a_browse_subtree);
				}
			}
			else
			{
				//skip

			}
		}
		while (abort_ == false && FindNextFileA(hFind, &file_info.data_) != 0);

		DWORD dwError=0;
		dwError = GetLastError();
		FindClose(hFind);
	}

	return true;
}



void FolderBrowser::Abort ()
{
	abort_ = true;
}

#if 0 // Teste only
void FolderBrowser::FileCallBack (const osal::windows::FileInfo& a_file_info)
{
	printf("%s -> %s  : size=%u time=%u\n", a_file_info.full_file_name(), a_file_info.short_file_name(), a_file_info.size(), a_file_info.access_time()  );
}
#endif

const WCHAR* FolderBrowser::AppendFileName (const WCHAR* a_file_name, const WCHAR* a_path, WCHAR* o_out)
{
	o_out[0] = 0;
	wcscat(o_out,a_path);
	int tmp_len = wcslen(o_out);
	if ( tmp_len>0 && o_out[tmp_len-1]!=L'\\' && o_out[tmp_len-1]!=L'/'  )
	{
		wcscat(o_out,L"\\");
	}
	wcscat(o_out,a_file_name);
	return o_out;
}

const char* FolderBrowser::AppendFileName (const char* a_file_name, const char* a_path, char* o_out)
{
	o_out[0] = 0;
	strcat(o_out,a_path);
	int tmp_len = strlen(o_out);
	if ( tmp_len>0 && o_out[tmp_len-1]!='\\' && o_out[tmp_len-1]!='/'  )
	{
		strcat(o_out,"\\");
	}
	strcat(o_out,a_file_name);
	return o_out;
}

/// FileInfo ///

osal::windows::FileInfo::FileInfo ()
{
	full_file_name_[0] = 0;
	short_file_name_ = NULL;
}

osal::windows::FileInfo::~FileInfo ()
{

}

void osal::windows::FileInfo::BuildShortFileName ()
{
	int32_t s = strlen(full_file_name_);
	for (int32_t i = s; i > 0; i--)
	{
		if (full_file_name_[i] == '/' || full_file_name_[i] == '\\')
		{
			short_file_name_ = full_file_name_+i+1;
			break;
		}
	}
}

void osal::windows::FileInfoW::BuildData (const WCHAR* a_directory)
{
	WCHAR w_full_file_name[MAX_PATH];

	FolderBrowser::AppendFileName(data_.cFileName, a_directory, w_full_file_name);

	File::WstrToUtf8(w_full_file_name, full_file_name_);
	BuildShortFileName();
}

void osal::windows::FileInfoA::BuildData (const char* a_directory)
{
	FolderBrowser::AppendFileName(data_.cFileName, a_directory, full_file_name_);

	BuildShortFileName();
}
