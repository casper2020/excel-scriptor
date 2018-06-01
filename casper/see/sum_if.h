/**
 * @file sum_if.h Specialization of Sum for conditional sum
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
#ifndef NRS_CASPER_CASPER_SEE_SUM_IF_H
#define NRS_CASPER_CASPER_SEE_SUM_IF_H

#include "casper/see/formula.h"

#include <vector>
#include <string>

namespace casper
{
    namespace see
    {
        /**
         * @brief Specialization of Formula for conditional sum
         */
        class SumIf : public Formula
        {
        protected:
            
            std::string                            sum_col_;
            std::vector<std::vector<std::string> > sum_rows_;
            std::string                            range_col_;
            std::vector<std::string>               range_cells_;
            
        public: // Methods

                            SumIf                (const char* const a_sum_col, const char* const a_range_col);
            virtual        ~SumIf                ();
            virtual void   CalculateDependencies (See& a_see);
            virtual double SumIfAllTerms         (SymbolTable& a_symtab, SymbolTable& a_criterias, FILE* a_logfile);
        };

    } // namespace see
} // namespace casper

#endif // NRS_CASPER_CASPER_SEE_SUM_IF_H
