/**
 * @file sum.h declaration of Sum formula
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
#ifndef NRS_CASPER_CASPER_SEE_SUM_H
#define NRS_CASPER_CASPER_SEE_SUM_H

#include "casper/see/formula.h"
#include "casper/term.h"

#include <string>

namespace casper
{
    namespace see
    {
        class See;

        /**
         * @brief Specialization of formula to sum a range of cells
         *
         */
        class Sum : public Formula
        {
        public: // Attributes

            std::string start_cellref_;
            std::string end_cellref_;
            int32_t     start_row_;
            int32_t     start_col_;
            int32_t     end_row_;
            int32_t     end_col_;

        protected: // Methods

            virtual void   CalculateDependencies (See& a_see);
            virtual double SumAllTerms           (SymbolTable& a_sym_tab, FILE* a_logfile);
            virtual bool   IsSum                 () const;
            virtual bool   IsSumIfs              () const;
                    void   ExpandCellRefs        (See& a_aliases);

        public: // Static methods

            static void ParseTableColRef (const char* a_value, std::string* o_table_name, std::string* o_col_name);
            static bool ParseCellRef     (const char* a_value, int32_t* o_col, int32_t* o_row);
            static void MakeRowColRef    (char o_cellref[20], int a_row, int a_col);

        public: // Methods

                     Sum (const char* a_start_cellref, const char* a_end_cellref);
                     Sum (int a_row, int a_col, int a_row_count);
            virtual ~Sum ();

        };

        inline bool Sum::IsSum () const
        {
            return true;
        }
        inline bool Sum::IsSumIfs () const
        {
            return false;
        }

    } // namespace see
} // namespace casper

#endif // NRS_CASPER_CASPER_SEE_SUM_H
