
#line 1 "/Users/bruno/work/excelscriptor/casper/number_parser.rl"
/**
 * @file number_parser.rl - Generic number parser using RAGEL.
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
#include "casper/number_parser.h"

#include "osal/osalite.h"
#include "osal/utils/pow10.h"


#line 30 "/Users/bruno/work/excelscriptor/casper/number_parser.cc"
static const int number_parser_machine_start = 5;
static const int number_parser_machine_first_final = 5;
static const int number_parser_machine_error = 0;

static const int number_parser_machine_en_main = 5;


#line 40 "/Users/bruno/work/excelscriptor/casper/number_parser.rl"


const uint8_t casper::NumberParser::k_max_fractional_digits_ = 18;

/**
 * Default constructor.
 */
casper::NumberParser::NumberParser ()
// : p_(NULL), pe_(NULL), eof_(NULL), cs_(0), te_(NULL), ts_(NULL), act_(0)
{
    number_                = NAN;
    integer_               = 0;
    fractional_            = 0;
    fractional_digits_cnt_ = 0;
    divider_               = 0;
    div_log10_             = 0;
    exponent_              = 0;
    integer_value_         = 0;
    double_value_          = false;
    negative_exponent_     = false;
    negative_              = false;
    
#line 61 "/Users/bruno/work/excelscriptor/casper/number_parser.cc"
	{
	(    cs_) = number_parser_machine_start;
	(    ts_) = 0;
	(    te_) = 0;
	(   act_) = 0;
	}

#line 62 "/Users/bruno/work/excelscriptor/casper/number_parser.rl"
}

/**
 * @brief Destructor.
 */
casper::NumberParser::~NumberParser ()
{
    /* empty */
}

#ifdef __APPLE_
#pragma mark -
#endif

/**
 * @brief Parse a 'double' number from a string.
 *
 * @return The number value, NAN if it's not a number.
 */
double casper::NumberParser::ParseDouble (const std::string& a_number)
{
    Parse(a_number);
    return DoubleValue();
}

/**
 * @brief Parse a 'float' number from a string.
 *
 * @return The number value, NAN if it's not a number.
 */
float casper::NumberParser::ParseFloat (const std::string& a_number)
{
    Parse(a_number);
    return FloatValue();
}

/**
 * @brief Parse a 'int' number from a string.
 *
 * @return The number value, NAN if it's not a number.
 */
int casper::NumberParser::ParseInt (const std::string& a_number)
{
    Parse(a_number);
    return IntValue();
}

#ifdef __APPLE_
#pragma mark -
#endif

/**
 * @brief Parse a number from a string.
 */
void casper::NumberParser::Parse (const std::string& a_number)
{
    number_                = NAN;
    integer_               = 0;
    fractional_            = 0;
    fractional_digits_cnt_ = 0;
    divider_               = 0;
    div_log10_             = 0;
    exponent_              = 0;
    integer_value_         = 0;
    double_value_          = false;
    negative_exponent_     = false;
    negative_              = false;

    char const* p_;
    char const* pe_;
    char const* eof_;

    p_          = (char*) a_number.c_str();
    eof_ = pe_  = p_ + a_number.size();

    
#line 146 "/Users/bruno/work/excelscriptor/casper/number_parser.cc"
	{
	(    cs_) = number_parser_machine_start;
	(    ts_) = 0;
	(    te_) = 0;
	(   act_) = 0;
	}

#line 138 "/Users/bruno/work/excelscriptor/casper/number_parser.rl"
    
#line 156 "/Users/bruno/work/excelscriptor/casper/number_parser.cc"
	{
	if ( (     p_) == (    pe_) )
		goto _test_eof;
	switch ( (    cs_) )
	{
tr2:
#line 34 "/Users/bruno/work/excelscriptor/casper/number_parser.rl"
	{{(     p_) = (((    te_)))-1;}{
        number_ = double_value_;
        {(     p_)++; (    cs_) = 5; goto _out;}
    }}
	goto st5;
tr8:
#line 87 "/Users/bruno/work/excelscriptor/casper/../osal/ragelib/generic_number_parser.rlh"
	{
        if ( divider_ == 1 && exponent_ == 0 ) {
            integer_value_ = negative_ ? -integer_ : integer_;
            double_value_  = integer_value_;
        } else {
            if ( negative_ == false ) {
                double_value_ = integer_ + (double) fractional_ / (double) divider_;
            } else {
                double_value_ = -(integer_ + (double) fractional_ / (double) divider_);
            }
            if ( exponent_ != 0 ) {
                if ( negative_exponent_ == false ) {
                    double_value_ *= (double) osal::utils::Pow10((int)exponent_);
                } else {
                    double_value_ /= (double) osal::utils::Pow10((int)exponent_);
                }
            }
        }
    }
#line 34 "/Users/bruno/work/excelscriptor/casper/number_parser.rl"
	{(    te_) = (     p_);(     p_)--;{
        number_ = double_value_;
        {(     p_)++; (    cs_) = 5; goto _out;}
    }}
	goto st5;
st5:
#line 1 "NONE"
	{(    ts_) = 0;}
	if ( ++(     p_) == (    pe_) )
		goto _test_eof5;
case 5:
#line 1 "NONE"
	{(    ts_) = (     p_);}
#line 204 "/Users/bruno/work/excelscriptor/casper/number_parser.cc"
	switch( (*(     p_)) ) {
		case 43: goto tr6;
		case 45: goto tr6;
	}
	if ( 48 <= (*(     p_)) && (*(     p_)) <= 57 )
		goto tr7;
	goto st0;
st0:
(    cs_) = 0;
	goto _out;
tr6:
#line 75 "/Users/bruno/work/excelscriptor/casper/../osal/ragelib/generic_number_parser.rlh"
	{
        negative_              = false;
        integer_               = 0;
        fractional_            = 0;
        fractional_digits_cnt_ = 0;
        fractional_digits_cnt_ = 0;
        divider_               = 1;
        negative_exponent_     = false;
        exponent_              = 0;
    }
#line 38 "/Users/bruno/work/excelscriptor/casper/../osal/ragelib/generic_number_parser.rlh"
	{
        if ( (*(     p_)) == '-' ) {
            negative_ = true;
        }
    }
	goto st1;
st1:
	if ( ++(     p_) == (    pe_) )
		goto _test_eof1;
case 1:
#line 238 "/Users/bruno/work/excelscriptor/casper/number_parser.cc"
	if ( 48 <= (*(     p_)) && (*(     p_)) <= 57 )
		goto tr0;
	goto st0;
tr0:
#line 1 "NONE"
	{(    te_) = (     p_)+1;}
#line 45 "/Users/bruno/work/excelscriptor/casper/../osal/ragelib/generic_number_parser.rlh"
	{
        integer_ *= 10;
        integer_ += ((*(     p_)) - '0');
    }
	goto st6;
tr7:
#line 1 "NONE"
	{(    te_) = (     p_)+1;}
#line 75 "/Users/bruno/work/excelscriptor/casper/../osal/ragelib/generic_number_parser.rlh"
	{
        negative_              = false;
        integer_               = 0;
        fractional_            = 0;
        fractional_digits_cnt_ = 0;
        fractional_digits_cnt_ = 0;
        divider_               = 1;
        negative_exponent_     = false;
        exponent_              = 0;
    }
#line 45 "/Users/bruno/work/excelscriptor/casper/../osal/ragelib/generic_number_parser.rlh"
	{
        integer_ *= 10;
        integer_ += ((*(     p_)) - '0');
    }
	goto st6;
st6:
	if ( ++(     p_) == (    pe_) )
		goto _test_eof6;
case 6:
#line 275 "/Users/bruno/work/excelscriptor/casper/number_parser.cc"
	switch( (*(     p_)) ) {
		case 44: goto st2;
		case 46: goto st2;
		case 69: goto st3;
		case 101: goto st3;
	}
	if ( 48 <= (*(     p_)) && (*(     p_)) <= 57 )
		goto tr0;
	goto tr8;
st2:
	if ( ++(     p_) == (    pe_) )
		goto _test_eof2;
case 2:
	if ( 48 <= (*(     p_)) && (*(     p_)) <= 57 )
		goto tr3;
	goto tr2;
tr3:
#line 1 "NONE"
	{(    te_) = (     p_)+1;}
#line 51 "/Users/bruno/work/excelscriptor/casper/../osal/ragelib/generic_number_parser.rlh"
	{
        if ( ( fractional_digits_cnt_ + 1 ) < k_max_fractional_digits_ ) {
            fractional_ *= 10;
            divider_    *= 10;
            fractional_ += ((*(     p_)) - '0');
            fractional_digits_cnt_++;
        }
    }
	goto st7;
st7:
	if ( ++(     p_) == (    pe_) )
		goto _test_eof7;
case 7:
#line 309 "/Users/bruno/work/excelscriptor/casper/number_parser.cc"
	switch( (*(     p_)) ) {
		case 69: goto st3;
		case 101: goto st3;
	}
	if ( 48 <= (*(     p_)) && (*(     p_)) <= 57 )
		goto tr3;
	goto tr8;
st3:
	if ( ++(     p_) == (    pe_) )
		goto _test_eof3;
case 3:
	switch( (*(     p_)) ) {
		case 43: goto tr4;
		case 45: goto tr4;
	}
	if ( 48 <= (*(     p_)) && (*(     p_)) <= 57 )
		goto tr5;
	goto tr2;
tr4:
#line 67 "/Users/bruno/work/excelscriptor/casper/../osal/ragelib/generic_number_parser.rlh"
	{
        if ( (*(     p_)) == '-' ) {
            negative_exponent_ = true;
        }
    }
	goto st4;
st4:
	if ( ++(     p_) == (    pe_) )
		goto _test_eof4;
case 4:
#line 340 "/Users/bruno/work/excelscriptor/casper/number_parser.cc"
	if ( 48 <= (*(     p_)) && (*(     p_)) <= 57 )
		goto tr5;
	goto tr2;
tr5:
#line 61 "/Users/bruno/work/excelscriptor/casper/../osal/ragelib/generic_number_parser.rlh"
	{
        exponent_ *= 10;
        exponent_ += ((*(     p_)) - '0');
    }
	goto st8;
st8:
	if ( ++(     p_) == (    pe_) )
		goto _test_eof8;
case 8:
#line 355 "/Users/bruno/work/excelscriptor/casper/number_parser.cc"
	if ( 48 <= (*(     p_)) && (*(     p_)) <= 57 )
		goto tr5;
	goto tr8;
	}
	_test_eof5: (    cs_) = 5; goto _test_eof; 
	_test_eof1: (    cs_) = 1; goto _test_eof; 
	_test_eof6: (    cs_) = 6; goto _test_eof; 
	_test_eof2: (    cs_) = 2; goto _test_eof; 
	_test_eof7: (    cs_) = 7; goto _test_eof; 
	_test_eof3: (    cs_) = 3; goto _test_eof; 
	_test_eof4: (    cs_) = 4; goto _test_eof; 
	_test_eof8: (    cs_) = 8; goto _test_eof; 

	_test_eof: {}
	if ( (     p_) == (   eof_) )
	{
	switch ( (    cs_) ) {
	case 6: goto tr8;
	case 2: goto tr2;
	case 7: goto tr8;
	case 3: goto tr2;
	case 4: goto tr2;
	case 8: goto tr8;
	}
	}

	_out: {}
	}

#line 139 "/Users/bruno/work/excelscriptor/casper/number_parser.rl"

    OSAL_UNUSED_PARAM(number_parser_machine_start);
    OSAL_UNUSED_PARAM(number_parser_machine_first_final);
    OSAL_UNUSED_PARAM(number_parser_machine_error);
    OSAL_UNUSED_PARAM(number_parser_machine_en_main);
}

#ifdef __APPLE__
#pragma mark -
#endif

/**
 * @brief Static helper to parse a double number from a string.
 *
 * @param a_value The number ( string representation )
 * @param o_value The parsed number.
 *
 * @return True on success, false on error ( if not a number ).
 */
bool casper::NumberParser::ParseDouble (const std::string& a_value, double& o_value)
{
    casper::NumberParser number_parser;

    const double tmp = number_parser.ParseDouble(a_value);
    if ( false == number_parser.IsNAN()  ) {
        o_value = tmp;
        return true;
    }

    // NAN
    return false;
}

/**
 * @brief Static helper to parse a float number from a string.
 *
 * @param a_value The number ( string representation )
 * @param o_value The parsed number.
 *
 * @return True on success, false on error ( if not a number ).
 */
bool casper::NumberParser::ParseFloat (const std::string& a_value, float& o_value)
{
    casper::NumberParser number_parser;

    const float tmp = number_parser.ParseFloat(a_value);
    if ( false == number_parser.IsNAN()  ) {
        o_value = tmp;
        return true;
    }

    // NAN
    return false;
}

/**
 * @brief Static helper to parse a float number from a string.
 *
 * @param a_value The number ( string representation )
 * @param o_value The parsed number.
 *
 * @return True on success, false on error ( if not a number ).
 */
bool casper::NumberParser::ParseInt (const std::string& a_value, int& o_value)
{
    casper::NumberParser number_parser;

    const int tmp = number_parser.ParseInt(a_value);
    if ( false == number_parser.IsNAN()  ) {
        o_value = tmp;
        return true;
    }

    // NAN
    return false;
}
