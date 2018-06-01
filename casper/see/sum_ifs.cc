/**
 * @file sum_ifs.cc Specialization of Sum for conditional sums
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

#include "casper/see/sum_ifs.h"
#include "casper/see/sum.h"
#include "casper/see/see.h"

/**
 * @brief Constructor
 */
casper::see::SumIfs::SumIfs (const char* a_sum_col, SymbolTable& a_criterias)
{
    /*
     * Copy the criterias table cols and terms names
     */
    for ( SymbolTable::iterator it = a_criterias.begin(); it != a_criterias.end(); ++it ) {
        col_names_.push_back(it->first);
    }
    sum_col_ = a_sum_col;
}

/**
 * @brief Destructor
 */
casper::see::SumIfs::~SumIfs ()
{
    /* empty */
}

void casper::see::SumIfs::CalculateDependencies (See& a_see)
{
    std::vector<std::string> dummy_vec;
    StringSet                tmp_prec;
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
        
        for ( int i = -1; i < (int) col_names_.size(); ++i ) {
        
            if ( -1 == i ) {
                c = a_see.columns_.find(sum_col_)->second.col_;
            } else {
                c = a_see.columns_.find(col_names_[i])->second.col_;
            }
    
            Sum::MakeRowColRef(cell_ref, r, c);

            ait = a_see.aliases_.find(cell_ref);
            if ( ait != a_see.aliases_.end() ) {
                tmp_prec.insert(ait->second);
                sum_rows_[row_cnt].push_back(ait->second);
            } else {
                sit = a_see.symtab_.find(cell_ref);
                if ( sit != a_see.symtab_.end() ) {
                    tmp_prec.insert(sit->first);
                    sum_rows_[row_cnt].push_back(sit->first);
                } else {
                    sit = a_see.line_values_.find(cell_ref);
                    if ( sit != a_see.line_values_.end() ) {
                        tmp_prec.insert(sit->first);
                        sum_rows_[row_cnt].push_back(sit->first);
                    }
                }
            }
        }
        if ( sum_rows_[row_cnt].size() == col_names_.size() + 1 ) {
            sum_rows_.push_back(dummy_vec);
            precedents_.insert(tmp_prec.begin(), tmp_prec.end());
            ++row_cnt;
        } else {
            sum_rows_[row_cnt].clear();
        }
    }

    /*
     * Trim the last row if it's not complete
     */
    if ( sum_rows_[row_cnt].size() != col_names_.size() + 1 ) {
        sum_rows_.pop_back();
    }
}


double casper::see::SumIfs::SumIfAllTerms (SymbolTable& a_symtab, SymbolTable& a_criterias, FILE* a_logfile)
{
    int    matches, col;
    double sum;
    Term   comparator;
    
    sum = 0;
    for ( size_t row = 0; row < sum_rows_.size(); ++row ) {
        matches = 0;

        col = 1;
        for ( SymbolTable::iterator it = a_criterias.begin(); it != a_criterias.end(); ++it ) {
            comparator.Equal(a_symtab[sum_rows_[row][col]], it->second);
            if ( comparator.number_ == 1.0 ) {
                ++matches;
            }
            ++col;
        }
        if ( matches == (int) a_criterias.size() ) {
            if ( a_logfile != NULL ) {
                fprintf(a_logfile, " += %-30.30s ....... %g\n", sum_rows_[row][0].c_str(), a_symtab[sum_rows_[row][0]].ToNumber());
            }
            sum += a_symtab[sum_rows_[row][0]].ToNumber();
        }
    }
    return sum;
}
