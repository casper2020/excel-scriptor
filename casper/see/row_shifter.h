/**
 * @file row_shifter.h Helper to shift the rows of an excel expression
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
#ifndef NRS_CASPER_CASPER_SEE_ROW_SHIFTER_H
#define NRS_CASPER_CASPER_SEE_ROW_SHIFTER_H

#include "casper/see/see.h"
namespace casper
{
    namespace see
    {
        /**
         * @brief Utility that parses an Excel formula and shifts the row references
         *
         * This class works by parsing the excel expression, so it extends the normal
         * #casper::see::See neutralizing all the parser side effects with overridden methods.
         * Specializations of GetVariable and SetVariable handle the cell reference patching.
         */
        class RowShifter : public See
        {
            struct RowPatch
            {
                int         start_;        //!< Offset of the first replaced char (zero based)
                int         end_;          //!< Offset of the last replaced char (zero based)
                int         delta_;        //!< Difference in length introduced by the patch
                std::string replacement_;  //!< The new text that will replace the patched substring
                
                RowPatch (int a_start, int a_end, const char* a_replacement)
                {
                    start_       = a_start;
                    end_         = a_end;
                    replacement_ = a_replacement;
                    delta_       = (int) strlen(a_replacement) - (a_start - a_end + 1);
                }
            };

            typedef std::vector<RowPatch> PatchList;
            
        public: // Attributes
            
            bool      grab_definitions_;  //!< true to catch variable definitions, false for shifting
            StringSet defined_variables_; //!< List of variables defined in the cloned row set
            char*     buffer_;            //!< Output buffer holds the modified string
            size_t    buffer_size_;       //!< Current size of the output buffer
            PatchList patches_;           //!< Array with patched segments
            int       row_shift_;         //!< Offset to add to rows when pre-parsing
            int       clone_suffix_;      //!< Suffix, eg. 1 for A008.1, 2 for A008.2

        protected: // Methods
            
            static bool PatchComparator (const RowPatch& a_patch_a, const RowPatch& a_patch_b);

        public: // Methods

                    RowShifter  ();
            virtual ~RowShifter ();

            const char* ShiftFormula             (const char* a_formula, int a_row_shift);
            void        GrabVariableDefinitions  (const char* a_formula);
            void        ClearVariableDefinitions ();
            void        ParseSuffix              (const char* a_clone_name);

            /*
             * Overrides to customize the parser actions and neutralize the normal side-effects
             */
            virtual void GetVariable        (Term& a_result,  Term& a_varname, casper::see::location& a_location);
            virtual void SetVariable        (Term& a_varname, Term& a_value,   casper::see::location& a_location);
            virtual void GetLinesTableValue (Term& a_result,  Term& a_column_name);
            virtual void TableHeaderMatch   (Term& a_result,  Term& a_colname, Term& a_tablename);
            virtual void Offset             (Term& o_result,  const Term& a_ref, const Term& a_rows, const Term& a_cols);
            virtual void Lookup             (Term& a_result,  Term& a_value, Term& a_lookup_vector, Term& a_result_vector);
            virtual void Vlookup            (Term& a_result,  Term& a_value, Term& a_lookup_vector, Term& a_col_index, bool a_range_lookup,
                                             const char* const a_formula, const size_t& a_formula_length);
            virtual void SumIfs             (Term& a_result,  Term& a_sum_range);
            virtual void Sum                (Term& a_result,  Term& a_vector_ref);
            virtual void Sum                (Term& a_result,  Term& a_cell_start, Term& a_cell_end);
            virtual void SumIfOnLinesTable  (Term& a_result,  const char* a_sum_column, const Term& a_criteria,
                                             const char* const, const size_t&);
            virtual void SumIfsOnLinesTable (Term& a_result,  const char* a_sum_column, SymbolTable& a_criterias);
        };

    } // namespace see
} // namespace casper

#endif // NRS_CASPER_CASPER_SEE_ROW_SHIFTER_H
