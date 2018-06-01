/**
 * @file sum_if.cc Specialization of Sum for conditional sums
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

#include "casper/see/sum_if.h"
#include "casper/see/sum.h"
#include "casper/see/see.h"

/**
 * @brief Constructor
 */
casper::see::SumIf::SumIf (const char* const a_sum_col, const char* const a_range_col)
{
    sum_col_   = a_sum_col;
    range_col_ = a_range_col;
}

/**
 * @brief Destructor
 */
casper::see::SumIf::~SumIf ()
{
    /* empty */
}

void casper::see::SumIf::CalculateDependencies (See& a_see)
{
    std::vector<std::string> dummy_vec;
    StringHash::iterator     ait;
    SymbolTable::iterator    sit;
    char                     cell_ref[20];
    int                      c, start_row, end_row;
    size_t                   row_cnt;
    
    precedents_.clear();
    start_row = a_see.columns_.begin()->second.row_ + 1;
    end_row   = start_row + a_see.row_count_;
    row_cnt   = 0;
    sum_rows_.push_back(dummy_vec);

    /*
     * Make the formula depend on all sum range and criteria cols terms that are defined
     */
    for ( int32_t r = start_row; r <= end_row; ++r ) {

        cell_ref[0] = '\0';
        
        c = a_see.columns_.find(range_col_)->second.col_;
        Sum::MakeRowColRef(cell_ref, r, c);
        range_cells_.push_back(cell_ref);
        
        c = a_see.columns_.find(sum_col_)->second.col_;
        Sum::MakeRowColRef(cell_ref, r, c);

        const std::vector<std::string> columns = { cell_ref, range_col_.c_str() };
        for ( auto column : columns ) {
            ait = a_see.aliases_.find(column);
            if ( ait != a_see.aliases_.end() ) {
                precedents_.insert(ait->second);
                sum_rows_[row_cnt].push_back(ait->second);
            } else {
                sit = a_see.symtab_.find(column);
                if ( sit != a_see.symtab_.end() ) {
                    precedents_.insert(sit->first);
                    sum_rows_[row_cnt].push_back(sit->first);
                }
            }
        }

        if ( sum_rows_[row_cnt].size() == 1 ) {
            sum_rows_.push_back(dummy_vec);
            ++row_cnt;
        } else {
            sum_rows_[row_cnt].clear();
            range_cells_.pop_back();
        }
        
    }

    /*
     * Trim the last row if it's not complete
     */
    if ( sum_rows_[row_cnt].size() != 1 ) {
        sum_rows_.pop_back();
    }
}


double casper::see::SumIf::SumIfAllTerms (SymbolTable& a_symtab, SymbolTable& a_criterias, FILE* a_logfile)
{
    casper::Term criteria = a_criterias.begin()->second;
    casper::Term result   = casper::Term(casper::Term::EUndefined);
    double       sum      = 0.0;
    
    const char* const op  = criteria.aux_condition_.c_str();
    const size_t      ol  = strlen(op);
    const int         col = 0;
    
    for ( size_t row = 0; row < sum_rows_.size(); ++row ) {
        
        casper::Term  range_col_value = a_symtab[range_cells_[row]];
        casper::Term& cell_value      = a_symtab[sum_rows_[row][col]];
        
        result = false;
        
        switch (op[0]) {
            case '<':
            {
                if ( 1 == ol ) {
                    result.Less(cell_value, range_col_value);
                } else if ( 2 == ol && '=' == op[1] ) {
                    result.LessOrEqual(cell_value, range_col_value);
                }
                break;
            }
            case '>':
            {
                if ( 1 == ol ) {
                    result.Greater(cell_value, range_col_value);
                } else if ( 2 == ol && '=' == op[1] ) {
                    result.GreaterOrEqual(cell_value, range_col_value);
                }
                break;
            }
            case '=':
            {
                result.Equal(cell_value, range_col_value);
                break;
            }
            case '!':
            {
                if ( 2 == ol && '=' == op[1] ) {
                    result.NotEqual(cell_value, range_col_value);
                }
                break;
            }
            default:
                break;
        }
        
        if ( true == result.GetBoolean() ) {
            sum += cell_value.ToNumber();
            if ( a_logfile != NULL ) {
                fprintf(a_logfile, " += %-30.30s ....... %g\n", sum_rows_[row][col].c_str(), a_symtab[sum_rows_[row][col]].ToNumber());
            }
        }
    }
    
    return sum;
}
