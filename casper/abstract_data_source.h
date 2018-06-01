/**
 * @file "Interface" implemented by the report data sources
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
#ifndef NRS_CASPER_CASPER_ABSTRACT_DATA_SOURCE_H
#define NRS_CASPER_CASPER_ABSTRACT_DATA_SOURCE_H

#include "casper/term.h"

namespace casper
{
    /**
     * @brief "Interface" implemented by the report data sources
     */
    class AbstractDataSource
    {

    protected: // Data

        int data_source_row_index_;
        int data_source_row_count_;

    public: // Constructor(s) / Destructor

                 AbstractDataSource ();
        virtual ~AbstractDataSource ();

    public: // Pure Virtual Method(s) / Function(s)
        
        virtual const Term* GetParameter   (const char* a_param_name) = 0;
        virtual const Term* GetField       (const char* a_field_name) = 0;
        virtual void        Rewind         ()                         = 0;

    public: // Optional - Virtual Method(s) / Function(s)
        
        virtual const SymbolTable* GetParameters  ()                      const;
        virtual const SymbolTable* GetFields      (const size_t& a_index) const;
        
        virtual void               SetParameter   (const char* a_name, const Term& a_value);
        virtual void               SetField       (const char* a_name, const Term& a_value);
        
        virtual void               SetVariable    (const char* a_name, const Term& a_value);
        virtual const Term*        GetVariable    (const char* a_name);
        virtual bool               VariableExists (const char* a_name);
        
        virtual int                DataRowsCount   () const;
        virtual int                DataRowIndex    () const;
        virtual void               SetDataRowIndex (int a_row_index);
        virtual void               AppendDataRow   (const casper::SymbolTable& a_row);
        virtual void               RemoveDataRow   (const size_t& a_index);

    };

} // namespace casper

#endif // NRS_CASPER_CASPER_ABSTRACT_DATA_SOURCE_H
