/**
 * @file term.rl Implementation of Term
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

#include "casper/term.h"

#include <locale.h>
#include <inttypes.h>
#include "osal/osalite.h"
#if !defined(CASPER_NO_ICU)
#include "casper/java/fake_java_expression.h"
#endif

#include <cmath> // std::signbit

const char* casper::Term::k_default_date_format_ = "yyyy-MM-dd";

const char* casper::Term::kErrors[] = {
    /*          not used        */ "#ERROR!",
    /* ENan        = 0x10000000 */ "#N/A",
    /* EValue      = 0x20000000 */ "#VALUE!",
    /* ERef        = 0x30000000 */ "#REF!",
    /* EDiv0       = 0x40000000 */ "#DIV/0",
    /* ENumError   = 0x50000000 */ "#NUM!",
    /* ENullError  = 0x60000000 */ "#NULL!",
    /* ENameError  = 0x70000000 */ "#NAME?",
    /* ETypeError  = 0x80000000 */ "#TYPE?",
    /* EIndexError = 0x90000000 */ "#Index"
};

const char* casper::Term::ErrorMsg () const
{
    size_t error_index = type_ >> 28;
    if ( error_index < sizeof(kErrors)/sizeof(kErrors[0]) ) {
        return kErrors[error_index];
    } else {
        return kErrors[0];
    }
}

void casper::Term::ExcelDateValue (Term& a_date_text)
{
    number_ = osal::Date::ToExcelDate(a_date_text.text_);
    if ( false == isnan(number_) ) {
        type_ = EExcelDate;
    } else {
        type_ = EValue;
    }
}

void casper::Term::SetLocale (const char *a_locale)
{
OSALITE_TODO("This is a 'Martelada'")
#if defined(CASPER_NO_ICU)
    char temp_fix[6];

    strncpy(temp_fix, a_locale, sizeof(temp_fix));
    temp_fix[5] = 0;
    for ( size_t i = 0; i < sizeof(temp_fix); ++i) {
        if ( '-' == temp_fix[i] ) {
            temp_fix[i] = '_';
        }
    }
    // @TODO per-thread set
    char* rv = setlocale(LC_NUMERIC, temp_fix);
    if ( NULL == rv ) {
        throw OSAL_EXCEPTION("Unrecognized locale '%s'!", a_locale);
    }
#else
    OSAL_UNUSED_PARAM(a_locale);
#endif
}

void casper::Term::GetLocale (std::string& o_locale)
{
    // @TODO per-thread get
    const char* lc_numeric_locale = setlocale(LC_NUMERIC, NULL);
    if ( NULL == lc_numeric_locale ) {
        throw OSAL_EXCEPTION_NA("Unable to retrieve locale!");
    }
    o_locale = lc_numeric_locale;
}

bool casper::Term::NumericSeparator (std::string& o_point)
{
    // @TODO per-thread get
    const struct lconv * lc = localeconv();
    if ( NULL == lc || NULL == lc->decimal_point ) {
        return false;
    }
    o_point = lc->decimal_point;
    return true;
}

void casper::Term::NewDate ()
{
#if !defined(CASPER_NO_ICU)
    U_ICU_NAMESPACE::DateFormat* date_format = U_ICU_NAMESPACE::DateFormat::createDateInstance();
    if ( NULL != date_format  ){
        number_ = static_cast<double>(date_format->getCalendar()->getNow());
        type_   = EDate;
        delete date_format;
    }
#endif
}

#if !defined(CASPER_NO_ICU)

void casper::Term::ParseDate (casper::Term &a_date, casper::Term &a_pattern, const java::FakeJavaExpression& a_expression)
{
    if ( a_date.IsNull() ) {
        SetNull();
        return;
    }
    if ( EText != a_date.type_ || EText != a_pattern.type_ ) {
        if ( EDate != a_date.type_ ) {
            SetAndHandleError(EInvalidType);
        } else {
            type_   = a_date.type_;
            number_ = a_date.number_;
            return;
        }
    }
    ParseDate(a_date.text_.c_str(), a_pattern.text_.c_str(), *a_expression.GetLocale());
}

#endif

std::string casper::Term::AsString () const
{
    std::string json_string;

    if ( type_ & EErrorMask ) {
        if ( ENullError != type_ ) {
            throw OSAL_EXCEPTION_NA("Can't convert to string : an error is set!");
        }
    } else if ( 0 != (type_ & EErrorMask) ) {
        throw OSAL_EXCEPTION("Can't convert to string : %s", kErrors[type_ >> 28]);
    }
    
    const int type = ( 0xF0 & type_ ) ? ( 0xF0 & type_ ) : ( 0x0F & type_ );

    switch (type) {

        case casper::Term::EText:
        {
            json_string += text_;
            break;
        }

        case casper::Term::ENumber:
        {
            if ( isnan(number_) ) {
                json_string += "null";
            } else if ( 0.0 == number_ ) {  // ... avoid -0 ...
                json_string += "0";
            } else {
                char buf[40];

                int len = sprintf(buf, "%f", number_);
                for ( int i = len - 1; i; --i ) {
                    if ( buf[i] == '0' ) {
                        buf[i] = 0;
                        continue;
                    } else if ( buf[i] == '.' ||  buf[i] == ',' ) {
                        buf[i] = 0;
                        break;
                    } else {
                        break;
                    }
                }
                json_string += buf;
            }
            break;
        }

        case casper::Term::EBoolean:
            json_string += number_ != 0.0 ? "true" : "false";
            break;
            

        case casper::Term::EExcelDate:
            if ( 0.0 == number_ ) {
                json_string += "null";
            } else {
                json_string += osal::Date::ExcelDateToISO8601(number_);
            }
            break;
            
        default:
        {
            if ( ENullError == type_ ) {
                json_string = "null";
            } else {
                throw OSAL_EXCEPTION("Can't convert to string : don't know how to convert type 0x%X to string!",
                                     type_);
            }
        }

    }

    return json_string;
}

std::string casper::Term::DebugString () const
{
    if ( casper::Term::EUndefined == type_ ) {
        return "EUndefined";
    } else if ( ( type_ & EErrorMask ) ) {
        return ToStringBoolAsNum();
    }
    return AsString();
}

void casper::Term::SetFromString (const std::string& a_value)
{
    switch (type_) {

        case ENumber:
        case EBoolean:
        {
            NumberParser number_parser;
            number_ = number_parser.ParseDouble(a_value);
            break;
        }

        case EText:
            text_ = a_value;
            break;

        case EDate:
            throw OSAL_EXCEPTION("Can't set value to string : use ParseDate to convert string '%s' to date!",
                                 a_value.c_str());

        default:
            throw OSAL_EXCEPTION("Can't set value to string : don't know how to convert string '%s' to type 0x%X!",
                                 a_value.c_str(), type_);
    }
}
