
#line 1 "/Users/bruno/work/excelscriptor/casper/see/line_code_parser.rl"
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
    
    
#line 46 "/Users/bruno/work/excelscriptor/casper/see/line_code_parser.cc"
static const int line_code_expression_start = 1;
static const int line_code_expression_first_final = 10;
static const int line_code_expression_error = 0;

static const int line_code_expression_en_main = 1;


#line 54 "/Users/bruno/work/excelscriptor/casper/see/line_code_parser.cc"
	{
	cs = line_code_expression_start;
	}

#line 59 "/Users/bruno/work/excelscriptor/casper/see/line_code_parser.cc"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 95 )
		goto st2;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st2;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st2;
	} else
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	switch( (*p) ) {
		case 61: goto st3;
		case 95: goto st2;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st2;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st2;
	} else
		goto st2;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) == 34 )
		goto st4;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr4;
	} else if ( (*p) >= 65 )
		goto tr4;
	goto st0;
tr4:
#line 46 "/Users/bruno/work/excelscriptor/casper/see/line_code_parser.rl"
	{ code_start = p; }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 122 "/Users/bruno/work/excelscriptor/casper/see/line_code_parser.cc"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr5;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st5;
	} else
		goto st5;
	goto st0;
tr5:
#line 46 "/Users/bruno/work/excelscriptor/casper/see/line_code_parser.rl"
	{ code_len = (size_t)(p - code_start) + 1; }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 140 "/Users/bruno/work/excelscriptor/casper/see/line_code_parser.cc"
	switch( (*p) ) {
		case 34: goto st10;
		case 95: goto st7;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr5;
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( 77 <= (*p) && (*p) <= 78 )
		goto st8;
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr10;
	goto st0;
tr10:
#line 47 "/Users/bruno/work/excelscriptor/casper/see/line_code_parser.rl"
	{  subcode *= 10; subcode += ((*p) - '0'); }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 175 "/Users/bruno/work/excelscriptor/casper/see/line_code_parser.cc"
	if ( (*p) == 34 )
		goto st10;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr10;
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 54 "/Users/bruno/work/excelscriptor/casper/see/line_code_parser.rl"


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

