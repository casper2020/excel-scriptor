#pragma once
/**
 * @file formula.h declaration of formula Holder object
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
#ifndef NRS_CASPER_CASPER_SEE_FORMULA_H
#define NRS_CASPER_CASPER_SEE_FORMULA_H

#include "casper/term.h"

#include <string>
#include <set>

namespace casper
{
    namespace see
    {
        class See;

        /**
         * @brief Holder object for each formula
         */
        class Formula
        {
            friend class Parser;
            friend class See;
            friend class SumIfs;

        public: // data

            virtual bool   IsSum                 () const;
            virtual bool   IsSumIfs                () const;

        protected: // methods

            virtual void   CalculateDependencies (See& a_see);
            virtual double SumAllTerms           (SymbolTable& a_sym_tab, FILE* a_logfile);
            virtual double SumIfAllTerms         (SymbolTable& a_symtab, SymbolTable& a_criterias, FILE* a_logfile);
            virtual Term   VLOOKUP               (const Term& a_value, const char* const a_lookup_col, const Term& a_result_index, bool a_range_lookup);
            std::string name_;         //!< Name of the variable that holds the formula result
            std::string alias_;        //!< Alias of the formula name, i.e. the excel cell reference
            StringSet   precedents_;   //!< List of variables the formala depends upon
            std::string formula_;      //!< The expression to calculate
            std::string description_;  //!< Human readable descripton of the formula
            std::string type_;         //!<


            Formula ();
            virtual ~Formula ();

        public:

            const std::string& Name () const;
            const std::string& Formulas () const;
            StringSet getPrecedents();
            std::string getFormula();

        };

        inline StringSet Formula::getPrecedents()
        {
            return precedents_;
        }
        
        inline std::string Formula::getFormula()
        {
            return formula_;
        }
        
        inline bool Formula::IsSum () const
        {
            return false;
        }
        inline bool Formula::IsSumIfs () const
        {
            return false;
        }

        inline const std::string& Formula::Name () const
        {
            return name_;
        }

        inline const std::string& Formula::Formulas() const
        {
            return formula_;
        }

    } // namespace see
} // namespace casper

#endif // NRS_CASPER_CASPER_SEE_FORMULA_H
