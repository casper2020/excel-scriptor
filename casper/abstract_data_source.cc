/**
 * @file abstract_data_source.cc
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

#include "casper/abstract_data_source.h"

#ifdef __APPLE__
#pragma mark - PUBLIC - Constructor(s) / Destructor
#endif

/**
 * @brief Default constructor
 */
casper::AbstractDataSource::AbstractDataSource ()
{
    data_source_row_index_ = 0;
    data_source_row_count_ = 0;
}

/**
 * @brief Destructor
 */
casper::AbstractDataSource::~AbstractDataSource ()
{
    /* empty */
}

#ifdef __APPLE__
#pragma mark - [INSTANCE] - Public -  Optional Method(s) / Function(s)
#endif


#ifdef __APPLE__
#pragma mark - Symbol Tables
#endif

/**
 * @return The 'parameters' symbol table.
 */
const casper::SymbolTable* casper::AbstractDataSource::GetParameters () const
{
    throw OSAL_EXCEPTION_NA("Not implemented!");
}

/**
 * @brief Retrieve a data row for a specific index.
 *
 * @param a_index Data row index.
 *
 * @return The data row for the provided index.
 */
const casper::SymbolTable* casper::AbstractDataSource::GetFields (const size_t& a_index) const
{
    OSAL_UNUSED_PARAM(a_index);
    throw OSAL_EXCEPTION_NA("Not implemented!");
}

#ifdef __APPLE__
#pragma mark - Parameters
#endif

/**
 * @brief Set a parameter value.
 *
 * @param a_name  Parameter name.
 * @param a_value Parameter value.
 */
void casper::AbstractDataSource::SetParameter (const char* a_name, const Term& a_value)
{
    OSAL_UNUSED_PARAM(a_name);
    OSAL_UNUSED_PARAM(a_value);
    throw OSAL_EXCEPTION_NA("SetParameter(...) - Not implemented!");
}

#ifdef __APPLE__
#pragma mark - Fields
#endif

/**
 * @brief Sets a field value for the currently selected data row.
 *
 * @param a_name  Field name.
 * @param a_value Field value.
 */
void casper::AbstractDataSource::SetField (const char* a_name, const Term& a_value)
{
    OSAL_UNUSED_PARAM(a_name);
    OSAL_UNUSED_PARAM(a_value);
    throw OSAL_EXCEPTION_NA("SetField(...) - Not implemented!");
}

#ifdef __APPLE__ 
#pragma mark - Variables
#endif

/**
 * @brief Sets a variable value.
 *
 * @param a_name  Variable name.
 * @param a_value Variable value.
 */
void casper::AbstractDataSource::SetVariable (const char* a_name, const Term& a_value)
{
    OSAL_UNUSED_PARAM(a_name);
    OSAL_UNUSED_PARAM(a_value);
    throw OSAL_EXCEPTION_NA("SetVariable(...) - Not implemented!");
}

/**
 * @brief Retrieve a variable value.
 *
 * @param a_name  Variable name.
 *
 * @return Variable value.
 */
const casper::Term* casper::AbstractDataSource::GetVariable (const char* a_name)
{
    OSAL_UNUSED_PARAM(a_name);
    throw OSAL_EXCEPTION_NA("GetVariable(...) - Not implemented!");
}

/**
 * @brief Check if a variable exists.
 *
 * @param a_name  Variable name.
 *
 * @return True if the variable exists, false otherwise.
 */
bool casper::AbstractDataSource::VariableExists (const char* a_name)
{
    OSAL_UNUSED_PARAM(a_name);
    throw OSAL_EXCEPTION_NA("VariableExists(...) - Not implemented!");
}

#ifdef __APPLE__
#pragma mark - Data Row
#endif

/**
 * @return The number of available data rows.
 */
int casper::AbstractDataSource::DataRowsCount () const
{
    return data_source_row_count_;
}

/**
 * @return The current data row index.
 */
int casper::AbstractDataSource::DataRowIndex () const
{
    return data_source_row_index_;
}

/**
 * @brief Set data row index.
 *
 * @param a_index New data row index value.
 */
void casper::AbstractDataSource::SetDataRowIndex (int a_row_index)
{
    if ( 0 == a_row_index && 0 == data_source_row_count_ ) {
        data_source_row_index_ = 0;
        return;
    }
    if ( a_row_index < 0 || a_row_index >= data_source_row_count_ ) {
        throw OSAL_EXCEPTION_NA("Data source row index out of bounds!");
    }
    data_source_row_index_ = a_row_index;
}

/**
 * @brief Append a new data row.
 *
 * @param a_row The row data, see \link casper::SymbolTable \link.
 */
void casper::AbstractDataSource::AppendDataRow (const casper::SymbolTable& a_row)
{
    OSAL_UNUSED_PARAM(a_row);
    throw OSAL_EXCEPTION_NA("Not implemented!");
}

/**
 * @brief Remove a data row at a specific index.
 *
 * @param a_index Position where the data row should be removed from
 */
void casper::AbstractDataSource::RemoveDataRow (const size_t& a_index)
{
    OSAL_UNUSED_PARAM(a_index);
    throw OSAL_EXCEPTION_NA("Not implemented!");
}
