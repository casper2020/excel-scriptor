/**
 * @file row_shifter.cc Helper to shift the rows of an excel expression
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

#include "casper/see/row_shifter.h"

#include <algorithm>  // std::sort

/**
 * @brief Constructor, start with an empty output buffer
 */
casper::see::RowShifter::RowShifter ()
{
    buffer_           = NULL;
    buffer_size_      = 0;
    clone_suffix_     = -1;
    grab_definitions_ = false;
}

/**
 * @brief Destructor, free the output buffer
 */
casper::see::RowShifter::~RowShifter ()
{
    patches_.clear();
    defined_variables_.clear();
    if ( buffer_ == NULL ) {
        free(buffer_);
        buffer_ = NULL;
    }
}

/**
 * @brief Parse the formula to catch variable definitions
 * 
 * The actual work is done in #SetVariable
 *
 * @param a_formula The formula to analyze
 */
void casper::see::RowShifter::GrabVariableDefinitions (const char* a_formula)
{
    grab_definitions_ = true;
    scanner_.SetInput(a_formula, strlen(a_formula));
    parser_.parse();
    grab_definitions_ = false;
}

/**
 * @brief Reset the accumlated variable definitions
 */
void casper::see::RowShifter::ClearVariableDefinitions ()
{
    defined_variables_.clear();
}

/**
 * @brief Extract the suffix from the clone name
 *
 * @param a_clone_name The named of the cloned code, should end in a dot followed by a number
 */
void casper::see::RowShifter::ParseSuffix (const char* a_clone_name)
{
    const char* dot_sep = strchr(a_clone_name, '.');
    
    if ( dot_sep == NULL || sscanf(dot_sep + 1, "%d", &clone_suffix_) != 1 ) {
        throw OSAL_EXCEPTION("Clone code '%s' is not valid, use XXXX . number format (e.g. A008.1)", a_clone_name);
    }
}

/**
 * @brief Helper to shift the rows of an excel expression
 *
 * @param a_formula the formula to copy and shift
 * @param a_row_shift the number of rows to add or subtract
 *
 * @return Pointer to the modified formula, don't free it!!!!!
 */
const char* casper::see::RowShifter::ShiftFormula (const char* a_formula, int a_row_shift)
{
    size_t len = strlen(a_formula);
    
    patches_.clear();
    grab_definitions_ = false;
    row_shift_ = a_row_shift;
    scanner_.SetInput(a_formula, len);
    parser_.parse();
    
    /*
     * Sort patches and account for the space diference for patched formula
     */
    std::sort(patches_.begin(), patches_.end(), PatchComparator);
    int delta = 0;
    for (PatchList::iterator it = patches_.begin(); it != patches_.end(); ++it) {
        delta += it->delta_;
    }
    
    /*
     * Lazily allocate space for the output
     */
    if ( buffer_size_ < len + delta + 1 ) {
        if ( buffer_ != NULL ) {
            free(buffer_);
        }
        buffer_size_ = len + delta + 4096; // allow for more 4k to reduce allocations
        buffer_ = (char*) malloc(buffer_size_);
        if ( buffer_ == NULL ) {
            throw OSAL_EXCEPTION_NA("Out of memory in formula row shifter");
        }
    }
    
    /*
     * Copy and replace loop
     */
    const char* src = a_formula;
    char*       dst = buffer_;
    
    for (PatchList::iterator it = patches_.begin(); it != patches_.end(); ++it) {
        size_t chunk_length = it->start_ - (src - a_formula);
        memcpy(dst, src, chunk_length);
        dst += chunk_length;
        memcpy(dst, it->replacement_.c_str(), it->replacement_.size());
        dst += it->replacement_.size();
        src = a_formula + it->end_ + 1;
    }
    if ( *src ) {
        strcpy(dst, src); // Traling string
    } else {
        *dst = 0;         // Terminate the copy
    }
    return buffer_;
}

/**
 * @brief Helper to sort patches from expression start to expression end
 *
 * @param a_patch_a The first item to compare
 * @param a_patch_b The second item to compare
 *
 * @return true if item A preceeds item B
 */
bool casper::see::RowShifter::PatchComparator (const RowPatch& a_patch_a, const RowPatch& a_patch_b)
{
    return a_patch_a.start_ < a_patch_b.start_;
}

/**
 * @brief Intercept access to variables, if the variable is a cellreference change
 *
 * Change location and name are pushed to the patch list
 *
 * @param a_varname  Name of the variable being accessed
 * @param a_location Location of the variable whitin the expression
 */
void casper::see::RowShifter::GetVariable (Term&,  Term& a_varname, casper::see::location& a_location)
{
    int32_t col;
    int32_t row;
    char    cell_ref[20];
    
    if ( Sum::ParseCellRef(a_varname.text_.c_str(), &col, &row) ) {
        row += row_shift_;
        Sum::MakeRowColRef(cell_ref, row, col);
        patches_.push_back(RowPatch(a_location.begin.column, a_location.end.column, cell_ref));
    } else {
        if ( defined_variables_.find(a_varname.text_) != defined_variables_.end() ) {
            char sz_temp[4096];
            
            if ( clone_suffix_ == -1 ) {
                throw OSAL_EXCEPTION_NA("Fatal unknown clone suffix");
            }
            snprintf(sz_temp, sizeof(sz_temp), "%s_C%d", a_varname.text_.c_str(), clone_suffix_);
            patches_.push_back(RowPatch(a_location.begin.column, a_location.end.column, sz_temp));
        }
    }
}

/**
 * @brief Intercept setting of a variable, i.e. VAR=XXX
 *
 * @param a_varname  Name of the variable being accessed
 * @param a_location Location of the variable whitin the expression
 */
void casper::see::RowShifter::SetVariable (Term& a_varname, Term&, casper::see::location& a_location)
{
    int32_t col;
    int32_t row;

    if ( grab_definitions_ == true ) {
        if ( Sum::ParseCellRef(a_varname.text_.c_str(), &col, &row) ) {
            return; // it's a cell ref, row will be shifted
        } else {
            defined_variables_.insert(a_varname.text_);
        }
    } else {
        // Delete replacement to GetVariables
        GetVariable(a_varname, a_varname, a_location);
    }
}

/********************************************************************************************************/
/*                                                                                                      */
/*                   Dummy functions to neutrailze the normal parsing side effects                      */
/*                                                                                                      */
/********************************************************************************************************/

void casper::see::RowShifter::GetLinesTableValue (Term&, Term&)
{
    /* dummy */
}

void casper::see::RowShifter::TableHeaderMatch (Term&,  Term&, Term&)
{
    /* dummy */
}

void casper::see::RowShifter::Offset (Term&, const Term&, const Term&, const Term&)
{
    /* dummy */
}

void casper::see::RowShifter::Lookup (Term&, Term&, Term&, Term&)
{
    /* dummy */
}

void casper::see::RowShifter::Vlookup (Term&, Term&, Term&, Term&, bool, const char* const, const size_t&)
{
    /* dummy */
}

void casper::see::RowShifter::SumIfs (Term&, Term&)
{
    /* dummy */
}

void casper::see::RowShifter::Sum (Term&,  Term&)
{
    /* dummy */
}

void casper::see::RowShifter::Sum (Term&,  Term&, Term&)
{
    /* dummy */
}

void casper::see::RowShifter::SumIfOnLinesTable (Term&, const char*, const Term&,
                                                 const char* const, const size_t&)
{
    /* dummy */
}

void casper::see::RowShifter::SumIfsOnLinesTable (Term&, const char*, SymbolTable&)
{
    /* dummy */
}
