/**
 * @file line_code_parser.rl Helper to match a code to be cloned in the excel exported model
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

/**
 * @brief Helper to match a code to be cloned in the excel exported model.
 *
 * @param a_code_expression the line code tho check
 * @param a_code_to_clone the master code being cloned e.g. A008 that could have A008_M1, A008_M2 subcodes
 * @param o_sub_code if the three is match the numeric subcode will be copied
 *
 * @param @li true the code is a subcode of the cloned code that must cloned along with its master
 *        @li false there is not match
 */
bool casper::see::See::MatchLineCode (const char* a_code_expression, const char* a_code_to_clone, int& o_sub_code)
{
    int      cs;
    char*    p          = (char*) a_code_expression;
    char*    pe         = p + strlen(a_code_expression);
    char*    code_start = NULL;
    size_t   code_len   = 0;
    int      subcode    = 0;
    
    %%{
        machine line_code_expression;
        
        varname = [A-Za-z0-9_]+;
        code    = (zlen >{ code_start = fpc; } [A-Za-z]+[0-9]+ @{ code_len = (size_t)(fpc - code_start) + 1; } );
        subcode = ( ('_M' | '_N') [0-9]+ ${  subcode *= 10; subcode += (fc - '0'); });
        
        main := varname '=' '"' code subcode? '"';
        
        write data;
        write init;
        write exec;
    }%%

    o_sub_code = subcode;

    if ( cs >= line_code_expression_first_final ) {
        if ( code_start != NULL && code_len == strlen(a_code_to_clone) ) {
            if ( strncmp(a_code_to_clone, code_start, code_len) == 0 ) {
                return true;
            }
        }
    }
    return false;
    OSAL_UNUSED_PARAM(line_code_expression_error);
    OSAL_UNUSED_PARAM(line_code_expression_en_main);
}

