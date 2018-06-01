/**
 * @file Table.cc Implementation of Table
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

#include "casper/term.h"
#include "casper/see/table.h"
#include "osal/utils/tmp_json_parser.h"

#include <algorithm> // std::find_if
#include <regex>     // std::regex
#include <sstream>


/**
 * @brief Constructor
 *
 * @param a_name Table name
 * @param a_is_partial true if the table is to be partially loaded
 */
casper::see::Table::Table (const char* a_name, bool a_is_partial)
{
    if ( 0 != a_name[0] ) {
        name_ = a_name;
    }
    partially_loaded_ = a_is_partial;
    use_exact_match_  = a_is_partial;
    track_lookups_    = false;
}

/**
 * @brief Destructor
 */
casper::see::Table::~Table ()
{
    Clear();
}

void casper::see::Table::Clear ()
{
    colname_to_index_.clear();
    columns_.clear();
    tracked_lookups_.Clear();
}

casper::see::Table::Column& casper::see::Table::AddColumn (const char* a_name)
{
    Column dummy;
    int    index;

    dummy.name_ = a_name;

    index = (int) columns_.size();
    colname_to_index_[a_name] = index;
    columns_.push_back(dummy);
    return columns_[index];
}

/**
 * @brief Load the table from the parsed JSON data model
 *
 * @param a_value      Data model for the table
 * @param a_name       name of the table
 * @param a_is_partial true if the load is incremental, false for normal load
 */
void casper::see::Table::Load (Json::Value& a_value, const char* a_name)
{
    int  rows = -1;
    int  col_index = 0;

    name_ = a_name;

    if ( a_value.isArray() == false ) {
        throw OSAL_EXCEPTION_NA("the table top object must be an array");
    }
    for (Json::Value::iterator it = a_value.begin(); it != a_value.end(); ++it ) {

        if ( (*it)["name"].isNull() || (*it)["type"].isNull() || (*it)["data"].isNull() ) {
            throw OSAL_EXCEPTION("missing mandatory field %s", (*it)["name"].isNull() ? "name" : (*it)["type"].isNull() ? "type" : "data");
        }

        // Add or reuse the existing column and keep ref pointer to it
        const char* col_name = (*it)["name"].asCString();
        Column* col;
        if ( partially_loaded_ == false ) {
            col = &AddColumn(col_name);
        } else {
            std::map<std::string, int>::iterator column_it = colname_to_index_.find(col_name);
            if ( column_it == colname_to_index_.end()) {
                throw OSAL_EXCEPTION("Appending column %s that does not exist", col_name);
            } else {
                col = &columns_[column_it->second];
            }
        }

        // Fill or append the data vectors
        bool numeric = (*it)["type"] == "number";
        for ( Json::Value::iterator dit = (*it)["data"].begin(); dit != (*it)["data"].end(); ++dit ) {
            casper::Term value = casper::Term();
            if ( numeric ) {
                value = (*dit).asDouble();
            } else {
                value = (*dit).asString();
            }
            col->values_.push_back(value);
        }

        // Make sure all the colums have the same size
        if ( -1 == rows ) {
            rows = (int)(col->values_.size());
        } else {
            if ( rows != (int)(col->values_.size()) ) {
                throw OSAL_EXCEPTION("column '%s' height of %d is diferent from %d, all rows must have the same height",
                                     col_name, (int)(col->values_.size()), rows);
            }
        }
        col_index++;
    }
    partially_loaded_ = false;
}

double casper::see::Table::SumColumn (const char* a_sum_column)
{
    std::map<std::string, int>::iterator sum_index_it;
    int                                  col_index;
    double                               sum;
    double                               value;

    sum = 0.0;

    sum_index_it = colname_to_index_.find(a_sum_column);
    if ( sum_index_it == colname_to_index_.end() ) {
        throw OSAL_EXCEPTION("table '%s' does not have '%s' column", name_.c_str(), a_sum_column);
    }
    col_index = sum_index_it->second;

    int rows = (int)(columns_[col_index].values_.size());

    for ( int row = 0; row < rows; ++row ) {
        if ( casper::Term::ENumber == columns_[col_index].values_[row].type_ ) {
            sum += columns_[col_index].values_[row].GetNumber();
        } else if ( false == std::isnan( ( value = columns_[col_index].values_[row].ToNumber() ) ) ) {
            sum += value;
        }
    }

    return sum;
}

void casper::see::Table::SumIfs (Term& a_result, const char* a_sum_column, SymbolTable& a_criterias)
{
    std::map<std::string, int>::iterator index_it;
    int                                  sum_col_index;
    int                                  criteria_col_index;
    double                               sum;
    double                               value;

    sum = 0.0;

    index_it = colname_to_index_.find(a_sum_column);
    if ( index_it == colname_to_index_.end() ) {
        throw OSAL_EXCEPTION("table '%s' does not have '%s' column", name_.c_str(), a_sum_column);
    }
    sum_col_index = index_it->second;

    int rows = (int)(columns_[sum_col_index].values_.size());

    for ( int row = 0; row < rows; ++row ) {
        bool match = true;
        for (SymbolTable::iterator criteria_it = a_criterias.begin(); criteria_it != a_criterias.end(); ++criteria_it ) {

            index_it = colname_to_index_.find(criteria_it->first);
            if ( index_it == colname_to_index_.end() ) {
                throw OSAL_EXCEPTION("table '%s' does not have '%s' column", name_.c_str(), criteria_it->first.c_str());
            }
            criteria_col_index = index_it->second;

            if ( false == Equal(columns_[criteria_col_index].values_[row], criteria_it->second) ) {
                match = false;
                break;
            }
        }
        if ( true == match ) {
            if ( casper::Term::ENumber == columns_[sum_col_index].values_[row].GetType() ) {
                sum += columns_[sum_col_index].values_[row].GetNumber();
            } else if ( false == std::isnan( ( value = columns_[sum_col_index].values_[row].ToNumber() ) ) ) {
                sum += value;
            }
        }
    }
    a_result = sum;
}

void casper::see::Table::Lookup (Term& a_result, Term& a_value, const char* a_search_col, const char* a_result_col)
{
    std::map<std::string, int>::iterator it;
    int                                  search_index;
    int                                  result_index;
    unsigned                             row;

    if ( a_search_col[0] == 0 ) {
        search_index = 0;
    } else {
        it = colname_to_index_.find(a_search_col);
        if ( it == colname_to_index_.end() ) {
            throw OSAL_EXCEPTION("'%s' is not a valid search column", a_search_col);
        }
        search_index = it->second;
    }
    it = colname_to_index_.find(a_result_col);
    if ( it == colname_to_index_.end() ) {
        throw OSAL_EXCEPTION("'%s' is not a valid result column", a_result_col);
    }
    result_index = it->second;

    if ( use_exact_match_ ) {
        Term result_idx;

        result_idx = (double) result_index + 1;
        return Vlookup(a_result, a_value, a_search_col, result_idx, false);
    }

    for ( row = 0; row < columns_[search_index].values_.size(); ++row ) {
        if ( true == Lower(a_value, columns_[search_index].values_[row]) ) {
            if ( row != 0 ) {
                --row;
            }
            break;
        }
    }

    if ( row > columns_[search_index].values_.size() || 0 == columns_[search_index].values_.size() ) {
        throw OSAL_EXCEPTION("'%s' is not a valid vlookup - no rows for search_index %d!", a_result_col, search_index);
    }

    if ( row == columns_[search_index].values_.size() ){
        --row;
    }
    a_result = columns_[result_index].values_[row];
    if ( true == track_lookups_ ) {
        TrackLookup(row);
    }
}

void casper::see::Table::Vlookup (Term& a_result, Term& a_value, const char* a_lookup_col, Term& a_result_index, bool a_range_lookup)
{
    std::map<std::string, int>::iterator it;
    int                                  search_index;
    int                                  result_index;
    unsigned                             row;

    if ( a_lookup_col[0] == 0 ) {
        search_index = 0;
    } else {
        it = colname_to_index_.find(a_lookup_col);
        if ( it == colname_to_index_.end() ) {
            throw OSAL_EXCEPTION("'%s' is not a valid result column", a_lookup_col);
        }
        search_index = it->second;
    }

    if ( ! (a_result_index.type_ & Term::ENumber) ) {
        throw OSAL_EXCEPTION("'%s' is not valid index a number", a_result_index.ToString().c_str());
    }
    result_index = (int) a_result_index.ToNumber();
    if ( result_index < 1 || result_index > (int) columns_.size() ) {
        a_result.type_   = Term::ENan;
        a_result.number_ = NAN;
        return;
    }
    result_index -= 1;

    if ( a_range_lookup == false ) {

        for ( row = 0; row < columns_[search_index].values_.size(); ++row ) {
            if ( true == Equal(a_value, columns_[search_index].values_[row]) ) {
                a_result = columns_[result_index].values_[row];
                if ( true == track_lookups_ ) {
                    TrackLookup(row);
                }
                return;
            }
        }

        a_result.type_   = Term::ENan;
        a_result.number_ = NAN;

        return;

    } else {

        for ( row = 0; row < columns_[search_index].values_.size(); ++row ) {
            if ( true == Lower(a_value, columns_[search_index].values_[row]) ) {
                if ( row != 0 ) {
                    --row;
                }
                break;
            }
        }

        if ( row == columns_[search_index].values_.size() ) {
            --row;
        }
        a_result = columns_[result_index].values_[row];

    }
    if ( true == track_lookups_ ) {
        TrackLookup(row);
    }
}

void casper::see::Table::PrepareTracking (const Json::Value& a_filter, const Json::Value& a_params)
{
    // ... filter is set?
    if ( true == a_filter.isNull() ) {
        // ... no ... full dump ...
        if ( 0 == columns_.size() ) {
            return;
        }
        const size_t number_of_rows = columns_[0].values_.size();
        if ( 0 == number_of_rows ) {
            TrackLookup(-1);
        } else {
            for ( size_t row_idx = 0 ; row_idx < number_of_rows ; ++row_idx ) {
                TrackLookup(static_cast<int>(row_idx));
            }
        }
    } else {
        // ... partial dump ...
        const Json::Value& column_name = a_filter["column_name"];
        if ( true == column_name.isNull() || Json::ValueType::stringValue != column_name.type() || 0 == column_name.asString().length() ) {
            throw OSAL_EXCEPTION("Missing or invalid filter value for column_name @ table '%s'!",
                                 name_.c_str()
                                 );
        }

        const Json::Value& column_value_regex = a_filter["column_value_regex"];
        if ( true == column_value_regex.isNull() || Json::ValueType::stringValue != column_value_regex.type() || 0 == column_value_regex.asString().length() ) {
            throw OSAL_EXCEPTION("Missing or invalid filter value for column_value_regex @ table '%s'!",
                                 name_.c_str()
                                 );
        }

        const std::string column_name_s = column_name.asString();

        const auto column_it = colname_to_index_.find(column_name_s);
        if ( colname_to_index_.end() == column_it ) {
            throw OSAL_EXCEPTION("Clound not apply filter @ table '%s' - column '%s' not found!",
                                 name_.c_str(), column_name_s.c_str()
                                 );
        } else if ( column_it->second < 0 ) {
            throw OSAL_EXCEPTION("Clound not apply filter @ table '%s' - got an invalid index ( %d ) for column '%s'!",
                                 name_.c_str(), column_it->second, column_name_s.c_str()
                                 );
        }

        const size_t column_idx = static_cast<size_t>(column_it->second);

        const size_t number_of_rows = columns_[column_idx].values_.size();
        if ( 0 == number_of_rows ) {
            TrackLookup(-1);
        } else {
            const std::string regexp = casper::see::Table::BuildQuery (column_value_regex.asString(), a_params);
            const std::regex filter_expr(regexp, std::regex_constants::ECMAScript);
            for ( size_t row_idx = 0 ; row_idx < number_of_rows ; ++row_idx ) {
                const std::string value = columns_[column_idx].values_[row_idx].AsString();
                auto tmp_begin = std::sregex_iterator(value.begin(), value.end(), filter_expr);
                if ( tmp_begin == std::sregex_iterator() ) {
                    continue;
                }
                TrackLookup(static_cast<int>(row_idx));
            }
        }
    }
}

void casper::see::Table::TrackLookup (int a_row)
{
    Json::Value* json_array = tracked_lookups_.JSONObject(a_row);
    if ( nullptr != json_array ) {

        if ( -1 == a_row ) {
            for ( size_t column_index = 0 ; column_index < columns_.size(); ++column_index ) {
                const auto column = columns_[column_index];
                Json::Value json_object = Json::Value(Json::ValueType::objectValue);
                json_object["name"] = column.name_;
                json_object["type"] = Json::nullValue;
                json_array->append(json_object);
            }
        } else {
            for ( size_t column_index = 0 ; column_index < columns_.size(); ++column_index ) {
                const auto column = columns_[column_index];
                Json::Value json_object = Json::Value(Json::ValueType::objectValue);
                json_object["name"] = column.name_;
                switch(column.values_[a_row].type_) {
                    case casper::Term::ENumber:
                        json_object["type"] = "number";
                        json_object["data"] = column.values_[a_row].ToNumber();
                        break;
                    case casper::Term::EText:
                        json_object["type"] = "text";
                        json_object["data"] = column.values_[a_row].AsString();
                        break;
                    case casper::Term::EDate:
                    case casper::Term::EExcelDate:
                        json_object["type"] = "date";
                        json_object["data"] = "\"" + column.values_[a_row].AsString() + "\"";
                        break;
                    case casper::Term::EBoolean:
                        json_object["type"] = "boolean";
                        json_object["data"] = column.values_[a_row].ToBoolean();
                        break;
                    case casper::Term::EUndefined:
                    default:
                        break;
                }
                json_array->append(json_object);
            }
        }
    }
}

bool casper::see::Table::Equal (const casper::Term& a_first, const casper::Term& a_second)
{
    if ( a_first.type_ != a_second.type_ ) {

        // .. boolean exception ...

        if ( casper::Term::ENumber == a_first.type_ && casper::Term::EBoolean == a_second.type_ ) {
            return a_first.GetNumber() == a_second.ToNumber();
        }

        if ( casper::Term::EBoolean == a_first.type_ && casper::Term::ENumber == a_second.type_ ) {
            return a_first.ToNumber() == a_second.GetNumber();
        }

        // ... number exception ...
        if ( casper::Term::ENumber == a_first.type_ && casper::Term::EText == a_second.type_ ) {
            return a_first.GetNumber() == a_second.ToNumber();
        }

        if ( casper::Term::EText == a_first.type_ && casper::Term::ENumber == a_second.type_ ) {
            return a_first.ToNumber() == a_second.GetNumber();
        }

        return false;
    }

    switch (a_first.type_) {

        case casper::Term::ENumber:
            return a_first.number_ == a_second.number_;

        case casper::Term::EText:
            return strcmp(a_first.text_.c_str(), a_second.text_.c_str()) == 0;

#if 0
        case casper::Term::EDate:
            return a_first.number_ == a_second.number_;

        case casper::Term::EBoolean:
            return a_first.number_ == a_second.number_;

        case casper::Term::ENullError:
            return a_first.type_ == a_second.type_;
#endif

        default:
            return false;
    }
}

bool casper::see::Table::Lower (const casper::Term& a_first, const casper::Term& a_second)
{
    if ( a_first.type_ != a_second.type_ ) {
        return false;
    }

    switch (a_first.type_) {

        case casper::Term::ENumber:
            return a_first.number_ < a_second.number_;

        case casper::Term::EText:
            return strcmp(a_first.text_.c_str(), a_second.text_.c_str()) < 0;

#if 0
        case casper::Term::EDate:
            return a_first.number_ < a_second.number_;

        case casper::Term::EBoolean:
            return a_first.number_ < a_second.number_;
#endif

        default:
            return false;
    }
}

/**
 * @brief
 *
 * @param a_query
 * @param a_params
 */
std::string casper::see::Table::BuildQuery (const std::string& a_query, const Json::Value& a_params)
{
    // ... unexpected object ?
    if ( a_params.type() != Json::ValueType::objectValue ) {
        throw OSAL_EXCEPTION("Unexpected object type for 'a_params' - got %d, expected %d!", a_params.type(), Json::ValueType::objectValue);
    }


    auto find_and_replace = [] (std::string& source, std::string const& find, std::string const& replace) {
        for ( std::string::size_type idx = 0; std::string::npos != ( idx = source.find(find, idx) ) ; ) {
            source.replace(idx, find.length(), replace);
            idx += replace.length();
        }
    };

    auto find_closure_start = [] (const char* const a_string) -> const char* {
        const char * upper_case_attempt = strstr(a_string, "$P{");
        if ( nullptr != upper_case_attempt ) {
            return upper_case_attempt;
        }
        return strstr(a_string, "$p{");
    };

    std::string query = a_query;

    const char* qry  = query.c_str();
    const char* pch  = find_closure_start(qry);
    const char* end  = nullptr;
    const size_t tcz = sizeof(char) * 3;

    while ( nullptr != pch ) {

        end = strstr(pch, "}");
        if ( nullptr == end ) {
            throw OSAL_EXCEPTION("Incomplete substitution closure '%s'!", pch);
        }

        const std::string param_name  = std::string(pch + tcz, ( end - ( pch + tcz ) ) );

        std::stringstream value;

        const Json::Value& tmp_field = a_params[param_name];
        switch (tmp_field.type()) {
            case Json::ValueType::nullValue:
                value << "null";
                break;
            case Json::ValueType::intValue:
                value << tmp_field.asInt64();
                break;
            case Json::ValueType::uintValue:
                value << tmp_field.asUInt64();
                break;
            case Json::ValueType::realValue:
                value << tmp_field.asDouble();
                break;
            case Json::ValueType::stringValue:
            {
                const std::string tmp = tmp_field.asString();
                for ( size_t idx = 0 ; idx < tmp.size(); ++idx ) {
                    if ( '\'' == tmp[idx] ) {
                        value << "''";
                    } else {
                        value << tmp[idx];
                    }
                }
                break;
            }
            case Json::ValueType::booleanValue:
                value << tmp_field.asBool();
                break;
            default:
                throw OSAL_EXCEPTION("Unexpected scalar type %d for param name '%s'!", tmp_field.type(), param_name.c_str());
        }

        const std::string replaceable = std::string(pch, ( end - pch ) + 1 );
        const size_t      adv         = pch - qry;

        find_and_replace(query, replaceable, value.str());

        pch = find_closure_start(query.c_str() + adv);
    }

    return query;
}
