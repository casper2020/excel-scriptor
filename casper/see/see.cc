/**
 * @file See.cc Implementation of Simple expression evaluator
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

#include "casper/see/see.h"
#include "casper/see/sum.h"
#include "casper/see/sum_ifs.h"
#include "casper/see/sum_if.h"
#include "casper/see/row_shifter.h"
#include "casper/see/table.h"
#include "casper/see/vlookup.h"

#include "lemon/topology_sort.h"
#include "osal/utils/tmp_json_parser.h"
#include "osal/utils/swatch.h"
#include "osal/osal_date.h"
#include "osal/debug_trace.h"
#include <algorithm>
#include <sstream>
#include <strings.h>

#ifdef __APPLE__
#pragma mark -
#pragma mark ::: CONSTRUCTOR(S) / DESTRUCTOR :::
#pragma mark -
#endif

/**
 * @brief Constructor
 */
casper::see::See::See ()
    : parser_(scanner_, *this)
{
    check_dependencies_          = false;
    temp_formula_                = NULL;
    row_count_                   = 0;
    log_file_                    = NULL;
    line_it_                     = code_list_.begin();
    code_col_name_               = "COD";
    condition_col_name_          = "CONDICAO";
    table_header_row_            = 0;
    data_source_row_index_       = -1;
    has_template_lines_          = false;
    serialize_empty_str_as_null_ = true;

    lines_templates_start_idx_ = 0;
    lines_templates_end_idx_   = 0;
    lines_templates_count_     = 0;
    lines_clones_start_idx_    = 0;
    lines_clones_end_idx_      = 0;
    lines_clones_count_        = 0;
    lines_clones_offset_       = 0;
    track_lookups_             = false;
}

/**
 * @brief Destructor
 */
casper::see::See::~See ()
{
    untouchable_tables_.clear();
    Clear();
    if ( nullptr != log_file_ ) {
        fclose(log_file_);
        log_file_ = nullptr;
    }
}

#ifdef __APPLE__
#pragma mark -
#pragma mark ::: CLEANUP :::
#pragma mark -
#endif

/**
 * @brief Clears all internal data
 */
void casper::see::See::Clear ()
{
    if ( temp_formula_ != NULL ) {
        delete temp_formula_;
        temp_formula_ = NULL;
    }

    // Release all formulas
    for ( FormulaList::iterator it = formulas_.begin(); it != formulas_.end(); ++it) {
        delete *it;
    }
    formulas_.clear();

    // Release all tables
    std::set<std::string> deletable_tables;
    for (TableHash::iterator tbl_it = tables_.begin(); tbl_it != tables_.end(); ++tbl_it) {
        const auto k_it = untouchable_tables_.find(tbl_it->first);
        if ( untouchable_tables_.end() == k_it ) {
            deletable_tables.insert(tbl_it->first);
        }
    }

    for ( auto it = deletable_tables.begin() ; deletable_tables.end() != it ; ++it ) {
        const auto tbl_it = tables_.find((*it));
        delete tbl_it->second;
        tables_.erase(tbl_it);
    }
    aliases_.clear();
    precedents_.clear();
    symtab_.clear();
    name_to_cell_aliases_.clear();
    line_values_.clear();
    sum_criterias_.clear();
    columns_.clear();
    column_name_index_.clear();
    code_list_.clear();
    data_source_row_index_ = -1;
}

#ifdef __APPLE__
#pragma mark -
#pragma mark ::: FORMULA LOADING :::
#pragma mark -
#endif

void casper::see::See::LoadModel (StringMultiHash& a_clone_map,
                                  const std::function<void(Json::Value& a_scalars)> a_patch_scalars,
                                  const std::function<void(Json::Value& a_lines, size_t& o_number_of_added_lines)> a_clone_lines)
{
    std::string model_file_path;

    if ( json_data_path_.length() == 0 ) {
        throw OSAL_EXCEPTION_NA("JSON folder path is not defined");
    }
    json_tables_path_  = json_data_path_;
    json_tables_path_ += "/Users/bruno/work/excelscriptor/models/tables/";
    model_file_path    = json_data_path_;
    model_file_path   += "/Users/bruno/work/excelscriptor/models/model_typed.json";
    LoadModelFromFile(model_file_path.c_str(), a_clone_map, a_patch_scalars, a_clone_lines);
}

void casper::see::See::LoadModelFromFile (const char* a_filename, StringMultiHash& a_clone_map,
                                          const std::function<void(Json::Value& a_scalars)> a_patch_scalars,
                                          const std::function<void(Json::Value& a_lines, size_t& o_number_of_added_lines)> a_clone_lines)
{
    osal::utils::Swatch tf;
    TmpJsonParser       parser;
    Json::Value*        parsed_json;

    tf.Start();
    parsed_json = parser.LoadAndParse(a_filename);
    if ( parsed_json == NULL ) {
        throw OSAL_EXCEPTION("model file %s not found", a_filename);
    }
    LoadModel(*parsed_json, a_clone_map, a_patch_scalars, a_clone_lines);
    tf.Stop();
    printf("Loading time %ld ms\n", tf.Ticks() / 1000);
}

void casper::see::See::LoadModel (Json::Value& a_model, StringMultiHash& a_clone_map,
                                  const std::function<void(Json::Value& a_scalars)> a_patch_scalars,
                                  const std::function<void(Json::Value& a_lines, size_t& o_number_of_added_lines)> a_clone_lines)
{
    Json::Value           values, formulas, lines, line_formulas, line_values, lines_header;
    Json::Value::Members  members;
    StringHash            line_values_expressions;
    Term                  term1, term2;
    char                  cell_ref[20];
    char                  tmp_cell_ref[20];
    size_t                row_count;
    casper::see::location location;
    std::stringstream     ss;

    row_count_ = 0;
    data_source_row_index_ = -1;

    if ( true == untouched_model_.isNull() ) {
        untouched_model_ = a_model;
    }

    /*
     * Basic model validation, make sure we have the necessary pieces
     */
    if ( a_model.isObject() == false ) {
        throw OSAL_EXCEPTION_NA("Top object of the model must be an array");
    }
    scalars_types_map_.clear();
    LoadValues(a_model["values"], values, &scalars_types_map_);
    LoadValues(a_model["formulas"], formulas, &scalars_types_map_);
    lines    = a_model["lines"];

    if (   values.isNull()   || values.isObject()   == false
        || formulas.isNull() || formulas.isObject() == false
        || lines.isNull()    || lines.isObject()    == false ) {
        throw OSAL_EXCEPTION_NA("model must have a 'values' object, a 'formulas' object and a 'lines' array");
    }
    lines_columns_types_map_.clear();
    LoadHeaders(lines["header"], lines_header, &lines_columns_types_map_);
    lines["header"] = lines_header;
    line_formulas = lines["formulas"];
    line_values   = lines["values"];
    if (   line_values.isNull()   || line_values.isArray()   == false
        || line_formulas.isNull() || line_formulas.isArray() == false
        || lines_header.isNull()  || lines_header.isObject() == false ) {
        throw OSAL_EXCEPTION_NA("'lines' must have a 'header' object, a 'formulas' array and a 'values' array");
    }

    row_count_ = MAX(line_formulas.size(), line_values.size());

    /*
     * Load the 'lines' table columns in hash, the column name will point to the excel cell reference
     */
    column_name_index_.clear();
    members = lines_header.getMemberNames();
    for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it ) {
        ColumnInfo cinfo;
        int32_t    row;
        int32_t    col;

        Sum::ParseCellRef(it->c_str(), &col, &row);
        cinfo.name_     = lines_header[it->c_str()].asString();
        cinfo.row_      = row;
        cinfo.col_      = col;

        auto typ_it = lines_columns_types_map_.find(it->c_str());
        if ( typ_it == lines_columns_types_map_.end() ) {
            throw OSAL_EXCEPTION("column '%s' type information not found\n", cinfo.name_.c_str());
        }
        cinfo.col_type_ = &typ_it->second;
        columns_[cinfo.name_] = cinfo;
        column_name_index_[cinfo.col_] = cinfo.name_;
    }

    if ( nullptr == a_clone_lines ) {
        CloneLinesTableLines(a_clone_map, line_formulas, line_values);
    }

    table_header_row_ = columns_.begin()->second.row_;

    /*
     * Evaluate value, if needed.
     */
    const auto evaluate_value = [this, &ss](const char* const a_ref, const char* const a_name, const char* const a_expression, Json::Value& a_value) {

        const char* expression_start = strstr(a_expression, "=excel_date(");
        if ( nullptr != expression_start ) {
            const char* const expression_end = strstr(expression_start, ")");
            if ( nullptr != expression_end ) {
                const char* const start = strstr(expression_start, "(") + sizeof(char);

                ss.str("");
                if ( nullptr != a_name ) {
                    ss << a_name << "=";
                } else {
                    ss << a_ref << "=";
                }
                ss << osal::Date::ToExcelDate(std::string { start, static_cast<size_t>(expression_end - start) });
                a_value[a_ref] = ss.str();
            }
        }

    };

    const auto evaluate_value_members = [this, &ss, &term1, &term2, &location, &evaluate_value] (Json::Value& a_value) {
        for ( auto member : a_value.getMemberNames() ) {
            std::string exp = a_value[member].asString();
            scanner_.SetInput(exp.c_str(), exp.size());
            if ( scanner_.Scan(&term1, &location) == Parser::token::VAR && scanner_.Scan(&term2, &location) == '=' ) {
                if ( term1.ToString() != member ) {
                    evaluate_value(member.c_str(), term1.text_.c_str(), exp.c_str(), a_value);
                    name_to_cell_aliases_[term1.text_] = member;
                } else {
                    evaluate_value(member.c_str(), nullptr, exp.c_str(), a_value);
                }
            }
        }
    };


    /*
     * Use the make shift expression parser to make a temp list of aliases related to values
     */
    members = values.getMemberNames();
    for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it ) {
        std::string exp = values[it->c_str()].asString();
        scanner_.SetInput(exp.c_str(), exp.size());
        if ( scanner_.Scan(&term1, &location) == Parser::token::VAR && scanner_.Scan(&term2, &location) == '=' ) {
            if ( term1.ToString() != it->c_str() ) {
                name_to_cell_aliases_[term1.text_] = it->c_str();
            }
        }
    }

    const auto type_formulas = [this] (const size_t a_start_index, const std::map<std::string, TypeMapEntry>& a_type_map) {

        char    ref[20] = { 0 };
        int32_t col = 0;
        int32_t row = 0;

        for ( size_t idx = a_start_index ; idx < formulas_.size() ; ++idx ) {
            const auto formula      = formulas_[idx];
            const auto type_by_name = a_type_map.find(formula->name_);
            if ( a_type_map.end() != type_by_name ) {
                formula->type_ = type_by_name->second.excel_;
            } else if ( 0 != formula->alias_.length() ) {
                const auto type_by_alias = a_type_map.find(formula->alias_);
                if ( a_type_map.end() != type_by_alias ) {
                    formula->type_ = type_by_alias->second.excel_;
                } else {
                    if ( &lines_columns_types_map_ == &a_type_map ) {
                        Sum::ParseCellRef(formula->alias_.c_str(), &col, &row);
                        Sum::MakeRowColRef(ref, table_header_row_, col);
                        const auto type_by_ref = a_type_map.find(ref);
                        if ( a_type_map.end() != type_by_ref ) {
                            formula->type_ = type_by_ref->second.excel_;
                        } else {
                            formula->type_ = "L???";
                        }
                    } else {
                        formula->type_ = "S???";
                    }
                }
            } else {
                formula->type_ = "A???";
            }
        }
    };
    size_t next_type_formulas_index = 0;

    /*
     * Load the scalar formulas
     */
    members = formulas.getMemberNames();
    for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it ) {
        LoadFormula(formulas[it->c_str()].asCString(), it->c_str());
    }

    // ... for debug proposes only ...
    if ( 0 != log_file_name_.length() ) {
        type_formulas(next_type_formulas_index, scalars_types_map_);
        next_type_formulas_index = formulas_.size();
    }

    /*
     * PATCH(S)
     */
    // ... 'scalars' ...
    if ( nullptr != a_patch_scalars ) {
        a_patch_scalars(values);
    }
    evaluate_value_members(values);

    // ... remove empty line(s) ...
    lines_templates_start_idx_ = 0;
    lines_templates_count_     = has_template_lines_ ? line_values.size() : 0;
    lines_templates_end_idx_   = lines_templates_count_;
    lines_clones_start_idx_    = lines_templates_count_ + 1;
    lines_clones_end_idx_      = lines_clones_start_idx_;
    lines_clones_count_        = 0;
    lines_clones_offset_       = table_header_row_ + std::min(line_formulas.size(), line_values.size()) + 1;

    // ... 'lines' ...
    if ( nullptr != a_clone_lines ) {
        // ... 'clone ' lines ...
        a_clone_lines(lines, lines_clones_count_);
        line_formulas          = lines["formulas"];
        line_values            = lines["values"];
        row_count_             = MAX(line_formulas.size(), line_values.size());
        lines_clones_end_idx_ += row_count_ - 1;
    }

    row_count = 1;
    for ( Json::Value::iterator it = line_values.begin(); it != line_values.end(); ++it ) {
        if ( row_count < lines_templates_count_ ) {
            row_count += 1;
            continue;
        }
        evaluate_value_members((*it));
        row_count += 1;
    }

    /*
     * Load the formulas from the document lines, keep track of the cell ref to make
     * formula aliases as needed
     */
    row_count = 1;
    for ( Json::Value::iterator rit = line_formulas.begin(); rit != line_formulas.end(); ++rit ) {
        if ( row_count < lines_templates_count_ ) {
            row_count += 1;
            continue;
        }
        members = (*rit).getMemberNames();
        for (Json::Value::Members::iterator cit = members.begin(); cit != members.end(); ++cit ) {
            ColumnHash::iterator info_it = columns_.find(cit->c_str());
            if ( info_it == columns_.end() ) {
                throw OSAL_EXCEPTION("Column '%s' is not declared in the table header", cit->c_str());
            }
            Sum::MakeRowColRef(cell_ref, columns_[cit->c_str()].row_ + static_cast<int>(row_count), columns_[cit->c_str()].col_);
            LoadFormula((*rit)[cit->c_str()].asCString(), cell_ref);
        }
        row_count += 1;
    }

    /*
     * FOR DEBUG PROPOSES ONLY
     */
    if ( 0 != log_file_name_.length() ) {
        type_formulas(next_type_formulas_index, lines_columns_types_map_);
        next_type_formulas_index = formulas_.size();
    }
    if ( 0 != log_file_name_.length() ) {
        Json::Value patched_object          = Json::Value(Json::ValueType::objectValue);
        patched_object["values"]            = values;
        patched_object["formulas"]          = formulas;
        patched_object["lines"]["header"]   = lines_header;
        patched_object["lines"]["values"]   = line_values;
        patched_object["lines"]["formulas"] = line_formulas;
        Json::StyledWriter writer;
        FILE* f = fopen("/tmp/pg_see-json_patch.log", "w");
        if ( nullptr != f ) {
            fprintf(f, "%s", writer.write(patched_object).c_str());
            fclose(f);
        }
    }

    /*
     * Accumulate all lines values in the temporary hash
     */
    row_count = 1;
    for ( Json::Value::iterator rit = line_values.begin(); rit != line_values.end(); ++rit ) {
        members = (*rit).getMemberNames();

        for (Json::Value::Members::iterator cit = members.begin(); cit != members.end(); ++cit ) {

            ColumnHash::iterator info_it = columns_.find(cit->c_str());
            if ( info_it == columns_.end() ) {
                throw OSAL_EXCEPTION("Column '%s' is not declared in the table header", cit->c_str());
            }
            Sum::MakeRowColRef(cell_ref, columns_[cit->c_str()].row_ + static_cast<int>(row_count), columns_[cit->c_str()].col_);

            std::string exp = (*rit)[cit->c_str()].asString();
            scanner_.SetInput(exp.c_str(), exp.size());
            if ( scanner_.Scan(&term1, &location) == Parser::token::VAR && scanner_.Scan(&term2, &location) == '=' ) {
                if ( strcmp(term1.text_.c_str(), cell_ref) != 0 ) {
                    name_to_cell_aliases_[term1.text_] = cell_ref;
                }

                term1.SetNull();
                if ( ( (Parser::token_type) '-' ) == scanner_.Scan(&term1, &location) ) {
                    scanner_.Scan(&term1, &location);
                }

                tmp_cell_ref[0] = '\0';
                Sum::MakeRowColRef(tmp_cell_ref, table_header_row_, columns_[cit->c_str()].col_);

                const auto model_type_it = lines_columns_types_map_.find(tmp_cell_ref);
                if ( lines_columns_types_map_.end() != model_type_it ) {

                    if ( term1.GetType() != model_type_it->second.term_ ) {

                        if ( true == term1.IsNull() ) {
                            SetDefaultTermValue(model_type_it->second.term_, term1);
                        } else {
                            ConvertTermType(model_type_it->second.term_, term1);
                        }

                    }

                }

                line_values_[cell_ref] = term1;
            }

            line_values_expressions[cell_ref] = (*rit)[cit->c_str()].asString();
        }
        row_count += 1;
    }

    /*
     * Expand the sums and calculate the dependencies
     */
    CalculateSumDependencies();
    SortDependencies();

    reference_symtab_.clear();

    /*
     * Resolve independent terms
     */
    for ( StringSet::iterator it = precedents_.begin(); it != precedents_.end(); ++it ) {
        const char* variable_name;
        std::string value;

        variable_name = it->c_str();

        bool        variable_is_null_or_does_not_exists;
        bool        is_nullable = false;
        std::string excel_type;

        do {
            variable_is_null_or_does_not_exists = false;
            /*
             * 1st attempt, the variable is known by cell ref in the scalar values
             */
            if ( values[variable_name].isNull() == false ) {
                value = values[it->c_str()].asString();
                break;
            }

            /*
             * 2nd attempt, the variable is known by name, so we need to get the cellref 1st
             */
            StringHash::iterator nit = name_to_cell_aliases_.find(variable_name);
            if ( nit != name_to_cell_aliases_.end() ) {
                if ( values[nit->second].isNull() == false ) {
                    value = values[nit->second].asString();
                    break;
                }
            }

            /*
             * 3rd attempt, the variable is known by cell ref in one of the lines
             */
            nit = line_values_expressions.find(variable_name);
            if ( nit != line_values_expressions.end() ) {
                value = nit->second;
                break;
            }

            /*
             * 4th attempt, the variable is known by name in one of the lines cells
             */
            nit = name_to_cell_aliases_.find(variable_name);
            if ( nit != name_to_cell_aliases_.end() ) {
                nit = line_values_expressions.find(nit->second);
                if ( nit != line_values_expressions.end() ) {
                    value = nit->second;
                    break;
                }
            }

            /*
             * hope lost, can't resolve variable, ignore LINES it's a bogus
             */
            if ( strcmp(variable_name, "LINES") != 0 ) {
                // ... value is null or does not exist ...
                variable_is_null_or_does_not_exists = true;
            } else {
                value = "";
            }

            break;

        } while (true);

        if ( true == variable_is_null_or_does_not_exists ) {

            const int model_type = GetParamType(variable_name, nullptr, is_nullable, excel_type, casper::Term::EUndefined);
            if ( casper::Term::EUndefined == model_type ) {
                throw OSAL_EXCEPTION("Type for scalar '%s' not defined!",
                                     variable_name
                );
            }
            SetDefaultTermValue(model_type, term2);
            reference_symtab_[variable_name] = term2;

        } else {

            scanner_.SetInput(value.c_str(), value.size());
            if ( scanner_.Scan(&term1, &location) == Parser::token::VAR && scanner_.Scan(&term2, &location) == '=' ) {
                switch (scanner_.Scan(&term2, &location)) {
                    case Parser::token::TEXTLITERAL:
                    case Parser::token::NUM:
                        reference_symtab_[variable_name] = term2;
                        break;
                    default:
                        break;
                }
            }

        }
    }
}

void casper::see::See::LoadFormula (const char* a_expression, const char* a_alias)
{
    try {
        casper::see::location location;
        Term term1, term2;
        int exp_len;

        temp_formula_ = new Formula();
        temp_formula_->formula_ = a_expression;
        if ( a_alias != NULL ) {
            temp_formula_->alias_ = a_alias;
        }
        exp_len = (int) strlen(a_expression);
        scanner_.SetInput(a_expression, exp_len);
        if ( scanner_.Scan(&term1, &location) == Parser::token::VAR && scanner_.Scan(&term2, &location) == '=' ) {
            temp_formula_->name_ = term1.text_;
            expression_name_ = term1.text_;
            if ( a_alias != NULL ) {
                name_to_cell_aliases_[expression_name_] = a_alias;
            }
        } else {
            expression_name_ = "";
        }

        scanner_.SetInput(a_expression, exp_len);

        check_dependencies_ = true;
        parser_.parse();
        check_dependencies_ = false;

        formulas_.push_back(temp_formula_);

        /*
         * Create a dummy entry into the symbol table, this is necessary for the dependency analysis
         */
        if ( symtab_.find(temp_formula_->name_) == symtab_.end() ) {
            Term dummy;

            symtab_[temp_formula_->name_] = dummy;
        }

        if ( a_alias != NULL && strcmp(a_alias, temp_formula_->name_.c_str()) != 0 ) {
            aliases_[a_alias] = temp_formula_->name_;
        }

        temp_formula_ = NULL;

    } catch (osal::Exception& a_exception) {

        delete temp_formula_;
        temp_formula_ = NULL;
        throw a_exception;

    } catch (...) {

        delete temp_formula_;
        temp_formula_ = NULL;
        throw OSAL_EXCEPTION_NA("unexpected exception type");

    }
}

/**
 * @brief Returns an auxiliary lookup table
 *
 * If the table does not exist #LoadTable will attempt to load it from disk
 *
 * @param a_table_name
 *
 * @return the table object
 */
casper::see::Table* casper::see::See::GetTableByName (const char* a_table_name)
{
    TableHash::iterator it;
    Table* table;

    it = tables_.find(a_table_name);
    if ( it == tables_.end() ) {
        table = LoadTable(a_table_name, NULL);
    } else {
        table = it->second;
        if ( table->IsPartialyLoaded() ) {
            LoadTable(a_table_name, table);
        }
    }
    return table;
}

/**
 * @brief Load an already prepared table
 *
 * Unloads existing table and adds the new table to the by name index
 * This method is called from the Ruby GEM to load tables directly from the Ruby Hashes
 *
 * @param a_table Table data
 */
void casper::see::See::LoadTable (casper::see::Table* a_table)
{
    std::string tbl_name;

    tbl_name = a_table->GetName();
    UnloadTable(tbl_name);
    tables_[tbl_name] = a_table;
}

/**
 * @brief Load, or complete the loading, of a table from disk
 *
 * Besides loading the data to a table object also adds it to the table index
 *
 * @param a_table_name Name of the table must match the JSON file on disk
 * @param a_partially_loaded_table an existing table that will be amended with the data from disk, or NULL
 *                                 if the table is to be fully loaded from the disk
 * @return The table object
 */
casper::see::Table* casper::see::See::LoadTable (const char* a_table_name, Table* a_partially_loaded_table)
{
    Table*        table;
    TmpJsonParser parser;
    Json::Value*  parsed_json;

    try {
        table = a_partially_loaded_table;
        std::string path = json_tables_path_;
        path += a_table_name;
        path += ".json";

        /*
         * Parse the datafile
         */
        parsed_json = parser.LoadAndParse(path.c_str());
        if ( parsed_json == NULL ) {
            throw OSAL_EXCEPTION("Table '%s' not found", a_table_name);
        }
        if ( table == NULL ) {
            table = new Table(a_table_name, false); // on a full load create a fresh table
        }
        table->Load(*parsed_json, a_table_name);

        /*
         * Last but not the least add the table to the index
         */
        if ( a_partially_loaded_table == NULL ) {
            LoadTable(table);
        }

    } catch (osal::Exception& a_exception) {
        if ( table != NULL ) {
            delete table;
        }
        throw a_exception;
    } catch (...) {
        if ( table != NULL && a_partially_loaded_table == NULL ) {
            delete table;
        }
        // Invalid version
        throw OSAL_EXCEPTION("Table '%s' not loaded or json file is invalid", a_table_name);
    }
    return table;
}


void casper::see::See::UnloadTable (const std::string& a_table_name)
{
    TableHash::iterator it;

    it = tables_.find(a_table_name);
    if ( it != tables_.end() ) {
        delete it->second;
        tables_.erase(a_table_name);
    }
}

void casper::see::See::AddDependency (Term& a_varname)
{
    temp_formula_->precedents_.insert(a_varname.text_);
}

void casper::see::See::AddAlias (Term& a_term)
{
    if ( temp_formula_->alias_.size() ) {
        aliases_[temp_formula_->alias_] = a_term.text_;
    }

    /*
     * Create a dummy entry into the symbol table, this is necessary for the dependency analysis
     */
    if ( symtab_.find(a_term.text_) != symtab_.end() ) {
        symtab_[a_term.text_] = a_term;
    }
}

/*
 * Handle line cloning
 */
bool casper::see::See::CloneLinesTableLines (StringMultiHash& a_clone_map, Json::Value& a_line_formulas, Json::Value& a_line_values)
{
    int start_row, code_col, master_row, new_row, row_delta;

    if ( a_clone_map.size() == 0 ) {
        /*
         * If there are no clone specifications we're done
         */
        return true;
    } else {
        /*
         * If there are clone specs we must have a "COD" column
         */
        ColumnHash::iterator it = columns_.find(code_col_name_);
        if ( it == columns_.end() ) {
            throw OSAL_EXCEPTION("The lines table does not have a %s column\n", code_col_name_);
        } else {
            start_row = it->second.row_;
            code_col  = it->second.col_;
        }
    }

    CloneMap   clones;
    RowShifter row_shifter;

    /*
     * Iterate clone spec to group the master and it's subclones into a ClontInfo structure
     */
    for (StringMultiHash::iterator it = a_clone_map.begin(); it != a_clone_map.end(); ++it ) {
        const char* cloned_code = it->first.c_str();
        const char* clone_name  = it->second.c_str();
        CloneInfo clone;
        int idx;

        if ( clones.find(clone_name) != clones.end() ) {
            throw OSAL_EXCEPTION("More than one code is being cloned as '%s'\n", clone_name);
        }

        idx = 0;
        clone.master_index_ = -1;
        for ( Json::Value::iterator rit = a_line_values.begin(); rit != a_line_values.end(); ++rit ) {
            int subcode;

            if ( (*rit)[code_col_name_].isString() ) {
                if ( MatchLineCode((*rit)[code_col_name_].asCString(), cloned_code, subcode) == true ) {
                    if ( subcode == 0 ) {
                        if ( clone.master_index_ != -1 ) {
                            throw OSAL_EXCEPTION("Code to clone '%s' at row %d is defined twice, first seen at %d\n",
                                                 clone.master_index_ + start_row + 1, idx + start_row + 1);
                        }
                        clone.master_index_ = idx;
                        clone.slave_clones_.push_back(SlaveCloneInfo(idx, 0));  // Slave 0 is the master, duh?
                    } else {
                        if ( clone.master_index_ == -1 ) {
                            throw OSAL_EXCEPTION("Subclone of '%s' found before it's master at row %d\n",
                                                 clone_name, idx + start_row + 1);
                        }
                        clone.slave_clones_.push_back(SlaveCloneInfo(idx, subcode));
                    }
                }
            }
            ++idx;
        }
        if ( clone.master_index_ == -1 ) {
            throw OSAL_EXCEPTION("Unable to find code '%s' to clone\n", cloned_code);
        }
        clone.cloned_code_ = cloned_code;
        clone.clone_name_  = clone_name;
        clones[clone_name] = clone;
    }

    /*
     * Now process each clone
     */
    for ( CloneMap::iterator it = clones.begin(); it != clones.end(); ++it ) {
        CloneInfo& clone = it->second;

        row_shifter.ParseSuffix(clone.clone_name_.c_str());
        row_shifter.ClearVariableDefinitions();
        DEBUGTRACE("see-calc-clone", "Clone of %s as %s:", clone.cloned_code_.c_str(), clone.clone_name_.c_str());
        DEBUGTRACE("see-calc-clone", "   Master index %d at row %d", clone.master_index_,  clone.master_index_ + start_row + 1);
        for ( std::vector<SlaveCloneInfo>::iterator slave_it = clone.slave_clones_.begin(); slave_it != clone.slave_clones_.end(); ++slave_it ) {

            Json::Value::Members members = a_line_values[slave_it->index_].getMemberNames();
            for (Json::Value::Members::iterator column_it = members.begin(); column_it != members.end(); ++column_it ) {
                row_shifter.GrabVariableDefinitions(a_line_values[slave_it->index_][column_it->c_str()].asCString());
            }

            members = a_line_formulas[slave_it->index_].getMemberNames();
            for (Json::Value::Members::iterator column_it = members.begin(); column_it != members.end(); ++column_it ) {
                row_shifter.GrabVariableDefinitions(a_line_formulas[slave_it->index_][column_it->c_str()].asCString());
            }
        }

        /*
         *
         */
        int last_row;
        master_row = last_row = row_delta = -1;

        for ( std::vector<SlaveCloneInfo>::iterator slave_it = clone.slave_clones_.begin(); slave_it != clone.slave_clones_.end(); ++slave_it ) {
            Json::Value clone_values;
            Json::Value clone_formulas;

            if ( slave_it->subcode_ == 0 ) {
                new_row    = a_line_values.size() + start_row + 1;
                master_row = new_row;
                row_delta  = a_line_values.size() - slave_it->index_;
                last_row   = new_row;
                DEBUGTRACE("see-calc-clone", "   * %s matched at row %d, idx %d rdelta %d ~~~> %d", clone.clone_name_.c_str(), slave_it->index_ + start_row + 1, slave_it->index_, row_delta, new_row);
            } else {
                new_row = (slave_it->index_ - clone.master_index_) + master_row;
                while ( last_row + 1 < new_row ) {
                    a_line_values.append(clone_values);
                    a_line_formulas.append(clone_formulas);
                    last_row++;
                    DEBUGTRACE("see-calc-clone", "   * %s adding empty row ~~> %d", clone.clone_name_.c_str(), last_row);
                }
                last_row = new_row;
                DEBUGTRACE("see-calc-clone", "   * %s matched at row %d, idx %d subcode %d, rdelta %d ~~~> %d", clone.clone_name_.c_str(), slave_it->index_ + start_row + 1, slave_it->index_, slave_it->subcode_, row_delta, new_row);
            }

            /*
             * Create clones for the line_values
             */
            Json::Value::Members members = a_line_values[slave_it->index_].getMemberNames();
            for (Json::Value::Members::iterator column_it = members.begin(); column_it != members.end(); ++column_it ) {
                if ( strcmp(column_it->c_str(), code_col_name_) == 0 ) {
                    std::string code_exp = a_line_values[slave_it->index_][column_it->c_str()].asCString();
                    size_t      start_pos = code_exp.find(clone.cloned_code_);

                    if ( start_pos == std::string::npos) {
                        throw OSAL_EXCEPTION("%s column does not contain the code???", code_col_name_);
                    }
                    code_exp.replace(start_pos, clone.cloned_code_.size(), clone.clone_name_);
                    clone_values[column_it->c_str()] = row_shifter.ShiftFormula(code_exp.c_str(), row_delta);
                } else {
                    clone_values[column_it->c_str()] = row_shifter.ShiftFormula(a_line_values[slave_it->index_][column_it->c_str()].asCString(), row_delta);
                }
            }

            /*
             * Create clones for the formula values
             */
            members = a_line_formulas[slave_it->index_].getMemberNames();
            for (Json::Value::Members::iterator column_it = members.begin(); column_it != members.end(); ++column_it ) {
                clone_formulas[column_it->c_str()] = row_shifter.ShiftFormula(a_line_formulas[slave_it->index_][column_it->c_str()].asCString(), row_delta);
            }

            DEBUGTRACE("see-calc-clone-values", "V ~~> %s", clone_values.toStyledString().c_str());
            DEBUGTRACE("see-calc-clone-values", "F ~~> %s", clone_formulas.toStyledString().c_str());

            /*
             * Append cloned values to formulas and value array
             */
            a_line_values.append(clone_values);
            a_line_formulas.append(clone_formulas);
        }
    }
    row_count_ = MAX(a_line_formulas.size(), a_line_values.size());

    OSAL_UNUSED_PARAM(code_col);

    return true;
}

#ifdef __APPLE__
#pragma mark -
#pragma mark DEPENDENCY ANALYSIS
#pragma mark -
#endif

void casper::see::See::CalculateSumDependencies ()
{
    for ( FormulaList::iterator it = formulas_.begin(); it != formulas_.end(); ++it) {
        current_formula_ = (*it);
        current_formula_->CalculateDependencies(*this);
    }
}

void casper::see::See::SortDependencies ()
{
    typedef lemon::ListDigraph              GR;
    typedef std::map<std::string, GR::Node> NodeMap;
    typedef GR::ArcMap<int>                 ArcCost;

    std::vector<Formula*> tmp_formulas;        // Copy of the formula array
    GR                    dep_graph;           // Formula dependency graph
    GR::NodeMap<int>      order(dep_graph);    // Helper map for topological sorting
    NodeMap               name_to_node;        // Hash to track nodes by formula name
    ArcCost               arc_cost(dep_graph);
    int                   formula_count;

    precedents_.clear();

    /*
     * Create a graph node for each formula and save the formula pointer
     * in the temporary array. Keep track of the node id assigned to each name
     */
    formula_count = (int) formulas_.size() - 1;
    for ( unsigned i = 0; i < formulas_.size(); ++i ) {
        GR::Node node = dep_graph.addNode();
        name_to_node[formulas_[i]->name_] = node;
        tmp_formulas.push_back(formulas_[i]);
    }

    /*
     * Update aliased cell references
     */
    for ( unsigned i = 0; i < formulas_.size(); ++i ) {
        StringSet to_remove;

        for (StringSet::iterator iter = formulas_[i]->precedents_.begin(); iter != formulas_[i]->precedents_.end(); ++iter ) {

            StringHash::iterator ait = aliases_.find(*iter);
            if ( ait != aliases_.end() && strcmp(iter->c_str(), ait->second.c_str()) != 0 ) {
                formulas_[i]->precedents_.insert(ait->second);
                to_remove.insert(*iter);
            }
        }
        for ( StringSet::iterator iter = to_remove.begin(); iter != to_remove.end(); ++iter ) {
            formulas_[i]->precedents_.erase(*iter);
        }
    }

    /*
     * Visit the formula precendent lists to add dependency arcs to the graph
     */
    for ( unsigned i = 0; i < formulas_.size(); ++i ) {
        StringSet& precedents = formulas_[i]->precedents_;

        GR::Node& formula_node = name_to_node[formulas_[i]->name_];

        for ( StringSet::iterator it = precedents.begin(); it != precedents.end(); ++it ) {
            NodeMap::iterator bn_it = name_to_node.find(*it);
            if ( bn_it != name_to_node.end() ) {
                GR::Arc arc = dep_graph.addArc(formula_node, bn_it->second);
                arc_cost.set(arc, 1);
            } else {
                if ( aliases_.find(*it) == aliases_.end() ) {
                    precedents_.insert(*it);
                }
            }
        }
    }

    /*
     * Make sure the dependency graph is acyclic
     */
    if ( lemon::checkedTopologicalSort(dep_graph, order) == false ) {
        std::string                           circular_dep_report;
        lemon::Path<GR>                       path;
        lemon::HartmannOrlinMmc<GR, ArcCost > hart(dep_graph, arc_cost);

        hart.cycle(path).run();

        circular_dep_report = "Found circular dependency in model:\n";

        for ( int i = 0; i < path.length(); ++i) {
            GR::Arc arc = path.nth(i);

            circular_dep_report += "  Formula: '";
            circular_dep_report += tmp_formulas[dep_graph.id(dep_graph.source(arc))]->formula_;
            circular_dep_report += "'\n      <= '";
            circular_dep_report += tmp_formulas[dep_graph.id(dep_graph.target(arc))]->formula_.c_str();
            circular_dep_report += "'\n";
        }
        fprintf(stderr, "\n%s\n", circular_dep_report.c_str());
        throw OSAL_EXCEPTION_NA(circular_dep_report.c_str());
    }

    /*
     * Copy the formula pointers back to array using the topological sort ordering,
     * each array entry depends only on the items with lower indexes
     */
    for (GR::NodeIt n(dep_graph); n != lemon::INVALID; ++n) {
        formulas_[formula_count - order[n]] = tmp_formulas[dep_graph.id(n)];
    }
}

#ifdef __APPLE__
#pragma mark -
#pragma mark ::: FORMULA CALCULATION :::
#pragma mark -
#endif

/**
 * @brief Public entry point to calculate one expression
 *
 * @param a_expression expression string
 */
void casper::see::See::Calculate (const char* a_expression, size_t a_len)
{
    casper::see::location location;
    Term term1, term2;

    result_.type_   = Term::ENan;
    result_.number_ = 0;
    result_.text_   = "";

    scanner_.SetInput(a_expression, a_len);
    if ( scanner_.Scan(&term1, &location) == Parser::token::VAR && scanner_.Scan(&term2, &location) == '=' ) {
        expression_name_ = term1.text_;
    } else {
        expression_name_ = "";
    }
    scanner_.SetInput(a_expression, a_len);
    check_dependencies_ = false;
    parser_.parse();
}

/**
 * @brief Public entry point to calculate the whole model
 *
 * @param a_params
 */
void casper::see::See::CalculateAll (const Json::Value& a_params)
{
    Term val;

    /*
     * Clear the symbol table and reload the "static" symbols created when the model was loaded
     */
    symtab_.clear();
    for ( SymbolTable::iterator it = reference_symtab_.begin(); it != reference_symtab_.end(); ++it ) {
        symtab_[it->first] = it->second;
    }

    /*
     * Clear tracked lookups.
     */
    track_filter_params_ = a_params;
    for ( auto lt_it : lt_tables_ ) {
        const auto table_it = tables_.find(lt_it.first);
        if ( tables_.end() != table_it ) {
            table_it->second->SetTrackLookups(lt_it.second, track_filter_params_);
        }
    }

    // ... for all object members ...
    bool        is_nullable = false;
    std::string excel_type  = "";
    for ( auto member : a_params.getMemberNames() ) {
        // ... pick and keep track if it's value  ...
        const Json::Value& tmp_field = a_params[member];
        switch (tmp_field.type()) {
            case Json::ValueType::nullValue:
            {
                casper::Term default_value = casper::Term(casper::Term::EUndefined);
                const int model_type = GetParamType(member.c_str(), nullptr, is_nullable, excel_type, casper::Term::EUndefined);
                if ( casper::Term::EUndefined == model_type ) {
                    throw OSAL_EXCEPTION("Type for scalar '%s' not defined!",
                                         member.c_str()
                    );
                }
                SetDefaultTermValue(model_type, default_value);
                symtab_[member] = default_value;
                break;
            }
            case Json::ValueType::intValue:
                symtab_[member] = static_cast<double>(tmp_field.asInt64());
                break;
            case Json::ValueType::uintValue:
                symtab_[member] = static_cast<double>(tmp_field.asUInt64());
                break;
            case Json::ValueType::realValue:
                symtab_[member] = static_cast<double>(tmp_field.asDouble());
                break;
            case Json::ValueType::stringValue:
                symtab_[member] = tmp_field.asString();
                break;
            case Json::ValueType::booleanValue:
                symtab_[member] = tmp_field.asBool();
                break;
            default:
                throw OSAL_EXCEPTION("Unexpected scalar type %d for member name '%s'!", tmp_field.type(), member.c_str());
        }
    }

    CalculateAll();
}

void casper::see::See::CalculateAll ()
{
    osal::utils::Swatch tf;
    int32_t col;
    int32_t row;


    if ( has_template_lines_ == true && 0 == lines_clones_count_ ) {
        return;
    }

    tf.Start();

    if ( 0 != log_file_name_.length() && nullptr == log_file_ ) {
        log_file_ = fopen(log_file_name_.c_str(), "w");
    }

    for ( unsigned i = 0; i < formulas_.size(); ++i ) {

        // ... log formulas ...
        if ( nullptr != log_file_ ) {
            fprintf(log_file_, "--- %s[%s] : %s ---\n", formulas_[i]->name_.c_str(), formulas_[i]->alias_.c_str(), formulas_[i]->type_.c_str());
            fprintf(log_file_, "%s\n", formulas_[i]->formula_.c_str());
            for ( auto precedent : formulas_[i]->precedents_ ) {
                const char* cell_ref;
                const auto n_it = name_to_cell_aliases_.find(precedent.c_str());
                std::string colname = "";

                if ( name_to_cell_aliases_.end() != n_it ) {
                    cell_ref = n_it->second.c_str();
                } else {
                    cell_ref = precedent.c_str();
                }
                if ( Sum::ParseCellRef(cell_ref, &col, &row) ) {
                    if ( row >= (int32_t) (lines_clones_offset_ -1) && row < (int32_t) (lines_clones_offset_ + lines_clones_count_) ) {
                        const auto cnit = column_name_index_.find(col);

                        if ( cnit != column_name_index_.end() ) {
                            colname = "(@" + cnit->second + ")";
                        }
                     }
                }
                const auto l_it = symtab_.find(precedent.c_str());
                if ( symtab_.end() != l_it ) {
                    fprintf(log_file_, "\t%s%s=%s\n", precedent.c_str(), colname.c_str(), l_it->second.DebugString().c_str());
                } else {
                    fprintf(log_file_, "\t%s=%s\n", precedent.c_str(), "<wtf>");
                }
            }
            fflush(log_file_);
        }
        // ... end of formulas logging ...

        if ( formulas_[i]->IsSum() ) {
            Term sum;

            sum.number_ = formulas_[i]->SumAllTerms(symtab_, log_file_);
            sum.type_   = Term::ENumber;

            symtab_[formulas_[i]->name_] = sum;
            result_ = sum;

        } else {
            current_formula_ = formulas_[i];
            Calculate(current_formula_->formula_.c_str(), current_formula_->formula_.size());
        }

        DEBUGTRACE("see-calc", "%-150.150s %s", formulas_[i]->formula_.c_str(), symtab_[formulas_[i]->name_.c_str()].ToString().c_str());

        if ( nullptr != log_file_ ) {
            fprintf(log_file_, "%s=%s\n", formulas_[i]->name_.c_str(), result_.DebugString().c_str());
            fflush(log_file_);
        }

    }

    if ( nullptr != log_file_ ) {
        fflush(log_file_);
    }

    tf.Stop();
    printf("Calculation time %ld ms\n", tf.Ticks() / 1000);
}

void casper::see::See::GetVariable (Term& a_result,  Term& a_varname, casper::see::location&)
{
    if ( check_dependencies_ ) {
        AddDependency(a_varname);
    } else {
        SymbolTable::const_iterator it = symtab_.find(a_varname.text_);

        if ( it != symtab_.end() ) {
            a_result = it->second;
        } else {
            StringHash::const_iterator sit = aliases_.find(a_varname.text_);
            if ( sit == aliases_.end() ) {
                throw OSAL_EXCEPTION("Variable %s not found", a_varname.text_.c_str());
            } else {
                it = symtab_.find(sit->second);
                if ( it == symtab_.end() ) {
                    throw OSAL_EXCEPTION("Variable %s not found", a_varname.text_.c_str());
                }
                a_result = it->second;
            }
        }

    }
}

void casper::see::See::SetVariable (Term& a_varname, Term& a_value, casper::see::location&)
{
    if ( check_dependencies_ ) {
        AddAlias(a_varname);
    } else {
        symtab_[a_varname.text_] = a_value;
    }
}

#ifdef __APPLE__
#pragma mark -
#pragma mark ::: SUMS, SUMIF & SUMIFS :::
#pragma mark -
#endif

#ifdef __APPLE__
#pragma mark ... SUM
#endif

void casper::see::See::Sum (Term& a_result, Term& a_vector_ref)
{
    char sztmp[300];
    snprintf(sztmp, sizeof(sztmp), "SUM(LINES[%s])", a_vector_ref.aux_text_.c_str());

    if ( check_dependencies_ ) {
        if ( a_vector_ref.text_ == "LINES" ) {
            SymbolTable::iterator sym_it;
            ColumnHash::iterator  it;
            Term dummy;

            it = columns_.find(a_vector_ref.aux_text_);
            if ( it == columns_.end() ) {
                throw OSAL_EXCEPTION("Column '%' does not exist in the model's LINES table", a_vector_ref.aux_text_.c_str());
            }
            /*
             * Insert a dependency into the formula being analysed, used the conventioned sum name
             */
            temp_formula_->precedents_.insert(sztmp);

            sym_it = symtab_.find(sztmp);
            if ( sym_it != symtab_.end() ) {
                return;
            }

            /*
             * Create a SUM formula and add to the formula list
             */
            class Sum* sum = new class Sum(it->second.row_, it->second.col_, row_count_); // @TODO bigger ROW LIMIT w/o O(n) complexity
            sum->name_     = sztmp;
            sum->formula_  = sztmp;
            formulas_.push_back(sum);
            symtab_[sztmp] = dummy;

        } else {
            // Sums of auxiliary tables do not inject dependencies
        }
    } else {
        if ( a_vector_ref.text_ == "LINES" ) {
            a_result = symtab_[sztmp];
        } else {
            a_result = GetTableByName(a_vector_ref.text_.c_str())->SumColumn(a_vector_ref.aux_text_.c_str());
        }
    }
}

void casper::see::See::Sum (Term& a_result, Term& a_cell_start, Term& a_cell_end)
{
    char sztmp[300];
    snprintf(sztmp, sizeof(sztmp), "SUM(%s:%s)", a_cell_start.text_.c_str(), a_cell_end.text_.c_str());

    if ( check_dependencies_ ) {
        /*
         * Insert a dependency into the formula being analysed, used the convetioned sum name
         */
        temp_formula_->precedents_.insert(sztmp);

        // TODO avoid formula duplicates ?

        /*
         * Create a SUM formula and add to the formula list
         */
        class Sum* sum      = new class Sum(a_cell_start.text_.c_str(), a_cell_end.text_.c_str());
        sum->name_    = sztmp;
        sum->formula_ = sztmp;
        formulas_.push_back(sum);
    } else {
        a_result = symtab_[sztmp];
    }
}

#ifdef __APPLE__
#pragma mark ... SUMIF
#endif

void casper::see::See::SumIf (Term& a_result, const casper::Term& a_range, const casper::Term& a_criteria,
                              const char* const a_formula, const size_t& a_formula_length)
{
#if 1
    // ... not working yet ...
    OSAL_UNUSED_PARAM(a_result);
    OSAL_UNUSED_PARAM(a_range);
    OSAL_UNUSED_PARAM(a_criteria);
    OSAL_UNUSED_PARAM(a_formula);
    OSAL_UNUSED_PARAM(a_formula_length);
    throw OSAL_EXCEPTION_NA("SUMIF is no supported!");
#else
    TableHash::iterator it;
    std::string         table_name;
    std::string         lookup_col;

    /*
     * Try to re-parse vector references that passed the parser as text
     */
    Sum::ParseTableColRef(a_range.text_.c_str(), &table_name, &lookup_col);

    if ( 0 == strcasecmp(table_name.c_str(), "lines") ) {
        SumIfOnLinesTable(a_result, lookup_col.c_str(), a_criteria,
                          a_formula, a_formula_length);
    } else {
        if ( false == check_dependencies_ ) {
            // TODO: GetTableByName(table_name.c_str())->SumIf(a_result, lookup_col.c_str(), sum_criterias_);
            throw OSAL_EXCEPTION("SUMIF for table '%s' not implemented!", table_name.c_str());
        } else {
            /*
             * During dependency analysis this is a no-operation
             */
        }
    }
    sum_criterias_.clear();
#endif
}

void casper::see::See::SumIf (Term& a_result, const casper::Term& a_range, const casper::Term& a_criteria, const casper::Term& a_sum_range,
                              const char* const a_formula, const size_t& a_formula_length)
{

    OSAL_UNUSED_PARAM(a_range);
    OSAL_UNUSED_PARAM(a_formula);
    OSAL_UNUSED_PARAM(a_formula_length);

    TableHash::iterator it;
    std::string         table_name;
    std::string         lookup_col;

    /*
     * Try to re-parse vector references that passed the parser as text
     */
    Sum::ParseTableColRef(a_sum_range.text_.c_str(), &table_name, &lookup_col);

    /*
     * If the colums are empty grab the value decomposed by the parser
     */
    if ( lookup_col.size() == 0 ) {
        lookup_col = a_sum_range.aux_text_;
    }

    if ( 0 == strcasecmp(table_name.c_str(), "lines") ) {
        SumIfOnLinesTable(a_result, lookup_col.c_str(), a_criteria,
                          a_formula, a_formula_length);
    } else {
        if ( false == check_dependencies_ ) {
            // GetTableByName(table_name.c_str())->SumIfs(a_result, lookup_col.c_str(), sum_criterias_);
            throw OSAL_EXCEPTION("SUMIF for table '%s' not implemented!", table_name.c_str());
        } else {
            /*
             * During dependency analysis this is a no-operation
             */
        }
    }
    sum_criterias_.clear();
}

#ifdef __APPLE__
#pragma mark ... SUMIFS
#endif

void casper::see::See::SumIfs (Term& a_result, Term& a_sum_range)
{
    TableHash::iterator it;
    std::string         table_name;
    std::string         lookup_col;

    /*
     * Try to re-parse vector references that passed the parser as text
     */
    Sum::ParseTableColRef(a_sum_range.text_.c_str(), &table_name, &lookup_col);

    /*
     * If the colums are empty grab the value decomposed by the parser
     */
    if ( lookup_col.size() == 0 ) {
        lookup_col = a_sum_range.aux_text_;
    }

    if ( "LINES" == table_name ) {
        SumIfsOnLinesTable(a_result, lookup_col.c_str(), sum_criterias_);
    } else {
        if ( false == check_dependencies_ ) {
            GetTableByName(table_name.c_str())->SumIfs(a_result, lookup_col.c_str(), sum_criterias_);
        } else {
            /*
             * During dependency analysis this is a no-operation
             */
        }
    }
    sum_criterias_.clear();
}

#ifdef __APPLE__
#pragma mark ... SUMIF & SUMIFS HELPERS
#endif

void casper::see::See::SumIfOnLinesTable (Term& a_result,  const char* a_sum_column, const Term& a_criteria,
                                          const char* const a_formula, const size_t& a_formula_length)
{
    casper::see::Formula* formula = ( true == check_dependencies_ ? temp_formula_ : current_formula_ );
    if ( nullptr == formula ) {
        throw OSAL_EXCEPTION_NA("Unable to calculate SUMIF - formula is nullptr!");
    }

    const std::string tmp_formula = std::string(a_formula, a_formula_length);
    const std::string key         = tmp_formula;

    if ( true == check_dependencies_ ) {

        /*
         * Insert a dependency into the formula being analysed, used the conventioned sum name
         */
        formula->precedents_.insert(key);

        /*
         * Prevent duplicates
         */
        const auto sym_it = symtab_.find(key);
        if ( sym_it != symtab_.end() ) {
            return;
        }

        /*
         * Create a SUM formula and add to the formula list
         */
        casper::see::SumIf* sum = new casper::see::SumIf(a_sum_column, a_criteria.aux_text_.c_str());
        sum->name_     = key;
        sum->formula_  = tmp_formula;
        formulas_.push_back(sum);
    } else {
        /*
         * Use the cached value if available, if not calculate with the current criterias.
         */
        SymbolTable table ;
        table[a_criteria.text_] = a_criteria;

        if ( symtab_.find(key) == symtab_.end() ) {
            a_result = current_formula_->SumIfAllTerms(symtab_, table, log_file_);
            symtab_[key] = a_result;
        } else {
            a_result = symtab_[key];
        }
    }
}

void casper::see::See::SumIfsOnLinesTable (Term& a_result, const char* a_sum_column, SymbolTable& a_criterias)
{
    ColumnHash::iterator index_it;
    char                 sztmp[300];
    int                  len;


    len = snprintf(sztmp, sizeof(sztmp), "SUMIFS(LINES[%s]", a_sum_column);
    for (SymbolTable::iterator criteria_it = a_criterias.begin(); criteria_it != a_criterias.end(); ++criteria_it ) {
        len += snprintf(sztmp + len, sizeof(sztmp) - len, ",LINES[%s],%s" , criteria_it->first.c_str(), criteria_it->second.ToString().c_str());
    }
    len += snprintf(sztmp + len, sizeof(sztmp) - len, ")");

    if ( true == check_dependencies_ ) {
        SymbolTable::iterator sym_it;
        Term dummy;

        /*
         * Insert a dependency into the formula being analysed, used the conventioned sum name
         */
        temp_formula_->precedents_.insert(sztmp);

        /*
         * Prevent duplicates
         */
        sym_it = symtab_.find(sztmp);
        if ( sym_it != symtab_.end() ) {
            return;
        }

        /*
         * Create a SUM formula and add to the formula list
         */
        class SumIfs* sum = new class SumIfs(a_sum_column, a_criterias);
        sum->name_     = sztmp;
        sum->formula_  = sztmp;
        formulas_.push_back(sum);
    } else {
        /*
         * Use the cached value if available, if not calculate with the current criterias.
         */
        if ( symtab_.find(sztmp) == symtab_.end() ) {
            a_result = current_formula_->SumIfAllTerms(symtab_, a_criterias, log_file_);
            symtab_[sztmp] = a_result;
        } else {
            a_result = symtab_[sztmp];
        }
    }
}

#ifdef __APPLE__
#pragma mark -
#pragma mark ::: TABLE LOOKUPS :::
#pragma mark -
#endif

#ifdef __APPLE__
#pragma mark ... OFFSET
#endif

void casper::see::See::Offset (casper::Term& o_result, const casper::Term& a_ref, const casper::Term& a_rows, const casper::Term& a_cols)
{
    o_result.type_   = casper::Term::ERef;
    o_result.number_ = NAN;

    casper::see::Formula* formula = ( true == check_dependencies_ ? temp_formula_ : current_formula_ );
    if ( nullptr == formula ) {
        throw OSAL_EXCEPTION_NA("Unable to calculate cell reference offset - formula is nullptr!");
    }

    if ( false == a_rows.IsNumber() || true == a_rows.IsNan() ) {
        throw OSAL_EXCEPTION_NA("Unable to calculate cell reference offset - number of rows is invalid!");
    } else if ( false == a_cols.IsNumber() || true == a_cols.IsNan() ) {
        throw OSAL_EXCEPTION_NA("Unable to calculate cell reference offset - number of columns is invalid!");
    }

    int32_t col;
    int32_t row;

    const auto target_column_info_it = columns_.find(a_ref.aux_text_);
    if ( columns_.end() == target_column_info_it ) {
        throw OSAL_EXCEPTION("Unable to parse cell reference '%s' - target column info for '%' not found!", a_ref.aux_text_.c_str());
    }

    if ( false == Sum::ParseCellRef(formula->name_.c_str(), &col, &row) ) {
        if ( false == Sum::ParseCellRef(formula->alias_.c_str(), &col, &row) ) {
            throw OSAL_EXCEPTION("Unable to parse cell reference '%s' - could not find reference cell!", a_ref.aux_text_.c_str());
        }
    }

    col = static_cast<int32_t>(target_column_info_it->second.col_);

    if ( ( static_cast<int>(row) + a_rows.number_ ) < lines_clones_offset_ ) {
        return;
    }

    char cell_ref[20] = { 0 };

    see::Sum::MakeRowColRef(cell_ref, static_cast<int>(row) + static_cast<int>(a_rows.number_), static_cast<int>(col) + static_cast<int>(a_cols.number_));

    if ( true == check_dependencies_ ) {
        formula->precedents_.insert(cell_ref);
    } else {
        StringHash::iterator cell_ref_to_name_it = name_to_cell_aliases_.find(cell_ref);
        if ( name_to_cell_aliases_.end() != cell_ref_to_name_it ) {
            const auto value_it = symtab_.find(cell_ref_to_name_it->first);
            if ( symtab_.end() != value_it ) {
                o_result = value_it->second;
            } else {
                auto it_2 = line_values_.find(cell_ref);
                if ( line_values_.end() != it_2 ) {
                    o_result = it_2->second;
                } else {
                    o_result = symtab_[cell_ref];
                }
            }
        } else {
            o_result = symtab_[cell_ref];
        }
    }
}

#ifdef __APPLE__
#pragma mark ... LOOKUP
#endif

void casper::see::See::Lookup (Term& a_result, Term& a_value, Term& a_lookup_vector, Term& a_result_vector)
{
    TableHash::iterator it;
    std::string         table_name;
    std::string         lookup_col;
    std::string         result_col;

    /*
     * During dependency analysis this is a no-operation
     */
    if ( true == check_dependencies_ ) {
        return;
    }

    /*
     * Try to re-parse vector references that passed the parser as text
     */
    Sum::ParseTableColRef(a_lookup_vector.text_.c_str(), &table_name, &lookup_col);
    Sum::ParseTableColRef(a_result_vector.text_.c_str(), &table_name, &result_col);

    /*
     * If the colums are empty grab the value decomposed by the parser
     */
    if ( lookup_col.size() == 0 ) {
        lookup_col = a_lookup_vector.aux_text_;
    }
    if ( result_col.size() == 0 ) {
        result_col = a_result_vector.aux_text_;
    }

    GetTableByName(table_name.c_str())->Lookup(a_result, a_value, lookup_col.c_str(), result_col.c_str());
}

#ifdef __APPLE_
#pragma mark - ... VLOOKUP
#endif

void casper::see::See::Vlookup (Term& a_result,  Term& a_value, Term& a_lookup_vector, Term& a_col_index, bool a_range_lookup,
                                const char* const a_formula, const size_t& a_formula_length)
{
    OSAL_UNUSED_PARAM(a_formula);
    OSAL_UNUSED_PARAM(a_formula_length);

    TableHash::iterator it;
    std::string         table_name;
    std::string         lookup_col;
    std::string         result_col;

    /*
     * Try to re-parse vector references that passed the parser as text
     */
    Sum::ParseTableColRef(a_lookup_vector.text_.c_str(), &table_name, &lookup_col);

    /*
     * If the colums are empty grab the value decomposed by the parser
     */
    if ( lookup_col.size() == 0 ) {
        lookup_col = a_lookup_vector.aux_text_;
    }

    if ( 0 == strcmp(table_name.c_str(), "lines") ) {
#if 1
        // ... not working yet ...
        throw OSAL_EXCEPTION("VLOOKUP on '%s' table is no supported!", table_name.c_str());
#else
        LinesVlookup(a_result, a_value, lookup_col.c_str(), a_col_index, a_range_lookup,
                     a_formula, a_formula_length);
#endif
    } else {
        /*
         * During dependency analysis this is a no-operation
         */
        if ( true == check_dependencies_ ) {
            return;
        }
        GetTableByName(table_name.c_str())->Vlookup(a_result, a_value, lookup_col.c_str(), a_col_index, a_range_lookup);
    }
}

#ifdef __APPLE__
#pragma mark ... 'LINES' TABLE VALUE ACCESS
#endif

void casper::see::See::GetLinesTableValue (Term& a_result, Term& a_column_name)
{
    ColumnHash::iterator cit;
    StringHash::iterator it;
    const char*          symbol_name;
    char                 cellref[20];
    int32_t              col, row;

    cit = columns_.find(a_column_name.text_);
    if ( cit == columns_.end() ) {
        throw OSAL_EXCEPTION("LINES table does not have any column named '%s'", a_column_name.text_.c_str());
    }

    /*
     * we need to figure out this expression row, 1st let's see if the expression is a cell reference
     */
    if ( Sum::ParseCellRef(expression_name_.c_str(), &col, &row) == false ) {
        /*
         * From the expression name try to grab the reference name
         */
        it = name_to_cell_aliases_.find(expression_name_);
        if ( it == name_to_cell_aliases_.end() || Sum::ParseCellRef(it->second.c_str(), &col, &row) == false ) {
            throw OSAL_EXCEPTION("Unable to find the row to which expression '%s' belongs", expression_name_.c_str());
        }
    }

    /*
     * Now that we know the row of the lines table combine the row with the column numeric value
     * this will give is the cell reference, either it or the aliased value must give us the symboltable key
     */
    Sum::MakeRowColRef(cellref, row, cit->second.col_);

    it = aliases_.find(cellref);
    if ( it != aliases_.end() ) {
        symbol_name = it->second.c_str();
    } else {
        symbol_name = cellref;
    }

    if ( check_dependencies_ ) {
        temp_formula_->precedents_.insert(symbol_name);
    } else {
        a_result = symtab_[symbol_name];
    }
}

#ifdef __APPLE__
#pragma mark - ... MATCH
#endif

void casper::see::See::TableHeaderMatch (Term& a_result, Term& a_colname, Term& a_tablename)
{
    int rv;

    if ( strcmp(a_tablename.GetText(), "lines") == 0 ) {

        const auto it = columns_.find(a_colname.GetText());
        if ( columns_.end() != it ) {
            rv = it->second.col_;
        } else {
            rv = -1;
        }

    } else {
        Table* table = casper::see::See::GetTableByName(a_tablename.ToString().c_str());
        rv = table->GetColumnIndex(a_colname.ToString().c_str());
    }

    if ( rv == -1 ) {
        throw OSAL_EXCEPTION("'%s' table does not have any column named '%s'", a_tablename.ToString().c_str(), a_colname.ToString().c_str());
    } else {
        a_result = (double) (rv + 1);
    }
}

#ifdef __APPLE__
#pragma mark -
#pragma mark ::: DEBUG HELPERS :::
#pragma mark -
#endif

/**
 * @brief Helper to unit test row shifter function
 *
 * @param a_expression Expression to shift
 * @param a_row_shift Number of rows to add to cell references
 *
 * @return The shifted expression
 */
const char* casper::see::See::RowShiftTesthook (const char* a_expression, int a_row_shift)
{
    static RowShifter rs;  // This is not thread safe but ok just for testing

    return rs.ShiftFormula(a_expression, a_row_shift);
}

#ifdef __APPLE__
#pragma mark -
#pragma mark ::: TYPED MODEL HELPERS :::
#pragma mark -
#endif

int casper::see::See::GetParamType (const char* const a_key, const casper::Term* a_term,
                                    bool& o_is_nullable, std::string& o_excel_type,
                                    const int a_default)
{
    o_is_nullable = false;
    o_excel_type  = "";
    const char* type_search_key = a_key;
    for ( uint8_t attempt = 0 ; attempt < 2 ; ++ attempt ) {
        const auto type_it = scalars_types_map_.find(type_search_key);
        if ( scalars_types_map_.end() != type_it ) {
            o_is_nullable = type_it->second.nullable_;
            o_excel_type  = type_it->second.excel_;
            return type_it->second.term_;
        } else if ( type_search_key == a_key ){
            const auto it = name_to_cell_aliases_.find(a_key);
            if ( name_to_cell_aliases_.end() != it ) {
                type_search_key = it->second.c_str();
                continue;
            } else if ( nullptr != a_term ) {
                return  a_term->GetType();
            }
        } else {
            break;
        }
    }
    return a_default;
}

int casper::see::See::GetLineColumnType (const char* const a_ref, const char* const a_name,
                                         const casper::Term* a_term,
                                         bool& o_is_nullable, std::string& o_excel_type)
{
    o_is_nullable = false;
    o_excel_type  = "";
    // ... 'lines' ....
    const auto type_by_ref_it = lines_columns_types_map_.find(a_ref);
    if ( lines_columns_types_map_.end() != type_by_ref_it ) {
        o_is_nullable = type_by_ref_it->second.nullable_;
        o_excel_type  = type_by_ref_it->second.excel_;
        return type_by_ref_it->second.term_;
    } else {
        const auto type_by_name_it = lines_columns_types_map_.find(a_name);
        if ( lines_columns_types_map_.end() != type_by_name_it ) {
            o_is_nullable = type_by_name_it->second.nullable_;
            o_excel_type  = type_by_name_it->second.excel_;
            return type_by_name_it->second.term_;
        } else if ( nullptr != a_term ) {
            return a_term->GetType();
        }
    }
    return casper::Term::EUndefined;
}

#ifdef __APPLE__
#pragma mark -
#pragma mark ::: CALCULATION RESULTS EXTRACTION :::
#pragma mark -
#endif

int casper::see::See::RewindScalarIterator ()
{
    scalar_it_ = symtab_.begin();
    return row_count_;
}

bool casper::see::See::GetNextScalar (Json::Value& o_scalars)
{
    const char* cellref;
    int         row, col;
    bool        is_nullable;
    std::string excel_type;

    while ( scalar_it_ != symtab_.end() ) {
        StringHash::iterator cell_it = name_to_cell_aliases_.find(scalar_it_->first.c_str());
        if ( cell_it != name_to_cell_aliases_.end() ) {
            cellref = cell_it->second.c_str();
        } else {
            cellref = scalar_it_->first.c_str();
        }
        if ( Sum::ParseCellRef(cellref, &col, &row) == true && row < table_header_row_ ) {
            Json::Value scalar;

            auto type_it = scalars_types_map_.find(cellref);
            if ( type_it == scalars_types_map_.end() ) {
                throw OSAL_EXCEPTION("scalar %s type info not found", scalar_it_->first.c_str());
            }

            if ( scalar_it_->second.HasError() && not (scalar_it_->second.IsNull() && type_it->second.nullable_) ) {
                throw OSAL_EXCEPTION("EXCEL_ERROR @scalar %s = %s", scalar_it_->first.c_str(), scalar_it_->second.ErrorMsg());
            }

            GetParamType(cellref, nullptr, is_nullable, excel_type);

            SerializeTermToJSONValue(scalar_it_->second, type_it->second.term_, excel_type, scalar);

            o_scalars[(char*) scalar_it_->first.c_str()] = scalar;
            ++scalar_it_;
            break;
        }
        ++scalar_it_;
    }
    return scalar_it_ != symtab_.end();
}


/**
 * @brief Serialize 'scalars' to a JSON object.
 *
 * @param o_object
 */
void casper::see::See::SerializeScalarsToJSONObject (Json::Value& o_object)
{
    RewindScalarIterator();
    o_object = Json::Value(Json::ValueType::objectValue);

    while ( GetNextScalar(o_object) == true) {
        /* empty */
    }
}

/**
 * @brief Helper to sort output codes
 *
 * @param a_lhs The first item to compare
 * @param a_rhs The second item to compare
 *
 * @return true if item LHS preceeds RHS
 */
bool casper::see::See::CodeComparator (const casper::see::CodeInfo& a_lhs, const casper::see::CodeInfo& a_rhs)
{
    if ( a_lhs.is_note_ && a_rhs.is_note_ == false ) {
        return false;
    } else if ( a_lhs.is_note_ == false && a_rhs.is_note_  ) {
        return true;
    } else if ( a_lhs.is_note_ && a_rhs.is_note_ ) {
        return a_lhs.index_ < a_rhs.index_;
    }
    if ( a_lhs.master_index_ == a_rhs.master_index_ ) {
        return a_lhs.index_ < a_rhs.index_;
    } else {
        return a_lhs.master_index_ < a_rhs.master_index_;
    }
}

/**
 * @brief Prepare the iterator to retrieve the filtered output rows
 *
 * After calling this function the valid lines are retrieved by calling #GetNextLine()
 *
 * @return The number of valid lines int the filtered output
 *
 * @note This iterator was made for the payslips, the rows must have a "CONDICAO" column equal diferent from zero.
 */
int casper::see::See::RewindPaySlipRowIterator ()
{
    int                   row, code_col, condition_col;
    ColumnHash::iterator  column_it;
    Term                  *condition, *code;
    IntHash               master_code_hash;
    std::string           master_code;
    bool                  sort_lines;
    bool                  is_note;

    if ( columns_.find(code_col_name_) == columns_.end() || columns_.find(condition_col_name_) == columns_.end() ) {
        throw OSAL_EXCEPTION("Sorry, this ROW iterator requires %s and %s columns on the 'lines' excel table", code_col_name_, condition_col_name_);
    }

    sort_lines = false;
    code_list_.clear();
    row           = columns_.begin()->second.row_ + 1;
    code_col      = columns_[code_col_name_].col_;
    condition_col = columns_[condition_col_name_].col_;

    /*
     * Traverse the output lines to separate chaff from wheat
     */
    for ( int idx = 0; idx < row_count_; ++idx, ++row ) {
        const char* line_code;
        const char* sub_sep;
        const char* dot_sep;

        /*
         * Filter lines without code, pre-parse the code
         */
        code = (Term*) GetCell(row, code_col);
        if ( code == NULL || code->ToString().size() == 0 ) {
            continue;
        }
        line_code = code->ToString().c_str();
        sub_sep   = strchr(line_code, '_');
        dot_sep   = strchr(line_code, '.');
        if ( sub_sep == NULL && dot_sep == NULL ) {
            master_code_hash[line_code] = idx;  // Take note of the master index for this code
        }

        /*
         * Filter out lines with zero'ed or undefined condition
         */
        condition = (Term*) GetCell(row, condition_col);
        if ( condition == NULL ) {
            continue;
        }
        if ( condition->ToNumber() == 0.0 ) {
            continue;
        }

        /*
         * If 'N' notes lines are active we'll need to sort anyway
         */
        if ( NULL != sub_sep && 'N' == sub_sep[1] ) {
            sort_lines = true;
            is_note = true;
        } else {
            is_note = false;
        }

        /*
         * Grab the master code index
         */
        if ( dot_sep != NULL ) {
            sort_lines = true;
            master_code = std::string(line_code, dot_sep - line_code);
        } else if ( sub_sep != NULL ) {
            master_code = std::string(line_code, sub_sep - line_code);
        } else {
            master_code = line_code;
        }
        IntHash::iterator it = master_code_hash.find(master_code);
        if ( it == master_code_hash.end() ) {
            throw OSAL_EXCEPTION("Cloned code '%s' does not have a corresponding source code", line_code);
        }
        code_list_.push_back(CodeInfo(line_code, it->second, idx, is_note));
    }
    if ( sort_lines ) {
        std::sort(code_list_.begin(), code_list_.end(), CodeComparator);
    }
    line_it_ = code_list_.begin();
    return (int) code_list_.size();
}

/**
 * @brief Return the next row from the lines output
 *
 * Before calling this the line_it_ iterator must be prepared with a call to #RewindPaySlipRowIterator()
 *
 * @param o_row json object with the row column
 *
 * @return true if the row was delivered to term hash, false if there are no more rows to return
 */
bool casper::see::See::GetNextLine (Json::Value& o_row)
{
    if ( line_it_ == code_list_.end() ) {
        return false;
    }
    GetLine(line_it_->index_, o_row);
    ++line_it_;
    return true;
}

/**
 * @brief Retrieve one line of the (unfiltered) output
 *
 * @param a_idx zero based index of the row
 * @param o_row json object with the row column
 */
void casper::see::See::GetLine (int a_idx, Json::Value& o_row)
{
    int  row_idx;
    Term const_value;

    o_row = Json::Value(Json::ValueType::objectValue);

    row_idx = a_idx + columns_.begin()->second.row_ + 1;
    for ( ColumnHash::iterator column_it = columns_.begin(); column_it != columns_.end(); ++column_it ) {

        Term* value = (Term*) GetCell(row_idx, column_it->second.col_);
        if ( value != NULL ) {
            Json::Value column;

            if ( value->HasError() && not (value->IsNull() && column_it->second.col_type_->nullable_) ) {
                throw OSAL_EXCEPTION("EXCEL_ERROR @row %d col %s = %s", row_idx, column_it->first.c_str(), value->ErrorMsg());
            }

            SerializeTermToJSONValue(*value, column_it->second.col_type_->term_, column_it->second.col_type_->excel_, column);
            o_row[column_it->first.c_str()] = column;
        }
    }
}

/**
 * @brief Helper to grap an "excel cell" from the data model
 *
 * This is a bit hairy because we have aliases and the values are spread
 * between two diferent hashes (symbol table and line constants)
 *
 * @param a_row The row index starts at one
 * @param a_col The column index starts at one
 *
 * @return Pointer to the Term or NULL if the cell does not exist
 */
const casper::Term* casper::see::See::GetCell (int a_row, int a_col)
{
    StringHash::iterator  ali_it;
    SymbolTable::iterator sym_it;
    char                  cell_reference[20];

    Sum::MakeRowColRef(cell_reference, a_row, a_col);

    /*
     * 1st Look for a symbol with name of the cell ref
     */
    sym_it = symtab_.find(cell_reference);
    if ( sym_it == symtab_.end() ) {
        /*
         * 2nd look for alias (a name that is equivalent to the cellref)
         */
        ali_it = aliases_.find(cell_reference);
        if ( ali_it != aliases_.end() ) {
            sym_it = symtab_.find(ali_it->second);
        }
    }
    if ( sym_it != symtab_.end() ) {
        return &sym_it->second;
    } else {
        /*
         * 3rd shot, the value could be an immutable value loaded from the lines table
         */
        sym_it = line_values_.find(cell_reference);
        if ( sym_it != line_values_.end() ) {
            return &sym_it->second;
        }
    }
    return NULL;
}

/**
 * @brief Serialize a 'term' to a JSON object.
 *
 * @param a_term
 * @param a_type
 * @param a_excel_type
 * @param o_value
 */
void casper::see::See::SerializeTermToJSONValue (const casper::Term& a_term, const int a_type, const std::string& a_excel_type,
                                                 Json::Value& o_value)
{
    switch (a_type) {
        case casper::Term::ENumber:
        case casper::Term::EDate:
        {
            const double number = a_term.ToNumber();
            double double_value;
            if ( 0.0 == number ) { // ... avoid -0 ...
                double_value = 0.0;
            } else {
                double_value = number;
            }
            // ... convert to data model type: INTEGER, DECIMAL, MONEY or DATE
            if ( 0 != a_excel_type.length() ) {
                // ... convert date to human readable ...
                if ( 0 == strncasecmp(a_excel_type.c_str(), "DATETIME", sizeof(char) * 8) ) {
                    o_value = osal::Date::ExcelDateToISO8601CombinedInUTC(double_value);
                } else if ( 0 == strncasecmp(a_excel_type.c_str(), "DATE", sizeof(char) * 4) ) {
                    o_value = osal::Date::ExcelDateToISO8601(double_value);
                } else if ( 0 == strncasecmp(a_excel_type.c_str(), "INTEGER", sizeof(char) * 7) ) {
                    o_value = static_cast<int>(double_value);
                } else { /* if ( 0 == strncasecmp(a_excel_type.c_str(), "DECIMAL", sizeof(char) * 7) ||
                          0 == strncasecmp(a_excel_type.c_str(), "MONEY"  , sizeof(char) * 5) ||
                          any other type ... ) { */
                    o_value = double_value;
                }
            } else {
                // ... no mapping provided ...
                o_value = double_value;
            }
            break;
        }
        case casper::Term::EExcelDate:
            if ( 0.0 == a_term.GetNumber() ) {
                o_value = Json::Value(Json::ValueType::nullValue);
            } else {
                o_value = osal::Date::ExcelDateToISO8601(a_term.GetNumber());
            }
            break;
        case casper::Term::EBoolean:
            if ( a_term.ToBoolean() ) {
                o_value = true;
            } else if ( strcasecmp(a_term.AsString().c_str(), "true") == 0 ) {
                o_value = true;
            } else if ( a_term.GetNumber() != 0.0 ) {
                o_value = true;
            } else {
                o_value = false;
            }
            break;
        case casper::Term::EText:
        {
            const std::string text   = a_term.AsString();
            const char* const c_text = text.c_str();
            if ( nullptr == c_text || (0 == strlen(c_text) && serialize_empty_str_as_null_) ) {
                o_value = Json::Value(Json::ValueType::nullValue);
            } else {
                o_value = text;
            }
            break;
        }
        default:
            o_value = a_term.AsString();
            break;
    }
}

#ifdef __APPLE__
#pragma mark -
#pragma mark ::: DATA SOURCE INTERFACE IMPLEMENTATION :::
#pragma mark -
#endif

/**
 * @brief Sets the datasource active row
 *
 * This is used to access record fields aka 'lines' table cell values. Should be called after the data
 * source is rewound.
 *
 * @param a_row_index index, 0 selects the first row
 */
void casper::see::See::SetDataRowIndex (int a_row_index)
{
    if ( a_row_index < 0 || a_row_index >= (int) code_list_.size() ) {
        throw OSAL_EXCEPTION("Row index '%d' is out of bounds", a_row_index);
    }
    data_source_row_index_ = code_list_[a_row_index].index_;
}

void casper::see::See::Rewind()
{
    RewindScalarIterator();
    RewindPaySlipRowIterator();
    SetDataRowIndex(0);
}

int casper::see::See::DataRowsCount () const
{
    return (int) code_list_.size();
}

const casper::Term* casper::see::See::GetParameter (const char* a_param_name)
{
    SymbolTable::iterator it = symtab_.find(a_param_name);

    if ( it == symtab_.end() ) {
        return NULL;
    } else {
        return &it->second;
    }
}

const casper::Term* casper::see::See::GetField (const char* a_field_name)
{
    ColumnHash::iterator col_it;

    col_it = columns_.find(a_field_name);
    if ( col_it == columns_.end() ) {
        throw OSAL_EXCEPTION("Field not found: '%s'", a_field_name);
    }
    if ( -1 == data_source_row_index_ ) {
        throw OSAL_EXCEPTION_NA("Data source is not ready");
    }

    return GetCell(table_header_row_ + 1 + data_source_row_index_, col_it->second.col_);
}

#ifdef __APPLE__
#pragma mark - ::: VLOOKUP HELPERS :::
#pragma mark -
#endif

#ifdef __APPLE__
#pragma mark ... LINES VLOOKUP
#endif

void casper::see::See::LinesVlookup (Term& a_result, const casper::Term& a_value, const char* const a_lookup_col, const casper::Term& a_result_index, bool a_range_lookup,
                                     const char* const a_formula, const size_t& a_formula_length)
{
    const std::string formula = std::string(a_formula, a_formula_length);
    const std::string key     = formula;

    if ( true == check_dependencies_ ) {

        /*
         * Insert a dependency into the formula being analysed, used the conventioned sum name
         */
        temp_formula_->precedents_.insert(formula);

        /*
         * Prevent duplicates
         */
        if ( symtab_.find(key) != symtab_.end() ) {
            return;
        }

        /*
         * Create a VLOOKUP formula and add to the formula list
         */
        casper::see::Vlookup* vlookup = new casper::see::Vlookup(*this, a_lookup_col, static_cast<int>(a_result_index.ToNumber()));
        vlookup->name_     = key;
        vlookup->formula_  = formula;
        formulas_.push_back(vlookup);
    } else {
        /*
         * Use the cached value if available, if not calculate with the current criterias.
         */
        if ( symtab_.find(key) != symtab_.end() ) {
            a_result = symtab_[key];
        } else {
            a_result = current_formula_->VLOOKUP(a_value, a_lookup_col, a_result_index, a_range_lookup);
            symtab_[key] = a_result;
        }
    }
}

#ifdef __APPLE__
#pragma mark -
#endif

void casper::see::See::LoadTypes (const Json::Value& a_object, std::map<std::string, TypeMapEntry>& o_types)
{
    const auto excel_type_to_term_type = [] (const char* const a_type) -> int {
        if ( 0 == strncasecmp(a_type, "TEXT", sizeof(char) * 4) ) {
            return casper::Term::EText;
        } else if ( 0 == strncasecmp(a_type,"DATE", sizeof(char) * 4) ) {
            return casper::Term::EExcelDate;
        } else if ( 0 == strncasecmp(a_type,"BOOLEAN", sizeof(char) * 7) ) {
            return casper::Term::EBoolean;
        } else if ( 0 == strncasecmp(a_type,"INTEGER", sizeof(char) * 7) || 0 == strncasecmp(a_type,"DECIMAL", sizeof(char) * 7) ||
                        0 == strncasecmp(a_type, "MONEY", sizeof(char) * 5) ) {
            return casper::Term::ENumber;
        } else {
            throw OSAL_EXCEPTION("Dont know how to convert type '%s' to term type!!", a_type);
        }
    };
    for ( auto cell_name_or_ref : a_object.getMemberNames() ) {
        const char* const  cell_name_or_ref_c_str = cell_name_or_ref.c_str();
        const Json::Value& object                 = a_object[cell_name_or_ref];
        if ( true == object.isNull() ) {
            throw OSAL_EXCEPTION("[%s]: Can't grab JSON object named '%s'!", __FUNCTION__, cell_name_or_ref_c_str);
        }
        const Json::Value& type  = object["type"];
        if ( true == type.isNull() ) {
            throw OSAL_EXCEPTION("[%s]: Unable to find 'type' attribute for '%s'!", __FUNCTION__, cell_name_or_ref_c_str);
        }
        const bool is_nullable = nullptr != strcasestr(type.asCString(), "_NULLABLE");
        o_types[cell_name_or_ref_c_str] = { type.asCString(), excel_type_to_term_type(type.asCString()), is_nullable };
    }
}

void casper::see::See::LoadValues (const Json::Value& a_object, Json::Value& o_values, std::map<std::string, TypeMapEntry>* o_types)
{
    o_values = Json::Value(Json::ValueType::objectValue);
    // ... load types?
    if ( nullptr != o_types ) {
        LoadTypes(a_object, *o_types);
    }
    // ... now load values ...
    for ( auto cell_name_or_ref : a_object.getMemberNames() ) {
        const char* const  cell_name_or_ref_c_str = cell_name_or_ref.c_str();
        const Json::Value& value = a_object[cell_name_or_ref]["value"];
        if ( true == value.isNull() ) {
            throw OSAL_EXCEPTION("[%s]: Unable to find 'type' attribute for '%s'!", __FUNCTION__, cell_name_or_ref_c_str);
        }
        o_values[cell_name_or_ref] = value;
    }
}

void casper::see::See::LoadHeaders (const Json::Value& a_object, Json::Value& o_values, std::map<std::string, TypeMapEntry>* o_types)
{
    o_values = Json::Value(Json::ValueType::objectValue);
    // ... load types?
    if ( nullptr != o_types ) {
        LoadTypes(a_object, *o_types);
    }
    // ... now load values ...
    for ( auto cell_name_or_ref : a_object.getMemberNames() ) {
        const char* const  cell_name_or_ref_c_str = cell_name_or_ref.c_str();
        const Json::Value& name = a_object[cell_name_or_ref]["name"];
        if ( true == name.isNull() ) {
            throw OSAL_EXCEPTION("[%s]: Unable to find 'name' attribute for '%s'!", __FUNCTION__, cell_name_or_ref_c_str);
        }
        o_values[cell_name_or_ref] = name;
    }
}


void casper::see::See::SetDefaultTermValue (int a_model_type, casper::Term& o_value)
{
    switch (a_model_type) {
        case casper::Term::ENumber:
            o_value = 0.0f;
            break;
        case casper::Term::EBoolean:
            o_value = false;
            break;
        case casper::Term::EText:
            o_value = "";
            break;
        case casper::Term::EExcelDate:
        {
            o_value.type_   = casper::Term::EExcelDate;
            o_value.number_ = 0;
            break;
        }
        default:
            // ... excel style ...
            o_value = 0.0f;
            break;
    }
}

void casper::see::See::ConvertTermType (int a_model_type, casper::Term& o_value)
{
    switch (a_model_type) {
        case casper::Term::ENumber:
            o_value.ConvertToNumber();
            break;
        case casper::Term::EBoolean:
            if ( casper::Term::EText == o_value.GetType() ) {
                o_value = ( 0 == strcasecmp("true", o_value.GetText()) || 0 == strcasecmp("t", o_value.GetText()) ) ? true : false ;
            } else {
                o_value = o_value.ConvertToBoolean();
            }
            break;
        case casper::Term::EText:
        case casper::Term::EUndefined:
        default:
            // ... leave it untouched ...
            break;
    }
}
