
#line 1 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"
/**
 * @file osal_date.rl - ISO-8601 ( subset ) date.
 *
 * Copyright (c) 2010-2016 Neto Ranito & Seabra LDA. All rights reserved.
 *
 * This file is part of osal.
 *
 * osal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osal  is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with osal.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "osal/osal_date.h"
#include "osal/osal_time.h"
#include "osal/osal_types.h"
#include "osal/osalite.h"
#include <math.h> // NAN
#include <limits> // std::numeric_limits
#include <vector>
#include <assert.h>
#include <cmath> // std::round

const double      osal::Date::k_seconds_per_day_                         = 24.0 * 60.0 * 60.0;
const double      osal::Date::k_excel_time_offset_                       = 25569.0;
const char* const osal::Date::k_default_iso8601_date_format_             = "%04d-%02d-%02d";                          // YYYY-MM-DD
const char* const osal::Date::k_default_iso8601_combined_in_utc_format_  = "%04d-%02d-%02dT%02d:%02d:%02dZ";          // YYYY-MM-DDTHH:MM:SSZ

/**
 * Default constructor.
 */
osal::Date::Date ()
{
    /* empty */    
}

/**
 * @brief Destructor.
 */
osal::Date::~Date ()
{
    /* empty */
}

#ifdef __APPLE_
#pragma mark -
#endif

/**
 * @brief Parse a 'date' from a string to an 'Excel' number representation.
 *
 * @param a_value
 *
 * @return
 */
double osal::Date::ToExcelDate (const std::string& a_value)
{
	osal::Time::HumanReadableTime human_time;
    int      cs;
    char*    p   = (char*) a_value.c_str();
    char*    pe  = p + a_value.size();
    double   rv  = NAN;

    human_time.seconds_ = 0;
    human_time.minutes_ = 0;
    human_time.hours_   = 0;
    human_time.weekday_ = std::numeric_limits<uint8_t>::max();

    
#line 81 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
static const int excel_date_value_start = 1;
static const int excel_date_value_first_final = 32;
static const int excel_date_value_error = 0;

static const int excel_date_value_en_main = 1;


#line 89 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
	{
	cs = excel_date_value_start;
	}

#line 94 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr2;
	goto st0;
tr2:
#line 91 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"
	{
            human_time.day_ = (p[-1] - '0') * 10 + (p[0] - '0');
        }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 124 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
	switch( (*p) ) {
		case 45: goto st4;
		case 47: goto st4;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st11;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st5;
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr6;
	goto st0;
tr6:
#line 86 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"
	{
            human_time.month_ = (p[-1] - '0') * 10 + (p[0] - '0');
        }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 156 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
	switch( (*p) ) {
		case 45: goto st7;
		case 47: goto st7;
	}
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st8;
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st9;
	goto st0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st10;
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr11;
	goto st0;
tr11:
#line 81 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"
	{
            human_time.year_ = (p[-3] - '0') * 1000 + (p[-2] - '0') * 100 + (p[-1] - '0') * 10 + (p[0] - '0');
        }
	goto st32;
tr33:
#line 116 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"
	{
            human_time.tz_minutes_ = (p[-1] - '0') * 10 + (p[0] - '0');
        }
	goto st32;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
#line 206 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
	goto st0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr12;
	goto st0;
tr12:
#line 81 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"
	{
            human_time.year_ = (p[-3] - '0') * 1000 + (p[-2] - '0') * 100 + (p[-1] - '0') * 10 + (p[0] - '0');
        }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 225 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
	switch( (*p) ) {
		case 45: goto st13;
		case 47: goto st13;
	}
	goto st0;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st14;
	goto st0;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr15;
	goto st0;
tr15:
#line 86 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"
	{
            human_time.month_ = (p[-1] - '0') * 10 + (p[0] - '0');
        }
	goto st15;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
#line 255 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
	switch( (*p) ) {
		case 45: goto st16;
		case 47: goto st16;
	}
	goto st0;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st17;
	goto st0;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr18;
	goto st0;
tr18:
#line 91 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"
	{
            human_time.day_ = (p[-1] - '0') * 10 + (p[0] - '0');
        }
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
#line 285 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
	if ( (*p) == 84 )
		goto st18;
	goto st0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st19;
	goto st0;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr20;
	goto st0;
tr20:
#line 96 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"
	{
            human_time.hours_ = (p[-1] - '0') * 10 + (p[0] - '0');
        }
	goto st20;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
#line 313 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
	if ( (*p) == 58 )
		goto st21;
	goto st0;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st22;
	goto st0;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr23;
	goto st0;
tr23:
#line 101 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"
	{
            human_time.minutes_ = (p[-1] - '0') * 10 + (p[0] - '0');
        }
	goto st23;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
#line 341 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
	if ( (*p) == 58 )
		goto st24;
	goto st0;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st25;
	goto st0;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr26;
	goto st0;
tr26:
#line 106 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"
	{
            human_time.seconds_ = (p[-1] - '0') * 10 + (p[0] - '0');
        }
	goto st26;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
#line 369 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
	switch( (*p) ) {
		case 43: goto st27;
		case 90: goto st32;
	}
	goto st0;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st28;
	goto st0;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr30;
	goto st0;
tr30:
#line 111 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"
	{
            human_time.tz_hours_   = (p[-1] - '0') * 10 + (p[0] - '0');
        }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 399 "/Users/bruno/work/excelscriptor/osal/osal_date.cc"
	if ( (*p) == 58 )
		goto st30;
	goto st0;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto st31;
	goto st0;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr33;
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof32: cs = 32; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof33: cs = 33; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
	_test_eof29: cs = 29; goto _test_eof; 
	_test_eof30: cs = 30; goto _test_eof; 
	_test_eof31: cs = 31; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 132 "/Users/bruno/work/excelscriptor/osal/osal_date.rl"


    if ( cs >= excel_date_value_first_final ) {
        rv = osal::Time::GetUtcEpochFromHumanReadableTime(human_time) / osal::Date::k_seconds_per_day_ + osal::Date::k_excel_time_offset_;
    }

    OSAL_UNUSED_PARAM(excel_date_value_error);
    OSAL_UNUSED_PARAM(excel_date_value_en_main);

    return rv;
}

/**
 * @brief Convert date components int to an a 'Excel' date.
 *
 * @param a_year
 * @param a_month
 * @param a_day
 *
 * @return
 */
double osal::Date::ToExcelDate (const uint16_t& a_year, const uint8_t& a_month, const uint8_t& a_day)
{
    osal::Time::HumanReadableTime human_time;
    
    human_time.seconds_    = 0;
    human_time.minutes_    = 0;
    human_time.hours_      = 0;
    human_time.year_       = a_year;
    human_time.day_        = a_day;
    human_time.weekday_    = std::numeric_limits<uint8_t>::max();
    human_time.month_      = a_month;
    human_time.tz_hours_   = 0;
    human_time.tz_minutes_ = 0;
    human_time.isdst_      = 0;

    return ((static_cast<double>(osal::Time::GetUtcEpochFromHumanReadableTime(human_time)) / osal::Date::k_seconds_per_day_) + osal::Date::k_excel_time_offset_);
}

/**
 * @brief Extract from an 'Excel' date the 'year' component.
 *
 * @param a_date
 *
 * @return
 */
uint16_t osal::Date::YearFromExcelDate (const double& a_date)
{
    osal::Time::HumanReadableTime human_time;    
    osal::Time::GetHumanReadableTimeFromUTC(static_cast<int64_t>(ExcelDateToEpoch(a_date)), human_time);
    return human_time.year_;
}

/**
 * @brief Extract from an 'Excel' date the 'month' component.
 *
 * @param a_date
 *
 * @return
 */
uint8_t osal::Date::MonthFromExcelDate (const double& a_date)
{
    osal::Time::HumanReadableTime human_time;    
    osal::Time::GetHumanReadableTimeFromUTC(static_cast<int64_t>(ExcelDateToEpoch(a_date)), human_time);
    return human_time.month_;
}

/**
 * @brief Extract from an 'Excel' date the 'day' component.
 *
 * @param a_date
 *
 * @return
 */
uint8_t osal::Date::DayFromExcelDate (const double& a_date)
{
    osal::Time::HumanReadableTime human_time;    
    osal::Time::GetHumanReadableTimeFromUTC(static_cast<int64_t>(ExcelDateToEpoch(a_date)), human_time);
    return human_time.day_;
}

/**
 * @brief Extract from an 'Excel' date the 'hours' component.
 *
 * @param a_date
 *
 * @return
 */
uint8_t osal::Date::HoursFromExcelDate (const double& a_date)
{
    osal::Time::HumanReadableTime human_time;    
    osal::Time::GetHumanReadableTimeFromUTC(static_cast<int64_t>(ExcelDateToEpoch(a_date)), human_time);
    return human_time.hours_;
}

/**
 * @brief Extract from an 'Excel' date the 'minutes' component.
 *
 * @param a_date
 *
 * @return
 */
uint8_t osal::Date::MinutesFromExcelDate (const double& a_date)
{
    osal::Time::HumanReadableTime human_time;    
    osal::Time::GetHumanReadableTimeFromUTC(static_cast<int64_t>(ExcelDateToEpoch(a_date)), human_time);
    return human_time.minutes_;
}

/**
 * @brief Extract from an 'Excel' date the 'seconds' component.
 *
 * @param a_date
 *
 * @return
 */
uint8_t osal::Date::SecondsFromExcelDate (const double& a_date)
{
    osal::Time::HumanReadableTime human_time;    
    osal::Time::GetHumanReadableTimeFromUTC(static_cast<int64_t>(ExcelDateToEpoch(a_date)), human_time);
    return human_time.seconds_;
}

/**
 * @brief Convert an 'epoch' value to an a 'Excel' date.
 *
 * @param a_epoch
 *
 * @return
 */
double osal::Date::EpochToExcelDate (const time_t& a_epoch)
{
    return ((static_cast<double>(a_epoch) / osal::Date::k_seconds_per_day_) + osal::Date::k_excel_time_offset_);
}

/**
 * @brief Convert an 'Excel' date to 'epoch'.
 *
 * @param a_date
 *
 * @return
 */
time_t osal::Date::ExcelDateToEpoch (const double& a_date)
{
    return static_cast<time_t>((a_date - osal::Date::k_excel_time_offset_) * osal::Date::k_seconds_per_day_ + 0.5);
}

/**
 * @brief Convert an excel date to ISO8601 date format.
 *        YYYY-MM-DD
 *
 * @param a_date
 * 
 * @return
 */
std::string osal::Date::ExcelDateToISO8601 (const double& a_date)
{
    if ( 0.0 == a_date ) {
        return "";
    }
    osal::Time::HumanReadableTime human_time;
    if ( true == osal::Time::GetUtcHumanReadableTimeFromUTC(static_cast<int64_t>(osal::Date::ExcelDateToEpoch(a_date)), human_time) ) {
        const int required_buffer_size = std::snprintf(nullptr, 0, 
                                                       osal::Date::k_default_iso8601_date_format_,
                                                       static_cast<int>(human_time.year_), static_cast<int>(human_time.month_), static_cast<int>(human_time.day_)
        );
        std::vector<char> buffer(required_buffer_size + 1);
        const int bytes_written = std::snprintf(&buffer[0], buffer.size(),
                                                osal::Date::k_default_iso8601_date_format_,
                                                static_cast<int>(human_time.year_), static_cast<int>(human_time.month_), static_cast<int>(human_time.day_)
        );
        return bytes_written > 1 ? std::string { buffer.begin(), buffer.end() - 1 } : "";
    } else {
        return "";
    }
}

/**
 * @brief Convert an excel date to ISO8601 date and time combined format in UTC.
 *        YYYY-MM-DDTHH:MM:SSZ
 *
 * @param a_date
 * 
 * @return
 */
std::string osal::Date::ExcelDateToISO8601CombinedInUTC (const double& a_date)
{
    if ( 0.0 == a_date ) {
        return "";
    }
    osal::Time::HumanReadableTime human_time;
    if ( true == osal::Time::GetUtcHumanReadableTimeFromUTC(static_cast<int64_t>(osal::Date::ExcelDateToEpoch(a_date)), human_time) ) {
        const int required_buffer_size = std::snprintf(nullptr, 0, 
                                                       osal::Date::k_default_iso8601_combined_in_utc_format_,
                                                       static_cast<int>(human_time.year_ ), static_cast<int>(human_time.month_  ), static_cast<int>(human_time.day_    ),
                                                       static_cast<int>(human_time.hours_), static_cast<int>(human_time.minutes_), static_cast<int>(human_time.seconds_)
        );
        std::vector<char> buffer(required_buffer_size + 1);
        const int bytes_written = std::snprintf(&buffer[0], buffer.size(),
                                                osal::Date::k_default_iso8601_combined_in_utc_format_,
                                                static_cast<int>(human_time.year_ ), static_cast<int>(human_time.month_  ), static_cast<int>(human_time.day_    ),
                                                static_cast<int>(human_time.hours_), static_cast<int>(human_time.minutes_), static_cast<int>(human_time.seconds_)
        );
        return bytes_written > 1 ? std::string { buffer.begin(), buffer.end() - 1 } : "";
    } else {
        return "";
    }
}

/**
 * @brief Convert an epoch date to ISO8601 date and time combined format in UTC.
 *        YYYY-MM-DDTHH:MM:SSZ
 *
 * @param a_date
 * 
 * @return
 */
std::string osal::Date::EpochToISO8601CombinedInUTC (const time_t& a_epoch)
{
    osal::Time::HumanReadableTime human_time;
    if ( true == osal::Time::GetUtcHumanReadableTimeFromUTC(static_cast<int64_t>(a_epoch), human_time) ) {
        const int required_buffer_size = std::snprintf(nullptr, 0, 
                                                       osal::Date::k_default_iso8601_combined_in_utc_format_,
                                                       static_cast<int>(human_time.year_ ), static_cast<int>(human_time.month_  ), static_cast<int>(human_time.day_    ),
                                                       static_cast<int>(human_time.hours_), static_cast<int>(human_time.minutes_), static_cast<int>(human_time.seconds_)
        );
        std::vector<char> buffer(required_buffer_size + 1);
        const int bytes_written = std::snprintf(&buffer[0], buffer.size(),
                                                osal::Date::k_default_iso8601_combined_in_utc_format_,
                                                static_cast<int>(human_time.year_ ), static_cast<int>(human_time.month_  ), static_cast<int>(human_time.day_    ),
                                                static_cast<int>(human_time.hours_), static_cast<int>(human_time.minutes_), static_cast<int>(human_time.seconds_)
        );
        return bytes_written > 1 ? std::string { buffer.begin(), buffer.end() - 1 } : "";
    } else {
        return "";
    }
}

/**
 * @brief Run some tests targeting ISO8601 subset conversion funcions implementation.
 */

void osal::Date::TestISO80601 ()
{
#if defined(DEBUG) || defined(_DEBUG) || defined(ENABLE_DEBUG)
    // ISO8601 -> Excel
    assert(42559.0    == osal::Date::ToExcelDate("2016-07-08")          );
    assert(42559.4078 == std::round(osal::Date::ToExcelDate("2016-07-08T09:47:14Z") * 10000) / 10000);
    assert(42559.4078 == std::round(osal::Date::ToExcelDate("2016-07-08T09:47:14+00:00") * 10000) / 10000);
    // Epoch UTC -> ISO8601 combined UTC format
    const time_t epoch = 1467971234; // Fri, 8 Jul 2016 09:47:14
    assert(0 == osal::Date::EpochToISO8601CombinedInUTC(epoch).compare("2016-07-08T09:47:14Z"));
    // Epoch -> Excel
    const double excel_date = 42559.4078;
    assert(excel_date == std::round(osal::Date::EpochToExcelDate(epoch) * 10000) / 10000);
    // Excel -> ISO8601
    assert(0 == ExcelDateToISO8601(excel_date).compare("2016-07-08"));
    assert(0 == ExcelDateToISO8601CombinedInUTC(excel_date).compare("2016-07-08T09:47:14Z"));
    // YYYY-MM-DD -> Excel
    assert(static_cast<double>(static_cast<int>(excel_date)) == ToExcelDate(/* a_year */2016, /* a_month */ 07, /* a_day */ 8));
    // Date Components from Excel
    assert(2016 == YearFromExcelDate(excel_date));
    assert(7 == MonthFromExcelDate(excel_date));
    assert(8 == DayFromExcelDate(excel_date));
    assert(9 == HoursFromExcelDate(excel_date));
    assert(47 == MinutesFromExcelDate(excel_date));
    assert(14 == SecondsFromExcelDate(excel_date));
#endif
}
