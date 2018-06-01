/**
 * @file sum.rl Implementation of Sum formula
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

#include "casper/see/sum.h"
#include "casper/see/see.h"
#include "osal/osalite.h"
#include "osal/debug_trace.h"

/**
 * @brief Constructor
 */
casper::see::Sum::Sum (int a_row, int a_col, int a_row_count)
{
    start_row_ = a_row;
    start_col_ = a_col;
    end_row_   = a_row + a_row_count;
    end_col_   = a_col;
}

/**
 * @brief Constructor
 */
casper::see::Sum::Sum (const char* a_start_cellref, const char* a_end_cellref)
{
    start_cellref_ = a_start_cellref;
    end_cellref_   = a_end_cellref;
    start_row_     = -1;
    start_col_     = -1;
    end_row_       = -1;
    end_col_       = -1;
}

void casper::see::Sum::ExpandCellRefs (See& a_see)
{
    if ( ParseCellRef(start_cellref_.c_str(), &start_col_, &start_row_) == false ) {
        StringHash::iterator it;

        it = a_see.name_to_cell_aliases_.find(start_cellref_);
        if ( it == a_see.name_to_cell_aliases_.end() ) {
            throw OSAL_EXCEPTION("Unable to resolve '%s' to a valid cell ref\n", start_cellref_.c_str());
        }
        if ( ParseCellRef(it->second.c_str(), &start_col_, &start_row_) == false ) {
            throw OSAL_EXCEPTION("Unable to resolve '%s' aliased to '%s' to a valid cell ref\n", start_cellref_.c_str(), it->second.c_str());
        }
    }
    if ( ParseCellRef(end_cellref_.c_str(), &end_col_, &end_row_) == false ) {
        StringHash::iterator it;

        it = a_see.name_to_cell_aliases_.find(end_cellref_);
        if ( it == a_see.name_to_cell_aliases_.end() ) {
            throw OSAL_EXCEPTION("Unable to resolve '%s' to a valid cell ref\n", end_cellref_.c_str());
        }
        if ( ParseCellRef(it->second.c_str(), &end_col_, &end_row_) == false ) {
            throw OSAL_EXCEPTION("Unable to resolve '%s' aliased to '%s' to a valid cell ref\n", end_cellref_.c_str(), it->second.c_str());
        }
    }

    if ( end_col_ < start_col_ ) {
        int32_t tmp = start_col_;

        start_col_ = end_col_;
        end_col_   = tmp;
    }
    if ( end_row_ < start_row_ ) {
        int32_t tmp = start_row_;

        start_row_ = end_row_;
        end_row_   = tmp;
    }
}

/**
 * @brief Destructor
 */
casper::see::Sum::~Sum ()
{
    /* Empty */
}

void casper::see::Sum::CalculateDependencies (See& a_see)
{
    StringHash::iterator  ait;
    SymbolTable::iterator sit;
    char cell_ref[20];

    precedents_.clear();
    if ( -1 == start_row_ ) {
        ExpandCellRefs(a_see);
    }

    for ( int32_t c = start_col_; c <= end_col_; ++c ) {
        for ( int32_t r = start_row_; r <= end_row_; ++r ) {
            MakeRowColRef(cell_ref, r, c);

            sit = a_see.symtab_.find(cell_ref);
            if ( sit != a_see.symtab_.end() ) {
                precedents_.insert(sit->first);
            }

            ait = a_see.aliases_.find(cell_ref);
            if ( ait != a_see.aliases_.end() ) {
                precedents_.insert(ait->second);
            }
        }
    }
}

void casper::see::Sum::MakeRowColRef (char o_cellref[20], int a_row, int a_col)
{
    int    q;
    int    i;
    char   rev[5];

    q = a_col;
    i = 0;
    while ( q != 0 && i < (int)(sizeof(rev) - 1) ) {
        rev[i++] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[(q - 1) % 26];
        q        = (q - 1) / 26;
    }

    char* p = rev + i;
    do {
        --p;
        *(o_cellref) = *p;
        ++o_cellref;
    } while (p != rev);

    if ( a_row != -1 ) {
        snprintf(o_cellref, 20 - i, "%d", a_row);
    } else {
        *(o_cellref) = 0;
    }
}

bool casper::see::Sum::ParseCellRef (const char* a_value, int32_t* o_col, int32_t* o_row)
{
    bool    rv;
    int     cs;
    char*   p   = (char*) a_value;
    char*   pe  = (char*) a_value + strlen(a_value);
    int32_t row = 0;
    int32_t col = 0;

    %%{
        machine cell_ref;

        action update_col
        {
            col *= 26; col += islower(fc) ? (fc - 'a') + 1 : (fc - 'A') +1;
        }

        action update_row
        {
            row *= 10; row += (fc - '0');
        }

        main := [a-zA-Z]+ $update_col [0-9]+ $update_row;

        write data;
        write init;
        write exec;
    }%%

    if ( cs < cell_ref_first_final ) {
        *o_col = 0;
        *o_row = 0;
        rv     = false;
    } else {
        *o_col = col;
        *o_row = row;
        rv     = true;
    }
    OSAL_UNUSED_PARAM(cell_ref_error);
    OSAL_UNUSED_PARAM(cell_ref_en_main);
    return rv;
}

void casper::see::Sum::ParseTableColRef (const char* a_value, std::string* o_table_name, std::string* o_col_name)
{
    int     cs;
    char*   p        = (char*) a_value;
    char*   pe       = (char*) a_value + strlen(a_value);
    char*   eof      = pe;
    char*   name     = (char*) a_value;
    int     name_len = 0;
    char*   col      = NULL;
    int     col_len  = 0;

    %%{
        machine cell_table_col_ref;

        table_name = [^\[]+ %{ name_len = (int)(fpc - name); col_len = 0; col = (char*) ""; };

        table_and_column = [^\[]+ %{ name_len = (int)(fpc - name); } '[' %{ col = fpc; } [^\]]+ %{ col_len = (int) (fpc - col); } ']';

        main := table_name | table_and_column;

        write data;
        write init;
        write exec;
    }%%

    if ( cs >= cell_table_col_ref_first_final ) {
        *(o_table_name) = std::string(name, name_len);
        *(o_col_name)   = std::string(col, col_len);
    }
    OSAL_UNUSED_PARAM(cell_table_col_ref_error);
    OSAL_UNUSED_PARAM(cell_table_col_ref_en_main);
}

double casper::see::Sum::SumAllTerms (SymbolTable& a_symtab, FILE* a_logfile)
{
    double sum;

    sum = 0;
    for ( StringSet::iterator it = precedents_.begin(); it != precedents_.end(); ++it ) {
        DEBUGTRACE("see-calc-sum", " += %-30.30s ....... %g", it->c_str(), a_symtab[*it].number_);
        if ( a_logfile != NULL ) {
            fprintf(a_logfile, " += %-30.30s ....... %g\n", it->c_str(), a_symtab[*it].number_);
        }
        sum += a_symtab[*it].ConvertToNumber();
    }
    return sum;
}

