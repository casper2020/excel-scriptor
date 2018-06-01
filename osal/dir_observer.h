#pragma once
/**
 * @file dir_observer.h - Helper to monitor directories
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

#ifndef NRS_OSAL_DIR_OBSERVER_H
#define NRS_OSAL_DIR_OBSERVER_H

#include <map>
#include <string>

namespace osal {

    class DirObserver {

    public: //

        class Callback {

        public: // constructor(s) / destructor

            virtual ~Callback ()
            {

            }

        public: // pure virtual method(s) / function(s) declaration

            virtual bool OnDirChanged (const char* a_name) = 0;

        };

    public: //

        typedef std::map<std::string, int32_t> DirMap;

    private: // static data

        static const uint32_t k_check_delay_;

    public: // constructor / destructor

        DirObserver ();
        virtual ~DirObserver ();

    public: // inherited virtual method(s) / function(s)

        virtual bool Observe (DirMap* a_dir_map, Callback* a_callback, volatile bool* a_abort_flag);

    }; // end of class


} // end of namespace osal

#endif // NRS_OSAL_DIR_OBSERVER_H
