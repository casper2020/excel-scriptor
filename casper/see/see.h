/**
 * @file See.h declaration of See model node
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
#ifndef NRS_CASPER_CASPER_SEE_SEE_H
#define NRS_CASPER_CASPER_SEE_SEE_H

#include "casper/see/parser.hh"
#include "casper/see/see_scanner.h"
#include "casper/see/formula.h"
#include "casper/term.h"
#include "casper/abstract_data_source.h"
#include "json/json.h"
#include "osal/exception.h"
#include <map>
#include <vector>
#include <set>
#include <deque>
#include <tuple>

namespace casper
{
    namespace see
    {
        class Formula;
        class Table;

        typedef std::vector<std::string>              StringList;

        typedef struct _TypeMapEntry {
            std::string excel_;
            int         term_;
            bool        nullable_;
        } TypeMapEntry;

        struct ColumnInfo
        {
            std::string name_;
            int           col_;
            int           row_;
            TypeMapEntry* col_type_;

            ColumnInfo ()
            {
                col_type_ = nullptr;
            }
        };

        typedef std::vector<Formula*>                 FormulaList;
        typedef std::map<std::string, Table*>         TableHash;
        typedef std::map<std::string, ColumnInfo>     ColumnHash;
        typedef std::map<int, std::string>            ColumnNameIndex;
        typedef std::map<std::string, Term*>          TermPtrHash;

        struct SlaveCloneInfo
        {
            int index_;    //!< Source index in the original data model
            int subcode_;  //!< Numeric prefix of the subclone

            SlaveCloneInfo (int a_index, int a_subcode)
            {
                index_   = a_index;
                subcode_ = a_subcode;
            }
        };

        struct CloneInfo
        {
            int                         master_index_;  //!< Index of the master cloned row
            int                         clone_suffix_;  //!< The numeric suffix of the clone row
            std::string                 cloned_code_;   //!< The name of the code that is cloned (source)
            std::string                 clone_name_;    //!< The name of the new clone
            std::vector<SlaveCloneInfo> slave_clones_;  //!< List of subcloned lines
        };

        typedef std::map<std::string, CloneInfo> CloneMap;

        /**
         * @brief Helper structure to order filtered output lines
         */
        struct CodeInfo
        {
            std::string code_;
            int         master_index_;
            int         index_;
            bool        is_note_;

            CodeInfo (const char* a_code, int a_master_index, int a_index, bool a_is_note)
            {
                code_         = a_code;
                master_index_ = a_master_index;
                index_        = a_index;
                is_note_      = a_is_note;
            }
        };

        typedef std::vector<CodeInfo> CodeList;

        /**
         * @brief Simple expression engine that evaluates excel models
         */
        class See : public AbstractDataSource
        {
            friend class Parser;
            friend class Sum;
            friend class SumIf;
            friend class SumIfs;
            friend class Vlookup;

        public: //temp

            int                   row_count_;                   //!< Number of rows in table
            int                   table_header_row_;            //!< Excel row number of lines table header row, base is 1!!

        protected: // Data

            bool                  check_dependencies_;          //!< true during the load phase, false during calculations
            SymbolTable           symtab_;                      //!< Symbol table a dictionary of Term nodes
            Scanner               scanner_;                     //!< Term tokenizer/Scanner
            Parser                parser_;                      //!< Term gramar parser
            FormulaList           formulas_;                    //!< Array with pointers to all formulas
            SymbolTable           reference_symtab_;            //!< Holds the constant terms created by the loading process
            SymbolTable           line_values_;                 //!< Keeps literals of the lines table
            StringSet             precedents_;                  //!< List of independent terms used by the formulas
            StringHash            aliases_;                     //!< Maps the cells to name mappings
            ColumnNameIndex       column_name_index_;           //!< Holds the names of the columns indexed by col number
            Formula*              temp_formula_;                //!< Temp holder for formula being loaded in dependency analysis
            Formula*              current_formula_;             //!< Formula being calculated in calculation phase
            StringHash            name_to_cell_aliases_;        //!< Maps the formulas names to cells refs
            TableHash             tables_;                      //!< Holds the engine lookup tables
            ColumnHash            columns_;                     //!< Holds the definition of lines table columns
            SymbolTable           sum_criterias_;               //!< List of criterias for SUMIFS (we only handle one at a time)
            std::string           expression_name_;             //!< The name of the current left hand side variable being evaluated
            std::string           json_data_path_;              //!< Path to the folder with static JSON data
            std::string           json_tables_path_;            //!< Path to the folder with static JSON data
            SymbolTable::iterator scalar_it_;                   //!< Iterator to retrieve scalar results
            CodeList              code_list_;                   //!< List of output codes
            CodeList::iterator    line_it_;                     //!< Iterator to retrieve output lines
            const char*           code_col_name_;               //!< Name of the code column in the model (default "COD")
            const char*           condition_col_name_;          //!< Name of the condition column (default "CONDICAO")
            bool                  has_template_lines_;          //!< The lines table contains template lines
            bool                  serialize_empty_str_as_null_; //!< Export empty strings as null
            Json::Value           untouched_model_;             //!<

            size_t                lines_templates_start_idx_;   //!<
            size_t                lines_templates_end_idx_;     //!<
            size_t                lines_templates_count_;       //!<
            size_t                lines_clones_start_idx_;      //!<
            size_t                lines_clones_end_idx_;        //!<
            size_t                lines_clones_count_;          //!<
            size_t                lines_clones_offset_;         //!<

            std::string           log_file_name_;               //!< File to log step by step calculations
            FILE*                 log_file_;                    //!< Handle for log file

            std::map<std::string, TypeMapEntry> scalars_types_map_;
            std::map<std::string, TypeMapEntry> lines_columns_types_map_;

            bool                               track_lookups_;
            Json::Value                        track_filter_params_;

            struct lt_tables_comparator {
                bool operator() (const std::string& a_lhs, const std::string& a_rhs) const {
                    return strcasecmp(a_lhs.c_str(), a_rhs.c_str()) < 0;
                }
            };

            std::map<std::string, Json::Value, lt_tables_comparator> lt_tables_;

        public:

            std::set<std::string> untouchable_tables_;        //!<

        public:

            Term                  result_;               //!< Final result of expression calculation

        protected: // Methods

            void        AddDependency            (Term& a_varname);
            void        AddAlias                 (Term& a_varname);
            void        Clear                    ();
            void        LoadModel                (Json::Value& a_model, StringMultiHash& a_clone_map,
                                                  const std::function<void(Json::Value& a_scalars)> a_patch_scalars = nullptr,
                                                  const std::function<void(Json::Value& a_lines, size_t& o_number_of_added_lines)> a_clone_lines = nullptr);
            void        LoadFormula              (const char* a_expression, const char* a_alias);
            void        LoadModelFromFile        (const char* a_filename, StringMultiHash& a_clone_map,
                                                  const std::function<void(Json::Value& a_scalars)> a_patch_scalars = nullptr,
                                                  const std::function<void(Json::Value& a_lines, size_t& o_number_of_added_lines)> a_clone_lines = nullptr);
            void        CalculateSumDependencies ();
            void        SortDependencies         ();
            bool        CloneLinesTableLines     (StringMultiHash& a_clone_map, Json::Value& a_lines_formulas, Json::Value& a_lines_value);
            const Term* GetCell                  (int a_row, int a_col);

            Table* GetTableByName                (const char* a_table_name);
            virtual Table* LoadTable             (const char* a_table_name, Table* a_partially_loaded_table);

            /*
             * Functions overridden in specializtion classes
             */
            using AbstractDataSource::GetVariable;
            virtual void GetVariable        (Term& a_result,  Term& a_varname, casper::see::location& a_location);
            using AbstractDataSource::SetVariable;
            virtual void SetVariable        (Term& a_varname, Term& a_value,   casper::see::location& a_location);
            virtual void GetLinesTableValue (Term& a_result,  Term& a_column_name);
            virtual void TableHeaderMatch   (Term& a_result,  Term& a_colname, Term& a_tablename);
            virtual void Offset             (Term& o_result,  const Term& a_ref, const Term& a_rows, const Term& a_cols);
            virtual void Lookup             (Term& a_result,  Term& a_value, Term& a_lookup_vector, Term& a_result_vector);
            virtual void Vlookup            (Term& a_result,  Term& a_value, Term& a_lookup_vector, Term& a_col_index, bool a_range_lookup,
                                             const char* const a_formula, const size_t& a_formula_length);
            virtual void SumIf              (Term& a_result, const Term& a_range, const Term& a_criteria,
                                             const char* const a_formula, const size_t& a_formula_length);
            virtual void SumIf              (Term& a_result, const Term& a_range, const Term& a_criteria, const Term& a_sum_range,
                                             const char* const a_formula, const size_t& a_formula_length);
            virtual void SumIfs             (Term& a_result,  Term& a_sum_range);
            virtual void Sum                (Term& a_result,  Term& a_vector_ref);
            virtual void Sum                (Term& a_result,  Term& a_cell_start, Term& a_cell_end);
            virtual void SumIfOnLinesTable  (Term& a_result, const char* a_sum_column, const Term& a_criteria,
                                             const char* const a_formula, const size_t& a_formula_length);
            virtual void SumIfsOnLinesTable (Term& a_result, const char* a_sum_column, SymbolTable& a_criterias);

            /*
             * Functions implemented in separated ragel files
             */
            bool MatchLineCode (const char* a_code_expression, const char* a_code_to_clone, int& o_sub_code);

            static bool CodeComparator (const casper::see::CodeInfo& a_lhs, const casper::see::CodeInfo& a_rhs);


#ifdef __APPLE__
#pragma mark -
#endif

        public: // Methods

            See ();
            virtual ~See ();

            /*
             * Loading and configuration API
             */
            void SetJsonDataPath         (const char* a_json_data_path);
            void SetHashasTemplateLines  (bool a_has_template);
            void SetLogFile              (const char* const a_file);
            void EnableLogging           ();
            void DisableLogging          ();
            virtual void LoadModel       (StringMultiHash& a_clone_map,
                                          const std::function<void(Json::Value& a_scalars)> a_patch_scalars = nullptr,
                                          const std::function<void(Json::Value& a_lines, size_t& o_number_of_added_lines)> a_clone_lines = nullptr);
            void LoadTable       (Table* a_table);
            void UnloadTable     (const std::string& a_table);
            void CalculateAll    (const Json::Value& a_params);
            void CalculateAll    ();

            /*
             * Access to relevant information
             */
            FormulaList      getFormulas();
            SymbolTable      getRefSymtab();
            SymbolTable      getLineValues();
            SymbolTable      getSymtab();
            StringSet        getPrecedents();
            StringHash       getAliases();
            ColumnNameIndex  getColumnNameIndex();

            /*
             * Calculation API
             */
            void Calculate      (const char* a_expression, size_t a_len);

            /*
             * Access to calculation results
             */
            int   RewindScalarIterator         ();
            bool  GetNextScalar                (Json::Value& o_scalar);
            void  GetLine                      (int a_idx, Json::Value& o_row);
            void  SerializeScalarsToJSONObject (Json::Value& a_object);
            Term& GetExpressionResult          ();
            int   RewindPaySlipRowIterator     ();
            bool  GetNextLine                  (Json::Value& o_row);
            void  SerializeTermToJSONValue     (const Term& a_term, const int a_type, const std::string& a_excel_type,
                                                Json::Value& o_value);
            void  SetSerializeEmptyStrAsNull   (bool a_bool);

            const TableHash& Tables () const;
            void  SetTrackLookups   (const Json::Value& a_lt_tables, const Json::Value& a_params);
            bool  TrackingLookups   () const;

            /*
             * Data source interface implementation
             */
            virtual void        Rewind          ();
            virtual int         DataRowsCount   () const;
            virtual void        SetDataRowIndex (int a_row_index);
            virtual const Term* GetParameter    (const char* a_param_name);
            virtual const Term* GetField        (const char* a_field_name);

            /*
             * Debug helpers
             */
            const char* RowShiftTesthook (const char* a_expression, int a_row_shift);

            /*
             * Type helpers
             */
            virtual int                 GetParamType                   (const char* const a_key, const casper::Term* a_term,
                                                                        bool& o_is_nullable,
                                                                        std::string& o_excel_type,
                                                                        const int a_default = casper::Term::EText);
            virtual int                 GetLineColumnType              (const char* const a_ref, const char* const a_name, const casper::Term* a_term,
                                                                        bool& o_is_nullable,
                                                                        std::string& o_excel_type);

        protected: // Virtual Method(s) / Function(s)

            virtual void LogDebugMessage (const char* const /* a_format */, ...) __attribute__ ((format (printf, 2, 3))) {}

        protected: //

            void LinesVlookup (Term& a_result, const casper::Term& a_value, const char* const a_lookup_col, const casper::Term& a_result_index, bool a_range_lookup,
                               const char* const a_formula, const size_t& a_formula_length);

            void LoadTypes           (const Json::Value& a_object, std::map<std::string, TypeMapEntry>& o_types);
            void LoadValues          (const Json::Value& a_object, Json::Value& o_values, std::map<std::string, TypeMapEntry>* o_types);
            void LoadHeaders         (const Json::Value& a_object, Json::Value& o_headers, std::map<std::string, TypeMapEntry>* o_types);
            void SetDefaultTermValue (int a_model_type, casper::Term& o_value);
            void ConvertTermType     (int a_model_type, casper::Term& o_value);

        };


        inline FormulaList See::getFormulas()
        {
            return formulas_;
        }
        inline SymbolTable See::getRefSymtab()
        {
            return reference_symtab_;
        }
        inline SymbolTable See::getLineValues()
        {
            return line_values_;
        }
        inline SymbolTable See::getSymtab()
        {
            return symtab_;
        }
        inline StringSet See::getPrecedents()
        {
            return precedents_;
        }
        inline StringHash See::getAliases()
        {
            return aliases_;
        }
        inline ColumnNameIndex See::getColumnNameIndex()
        {
            return column_name_index_;
        }

        inline void See::SetJsonDataPath (const char* a_json_data_path)
        {
            if (a_json_data_path[strlen(a_json_data_path) -1] == '/' ) {
                json_data_path_ = a_json_data_path;
            } else {
                json_data_path_  = a_json_data_path;
                json_data_path_ +=  "/";
            }
        }

        inline void See::SetLogFile (const char* const a_file)
        {
            log_file_name_ = nullptr != a_file ? a_file : "";
        }

        inline Term& See::GetExpressionResult ()
        {
            return result_;
        }

        inline void See::SetHashasTemplateLines (bool a_has_template)
        {
            has_template_lines_ = a_has_template;
        }

        inline void See::SetSerializeEmptyStrAsNull (bool a_bool)
        {
            serialize_empty_str_as_null_ = a_bool;
        }

        inline const TableHash& See::Tables () const
        {
            return tables_;
        }

        inline void See::SetTrackLookups (const Json::Value& a_lt_tables, const Json::Value& a_params)
        {
            track_lookups_ = true;
            lt_tables_.clear();
            for ( Json::ArrayIndex idx = 0 ; idx < static_cast<Json::ArrayIndex>(a_lt_tables.size()) ; ++idx ) {
                const Json::Value& object = a_lt_tables[idx];
                lt_tables_[object["name"].asString()] = object["filter"];
            }
            track_filter_params_ = a_params;
        }

        inline bool See::TrackingLookups () const
        {
            return track_lookups_;
        }

    } // namespace see
} // namespace casper

#endif // NRS_CASPER_CASPER_SEE_SEE_H
