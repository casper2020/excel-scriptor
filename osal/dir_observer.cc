/**
 * @file dir_observer.cc - Helper to monitor directories
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

#include "osal/dir_observer.h"
#include "osal/osal_file.h"
#include "osal/osalite.h"

const uint32_t osal::DirObserver::k_check_delay_ = 10*1000; // 10s

osal::DirObserver::DirObserver ()
{

}

osal::DirObserver::~DirObserver ()
{

}

bool osal::DirObserver::Observe(DirMap* a_dir_map, Callback* a_callback, volatile bool* a_abort_flag)
{
    // invalid params?
    if ( a_dir_map == NULL || a_dir_map->size() == 0 || a_abort_flag == NULL ) {
        return false;
    }
    // keep checking every
    while ( (*a_abort_flag) == false ) {
        //
        for ( DirMap::iterator it = a_dir_map->begin(); it != a_dir_map->end(); ++it ) {
            // aborted?
            if ( (*a_abort_flag) == true ) {
                break;
            }
            // modified?
            int32_t last_modification_time = 0;
            if ( osal::File::GetLastModificationTime(it->first.c_str(), &last_modification_time) == osal::File::EStatusOk ) {
                // changed?
                if ( it->second != last_modification_time ) {
                    // notify listener
                    if ( a_callback->OnDirChanged(it->first.c_str()) == false ) {
                        // aborted by the owner of this object
                        return false;
                    }
                    // keep track of this time
                    (*a_dir_map)[it->first] = last_modification_time;
                }
            }
        }
        // delay next check
        OsalSleepMs(k_check_delay_);
    }
    //
    return true;
}
