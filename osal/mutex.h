#pragma once
/**
 * @file mutex.h - Header mux that pulls the mutex implementation for each platform
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

#ifndef NRS_OSAL_MUTEX_H_
#define NRS_OSAL_MUTEX_H_

#if 0 // def // _WIN32 MARTELADA_TO_REMOVE_WIN32
    #include "osal/windows/windows_mutex.h"s
    namespace osal
    {
        typedef osal::windows::Mutex Mutex;
    }
#else
    #include "osal/posix/posix_mutex.h"
    namespace osal
    {
        typedef osal::posix::Mutex Mutex;
    }
#endif

#endif // NRS_OSAL_MUTEX_H_

// endof $Id: $
