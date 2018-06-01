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

#include "casper/see/vlookup.h"
#include "casper/see/table.h"

#ifdef DEBUG
    #define SEE_VLOOKUP_DEBUG_ENABLED 1
#endif // SEE_VLOOKUP_DEBUG_ENABLED

/**
 * @brief Default constructor.
 *
 * @param a_see
 * @param a_search_column_name
 * @param a_result_column_index
  */
casper::see::Vlookup::Vlookup (casper::see::See& a_see,
                               const char* const a_search_column_name, const int& a_result_column_index)
    : see_(a_see), search_column_name_(a_search_column_name), result_column_index_(a_result_column_index)
{
    /* empty */
}

/**
 * @brief Destructor.
 */
casper::see::Vlookup::~Vlookup ()
{
    /* empty */
}

/**
 * @brief
 *
 * @param a_see
 */
void casper::see::Vlookup::CalculateDependencies (See& a_see)
{
    OSAL_UNUSED_PARAM(a_see);

    casper::see::ColumnInfo search_column_info;
    casper::see::ColumnInfo result_column_info;

    SeekColumnsInfo(search_column_name_.c_str(), result_column_index_, search_column_info, result_column_info);
    
    const auto required_columns = { &search_column_info, &result_column_info };
    
    precedents_.clear();

    const size_t start_row = see_.lines_clones_offset_;
    const size_t end_row   = see_.lines_clones_offset_ + see_.lines_clones_count_ - 1;
    char cell_ref[20]      = { 0 };

    // ... for all required columns ...
    for ( auto column : required_columns ) {
        // for all 'cloned' lines ...
        for ( size_t row = start_row ; row <= end_row ; ++row ) {
            // ... rebuild cell reference ...
            Sum::MakeRowColRef(cell_ref, static_cast<int>(row), column->col_);
            // TODO:
            //                // ... exists @ aliases?
            //                const auto ait = aliases_.find(cell_ref);
            //                if ( ait != aliases_.end() ) {
            //                    temp_formula_->precedents_.insert(ait->second);
            //                } else {
            //                    // ... exists @ symb table?
            //                    const auto sit = symtab_.find(cell_ref);
            //                    if ( sit != symtab_.end() ) {
            //                        temp_formula_->precedents_.insert(sit->first);
            //                    }
            //                }
            precedents_.insert(cell_ref);
        }
    }
}

/**
 * @brief
 *
 * @param a_value
 * @param a_lookup_col
 * @param a_result_index
 * @param a_range_lookup
 *
 * @return
 */
casper::Term casper::see::Vlookup::VLOOKUP (const casper::Term& a_value, const char* const a_lookup_col, const casper::Term& a_result_index, bool a_range_lookup)
{
 
    std::map<std::string, int>::iterator it;
    
    casper::Term result (casper::Term::Term::ENan);
    result.number_ = NAN;
    
    casper::see::ColumnInfo search_column_info;
    casper::see::ColumnInfo result_column_info;
    
    char search_cell_ref[20];
    char result_cell_ref[20];
    
    const int result_index = (int) a_result_index.ToNumber();

    SeekColumnsInfo (a_lookup_col, result_index, search_column_info, result_column_info);

    const size_t start_row = see_.lines_clones_offset_;
    const size_t end_row   = see_.lines_clones_offset_ + see_.lines_clones_count_ - 1;
    
#if defined(SEE_VLOOKUP_DEBUG_ENABLED)
    auto debug_log = [this, &a_lookup_col, &result_index, &a_range_lookup, &search_column_info, &result_column_info, &result, &search_cell_ref, &result_cell_ref]() {
        fprintf(stderr, ">>> LinesVlookup(..., ..., '%s' ,%d, %d) ::: [%s][%d]%s : [%s][%d]%s => %s\n",
                a_lookup_col, result_index, a_range_lookup,
                search_cell_ref, search_column_info.col_, search_column_info.name_.c_str(),
                result_cell_ref, result_column_info.col_, result_column_info.name_.c_str(),
                result.DebugString().c_str());
    };
#endif // SEE_VLOOKUP_DEBUG_ENABLED
    
    if ( a_range_lookup == false ) {
        for ( size_t idx = start_row ; idx <= end_row ; ++idx ) {
            casper::see::Sum::MakeRowColRef (search_cell_ref, static_cast<int>(idx), search_column_info.col_);
            if ( true == casper::see::Table::Equal(a_value, see_.line_values_[search_cell_ref]) ) {
                casper::see::Sum::MakeRowColRef (result_cell_ref, static_cast<int>(idx), result_column_info.col_);
                result = see_.line_values_[result_cell_ref];
#if defined(SEE_VLOOKUP_DEBUG_ENABLED)
                debug_log();
#endif // SEE_VLOOKUP_DEBUG_ENABLED
                return result;
            }
        }
    } else {
        for ( size_t idx = start_row ; idx <= end_row ; ++idx ) {
            casper::see::Sum::MakeRowColRef (search_cell_ref, static_cast<int>(idx), search_column_info.col_);
            if ( true == casper::see::Table::Lower(a_value, see_.line_values_[search_cell_ref]) ) {
                casper::see::Sum::MakeRowColRef (result_cell_ref, static_cast<int>(idx), result_column_info.col_);
                result = see_.line_values_[result_cell_ref];
#if defined(SEE_VLOOKUP_DEBUG_ENABLED)
                debug_log();
#endif // SEE_VLOOKUP_DEBUG_ENABLED
                return result;
            }
        }
        casper::see::Sum::MakeRowColRef (search_cell_ref, static_cast<int>(end_row - 1), search_column_info.col_);
        result = see_.line_values_[search_cell_ref];
#if defined(SEE_VLOOKUP_DEBUG_ENABLED)
        debug_log();
#endif // SEE_VLOOKUP_DEBUG_ENABLED
    }
    
    return result;
}

/**
 * @brief Seek 'search' and 'result' columns.
 *
 * @param a_see
 * @param a_search_column_name
 * @param a_result_column_index
 * @param o_search_column_info
 * @param o_result_column_info
 */
void casper::see::Vlookup::SeekColumnsInfo (const char* const a_search_column_name, const int& a_result_column_index,
                                            casper::see::ColumnInfo& o_search_column_info, casper::see::ColumnInfo& o_result_column_info)
{
    const int max_int = std::numeric_limits<int>::max();
    
    o_result_column_info.col_ = max_int;
    
    int result_index = a_result_column_index;
    if ( result_index > -1 && result_index < static_cast<int>(see_.columns_.size()) ) {
        result_index -= 1;
        for ( auto it : see_.columns_ ) {
            if ( it.second.col_ == result_index ) {
                o_result_column_info = it.second;
                break;
            } else if ( it.second.col_ < o_result_column_info.col_ ) {
                o_result_column_info = it.second;
            }
        }
    }
    
    o_search_column_info.col_ = max_int;
    
    if ( a_search_column_name[0] == 0 ) {
        const int search_index = 0;
        for ( auto it : see_.columns_ ) {
            if ( o_search_column_info.col_ == search_index ) {
                o_search_column_info = it.second;
                break;
            } else if ( it.second.col_ < o_search_column_info.col_ ) {
                o_search_column_info = it.second;
            }
        }
    } else {
        for ( auto it : see_.columns_ ) {
            if ( 0 == strcasecmp(it.second.name_.c_str(), a_search_column_name) ) {
                o_search_column_info = it.second;
                break;
            }
        }
    }
    
    if ( max_int == o_search_column_info.col_ ) {
        throw OSAL_EXCEPTION("Column named '%s' is not a valid lookup column", a_search_column_name);
    } else if ( max_int == o_result_column_info.col_ ) {
        throw OSAL_EXCEPTION("Column @ index %d is not a valid result column!", a_result_column_index);
    }
}
