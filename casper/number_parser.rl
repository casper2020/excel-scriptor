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

%%{
    machine number_parser_machine;

    write data;

    include generic_number_parser "../osal/ragelib/generic_number_parser.rlh";

    main := |*
    double_number => {
        number_ = double_value_;
        fbreak;
    };
    *|;

}%%

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
    %% write init;
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

    %% write init;
    %% write exec;

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
