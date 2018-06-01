/**
 * @file vlookup.h Specialization of Formula for VLOOKUP
 *
 * Copyright (c) 2010-2016 Neto Ranito & Seabra LDA. All rights reserved.
 *
 * This file is part of casper.
 *
 * casper is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * casper  is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with casper.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#ifndef NRS_CASPER_CASPER_SEE_VLOOKUP_H
#define NRS_CASPER_CASPER_SEE_VLOOKUP_H

#include "casper/see/formula.h"
#include "casper/see/see.h"

#include <vector>
#include <string>

namespace casper
{

    namespace see
    {
        /**
         * @brief Specialization of Formula for VLOOKUP
         */
        class Vlookup : public Formula
        {

        protected: // Refs

            See&       see_;

        protected: // Const Data

            const std::string search_column_name_;
            const int         result_column_index_;

        public: // Constructor(s) / Destructor
            
            Vlookup (See& a_see,
                     const char* const a_search_column_name, const int& a_result_column_index);
            virtual ~Vlookup ();

        public: // Inherited virtual method(s) / function(s)

            virtual void CalculateDependencies (See& a_see);
            virtual Term VLOOKUP               (const Term& a_value, const char* const a_lookup_col, const Term& a_result_index, bool a_range_lookup);

        protected: // Method(s) / Function(s)

            void    SeekColumnsInfo            (const char* const a_search_column_name, const int& a_result_column_index,
                                                ColumnInfo& o_search_column_info, ColumnInfo& o_result_column_info);

        }; // end of class LinesVLookup

    } // end of namespace see

} // end of namespace casper

#endif // NRS_CASPER_CASPER_SEE_VLOOKUP_H
