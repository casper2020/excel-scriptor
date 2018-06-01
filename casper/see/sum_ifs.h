/**
 * @file sum_ifs.h Specialization of Sum for conditional sums
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
#ifndef NRS_CASPER_CASPER_SEE_SUM_IFS_H
#define NRS_CASPER_CASPER_SEE_SUM_IFS_H

#include "casper/see/formula.h"

#include <vector>
#include <string>

namespace casper
{
    namespace see
    {
        /**
         * @brief Specialization of Formula for condtional sums
         */
        class SumIfs : public Formula
        {
        protected:

            std::string                            sum_col_;
            std::vector<std::string>               col_names_;
            std::vector<std::vector<std::string> > sum_rows_;

        public: // Methods

                           SumIfs                (const char* a_sum_col, SymbolTable& a_criterias);
            virtual        ~SumIfs               ();
            virtual void   CalculateDependencies (See& a_see);
            virtual double SumIfAllTerms         (SymbolTable& a_symtab, SymbolTable& a_criterias, FILE* a_logfile);
            virtual bool   IsSum                 () const;
            virtual bool   IsSumIfs              () const;
        };

        inline bool SumIfs::IsSum () const
        {
            return false;
        }
        inline bool SumIfs::IsSumIfs () const
        {
            return true;
        }

    } // namespace see
} // namespace casper

#endif // NRS_CASPER_CASPER_SEE_SUM_IFS_H
