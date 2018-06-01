#pragma once
/**
 * @file Table.h declaration of in-memory data Table
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
#ifndef NRS_CASPER_CASPER_SEE_TABLE_H
#define NRS_CASPER_CASPER_SEE_TABLE_H

#include "see.h"
#include "json/json.h"
#include <string>
#include <map>
#include <vector>

namespace casper
{
    class Term;
    
    namespace see
    {
        
        /**
         * @brief Simple in memory data table
         */
        class Table
        {
        public: // Data type
            
            struct Column
            {
                std::string       name_;
                std::vector<Term> values_;
            };
            
            class TrackedLookups
            {
                
            public:
                
                typedef std::map<int, Json::Value*> AccessMap;
                typedef std::vector<int>            AccessOrder;
                
            private: // Data
                
                AccessMap   access_map_;
                AccessOrder access_order_;
                
            public: // Constructor(s) / Destructor
                
                virtual ~TrackedLookups ()
                {
                    Clear();
                }
                
            public: // Method(s) / Function(s)
                
                inline void Clear ()
                {
                    for ( auto m_it : access_map_ ) {
                        delete m_it.second;
                    }
                    access_map_.clear();
                    access_order_.clear();
                }
                
                inline Json::Value* JSONObject (int a_row_index)
                {
                    // ... row already tracked?
                    for ( size_t idx = 0 ; idx < access_order_.size() ; ++idx ) {
                        if ( access_order_[idx] == a_row_index ) {
                            // ... yes ...
                            return nullptr;
                        }
                    }
                    
                    // ... no... track it now ...
                    
                    Json::Value* object = new Json::Value(Json::ValueType::arrayValue);
                    
                    access_map_[a_row_index] = object;
                    
                    access_order_.push_back(a_row_index);
                    
                    return object;
                }
                
                inline const AccessOrder& Order () const
                {
                    return access_order_;
                }
                
                inline const AccessMap& Map() const
                {
                    return access_map_;
                }
                
            };

        protected: // Attributes
            
            std::string                   name_;
            std::vector<Column>           columns_;
            std::map<std::string, int>    colname_to_index_;
            bool                          partially_loaded_;
            bool                          use_exact_match_;
            bool                          track_lookups_;
            TrackedLookups                tracked_lookups_;
            
        public: // Methods

            Table (const char* a_name, bool a_is_partial);
            virtual ~Table ();
            
            /*
             * Table managment
             */
            const char* GetName            ();
            void        Load               (Json::Value& a_value, const char* a_name);
            Column&     AddColumn          (const char* a_name);
            int         GetColumnIndex     (const char* a_colname) const;
            void        Clear              ();
            void        SetPartiallyLoaded (bool a_loaded);
            bool        IsPartialyLoaded   () const;
            int         GetRowCount        () const;
            
            Column*     EnsureColumn     (const char* a_name);
            void        SetColumnValue   (const char* const a_name, const Term& a_value);
            
            bool        IsTrackingLookups () const;
            void        SetTrackLookups   (const Json::Value& a_filter, const Json::Value& a_params);
            const       TrackedLookups&   GetTrackedLookups () const;
            const std::vector<Table::Column>& GetColumns () const;
            
            /*
             * Lookup operations
             */
            void        Lookup    (Term& a_result, Term& a_value, const char* a_search_col, const char* a_result_col);
            void        Vlookup   (Term& a_result, Term& a_value, const char* a_lookup_col, Term& a_col_index, bool a_range_lookup);
            void        SumIfs    (Term& a_result, const char* a_sum_column, SymbolTable& a_criteria);
            double      SumColumn (const char* a_sum_column);
            
        protected:
            
            void       TrackLookup     (int a_row);
            void       PrepareTracking (const Json::Value& a_filter, const Json::Value& a_params);
            
        public: //
            
            static bool        Equal      (const casper::Term& a_first, const casper::Term& a_second);
            static bool        Lower      (const casper::Term& a_first, const casper::Term& a_second);
            static std::string BuildQuery (const std::string& a_query, const Json::Value& a_params);
            
        };

        inline const char* Table::GetName ()
        {
            return name_.c_str();
        }
        
        inline int Table::GetColumnIndex (const char* a_colname) const
        {
            std::map<std::string, int>::const_iterator it;
            
            it = colname_to_index_.find(a_colname);
            if ( it != colname_to_index_.end() ) {
                return it->second;
            } else {
                return -1;
            }
        }
        
        inline const std::vector<Table::Column>& Table::GetColumns () const
        {
            return columns_;
        }

        inline void Table::SetPartiallyLoaded (bool a_loaded)
        {
            partially_loaded_ = a_loaded;
        }

        inline bool Table::IsPartialyLoaded () const
        {
            return partially_loaded_;
        }
        
        inline int Table::GetRowCount () const
        {
            if ( columns_.size() == 0 ) {
                return 0;
            } else {
                return (int) (columns_[0].values_.size());
            }
        }
        
        inline Table::Column* Table::EnsureColumn (const char* a_name)
        {
            if ( partially_loaded_ == false ) {
                return &AddColumn(a_name);
            } else {
                std::map<std::string, int>::iterator column_it = colname_to_index_.find(a_name);
                if ( column_it == colname_to_index_.end()) {
                    throw OSAL_EXCEPTION("Appending column %s that does not exist", a_name);
                } else {
                    return &columns_[column_it->second];
                }
            }
        }
        
        inline void Table::SetColumnValue (const char* const a_name, const Term& a_value)
        {
            EnsureColumn(a_name)->values_.push_back(a_value);
        }
        
        inline bool Table::IsTrackingLookups () const
        {
            return track_lookups_;
        }
        
        inline void Table::SetTrackLookups (const Json::Value& a_filter, const Json::Value& a_params)
        {
            track_lookups_ = true;
            tracked_lookups_.Clear();
            PrepareTracking(a_filter, a_params);
        }
        
        inline const Table::TrackedLookups& Table::GetTrackedLookups () const
        {
            return tracked_lookups_;
        }
    
    } // namespace see
} // namespace casper

#endif // NRS_CASPER_CASPER_SEE_TABLE_H
