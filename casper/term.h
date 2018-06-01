/**
 * @file term.h declaration of Term class
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
#ifndef NRS_CASPER_CASPER_TERM_H
#define NRS_CASPER_CASPER_TERM_H

#include "osal/osalite.h"
#include "osal/osal_types.h"
#include "osal/osal_time.h"
#include "osal/osal_date.h"
#include "osal/utils/pow10.h"
#include "osal/exception.h"

#if !defined(CASPER_NO_ICU)
#include "unicode/ustring.h"
#include "unicode/unistr.h"
#include "unicode/datefmt.h"  // U_ICU_NAMESPACE::DateFormat
#include "unicode/smpdtfmt.h" // U_ICU_NAMESPACE::SimpleDateFormat
#endif

#include <map>
#include <set>
#include <vector>
#include <string>
#include <math.h>

#include <string.h>
#include <stdio.h>
#include <alloca.h>
#include <ctype.h>
#include <algorithm>

#include "casper/number_parser.h"

namespace casper
{
    /*
     * Forward declarations
     */
    namespace see
    {
        class FakeJavaParser;
        class FakeJavaExpression;
        class Parser;
        class Scanner;
        class See;
        class RowShifter;
        class Sum;
        class SumIf;
        class SumIfs;
        class Vlookup;
        class Table;
    }

    /*
     * Type defs for several handy containers
     */
    typedef std::map<std::string, std::string>      StringHash;
    typedef std::multimap<std::string, std::string> StringMultiHash;
    typedef std::vector<std::string>                StringList;
    typedef std::map<std::string, double>           DoubleHash;
    typedef std::map<std::string, int>              IntHash;
    typedef std::map<std::string, bool>             BooleanHash;
    typedef std::vector<int>                        IntList;
    typedef std::set<std::string>                   StringSet;
    typedef std::set<int>                           IntSet;
    
    class Term;
    
    typedef std::map<std::string, Term>             SymbolTable;

    namespace java
    {
        class FakeJavaExpression;
        class FakeJavaParser;
    }
    
    namespace epaper
    {
        namespace calc
        {
            class BasicParser;
            class BasicExpression;
        }
        namespace user_expression
        {
            class UserParser;
            class UserExpression;
        }
    }
    
    /**
     * @brief Performs basic operations on expression terms and holds the result as number or string
     */
    class Term
    {
        friend class Scanner;
        friend class see::Parser;
        friend class see::Scanner;
        friend class see::See;
        friend class see::RowShifter;
        friend class see::Sum;
        friend class see::SumIf;
        friend class see::SumIfs;
        friend class see::Vlookup;
        friend class see::Table;
        friend class java::FakeJavaParser;
        friend class java::FakeJavaExpression;
        friend class epaper::calc::BasicParser;
        friend class epaper::calc::BasicExpression;
        friend class epaper::user_expression::UserParser;
        friend class epaper::user_expression::UserExpression;
        
        static const char* kErrors[];
        
    public: // enumerations
        
        enum {
            /*
             * Types are bit fields as value can be valid
             * in one or more types, e.g. number and boolean
             */
            EUndefined  = 0x00,
            ENumber     = 0x01,
            EText       = 0x02,
            EDate       = 0x04,
            EBoolean    = 0x08,
            EExcelDate  = 0x10,
            
            /*
             * Errors are stored in upper 4 bits
             */
            EErrorMask   = 0xF0000000,
            ENan         = 0x10000000,  // "#N/A"
            EValue       = 0x20000000,  // "#VALUE!"
            ERef         = 0x30000000,  // "#REF!"
            EDiv0        = 0x40000000,  // "#DIV/0"
            ENumError    = 0x50000000,  // "#NUM!"
            ENullError   = 0x60000000,  // "#NULL!"
            ENameError   = 0x70000000,  // "#NAME?"
            EInvalidType = 0x80000000,  // "#TYPE?"
            EIndexError  = 0x90000000   // "#Index"
        };
        
    public:
        
        static const char* k_default_date_format_;
        
    protected:
        
        double      number_;
        std::string text_;
        std::string aux_text_;
        std::string aux_condition_;
        unsigned    type_;
        
    protected: // expression operators and functions
        
        void SetString        (Term& a_term);
        void Maximum          (Term& a_term1, Term& a_term2);
        void Minimum          (Term& a_term1, Term& a_term2);
        void Round            (Term& a_number, Term& a_num_digits);
        void RoundUp          (Term& a_number, Term& a_num_digits);
        void RoundDown        (Term& a_number, Term& a_num_digits);
        void If               (Term& a_logical_condition, Term& a_value_if_true, Term& a_value_if_false);
        void If               (Term& a_logical_condition, Term& a_value_if_true);
        void If               (Term& a_logical_condition);
        void AddNumOrText     (Term& a_term1, Term& a_term2);
        void Add              (Term& a_term1, Term& a_term2);
        void Subtract         (Term& a_term1, Term& a_term2);
        void Multiply         (Term& a_term1, Term& a_term2);
        void Divide           (Term& a_term1, Term& a_term2);
        void UnaryMinus       (Term& a_term);
        void Pow              (Term& a_number, Term& a_power);
        void SetExcelDate     (Term& a_year, Term& a_month, Term& a_day);
        void ExcelYear        (Term& a_date);
        void ExcelMonth       (Term& a_date);
        void ExcelDay         (Term& a_date);
        void ExcelHour        (Term& a_date);
        void ExcelMinute      (Term& a_date);
        void ExcelSecond      (Term& a_date);
        void ExcelDateValue   (Term& a_date_text);
        void Equal            (Term& a_term1, Term& a_term2);
        void NotEqual         (Term& a_term1, Term& a_term2);
        void Concatenate      (Term& a_term1, Term& a_term2);
        void Greater          (Term& a_term1, Term& a_term2);
        void Less             (Term& a_term1, Term& a_term2);
        void GreaterOrEqual   (Term& a_term1, Term& a_term2);
        void LessOrEqual      (Term& a_term1, Term& a_term2);
        void IsError          (Term& a_term);
        void IfError          (Term& a_value, Term& a_value_if_error);
        void Find             (Term& a_text, Term& a_within_text, Term& a_start_num);
        void Find             (Term& a_text, Term& a_within_text);
        void Find             (Term& a_result, Term& a_text, Term& a_within_text, int a_offset);
        void Or               (Term& a_term1, Term& a_term2);
        void And              (Term& a_term1, Term& a_term2);
        void Not              (Term& a_term);
        void Absolute         (Term& a_term);
        void FakeJavaAbs      (Term& a_term);
        void Left             (Term& a_string, Term& a_num_chars);
        void Right            (Term& a_string, Term& a_num_chars);
        void Mid              (Term& a_string, Term& a_beginIndex, Term& a_num_chars);
        void BooleanTrue      ();
        void BooleanFalse     ();
        void IsNan            (Term& a_term);
        void ParseBoolean     (Term& a_term);
        void IsEmpty          (Term& a_string);
        void Length           (Term& a_string);
        void ToUpperCase      (Term& a_string);
        void ToLowerCase      (Term& a_string);
        void Substring        (Term& a_string, Term& a_beginIndex, Term& a_endIndex);
        void Substring        (Term& a_string, Term& a_beginIndex);
        void Substring        (Term& a_string, int a_beginIndex, int a_endIndex);
        void IndexOf          (Term& a_string, Term& a_substring);
        void Replace          (Term& a_string, Term& a_from, Term& a_to);
        void ParseDouble      (Term& a_term);
        void DoubleToString   (Term& a_number);
        void ParseInteger     (Term& a_term);
        void ToI              (Term& a_term);
        void ToF              (Term& a_term);
        void IntegerToString  (Term& a_number);
        void NewDate          ();
        void ParseDate        (Term& a_date, casper::Term &a_pattern, const java::FakeJavaExpression& a_expression);
        double Percentage       ();
        
    public: // Constuctor
        
        Term ();
        Term (int a_type);
        Term (const Term& a_term);
        Term (const std::string& a_string);
        virtual ~Term ();
        
        virtual void SetAndHandleError (int a_error);
        void         operator =        (const char* a_string);
        void         operator =        (std::string a_string);
        void         operator =        (double a_value);
        void         operator =        (bool a_value);
        void         operator =        (const Term& a_term);
        bool         operator ==       (const Term& a_term) const;
        Term         operator -        ();
        
        std::string  ToString          ();
        std::string  AsString          () const;
        std::string  DebugString       () const;
        std::string  ToStringBoolAsNum () const;
        bool         ToBoolean         () const;
        double       ToNumber          () const;
        bool         IsBoolean         () const;
        bool         HasError          () const;
        const char*  ErrorMsg          () const;
        bool         IsNumber          () const;
        void         SetNan            ();
        bool         IsNan             () const;
        bool         IsDate            () const;
        bool         IsNull            () const;
        bool         IsEqual           (const Term& a_term) const;
        bool         GetBoolean        () const;
        double       GetNumber         () const;
        const char*  GetText           () const;
        int          GetType           () const;
#if !defined(CASPER_NO_ICU)
        bool         ParseDate         (const char* a_date, const char* a_pattern, const U_ICU_NAMESPACE::Locale& a_locale);
#endif
        
        void         SetNull           ();
        bool         ConvertToBoolean  ();
        double       ConvertToNumber   ();
        
#if !defined(CASPER_NO_ICU)
        void         Now                ();
        bool         ConvertToDate     (const char* a_pattern, const U_ICU_NAMESPACE::Locale& a_locale);
        Term         ToDate            (const char* a_pattern, const U_ICU_NAMESPACE::Locale& a_locale) const;
        Term         ToNow             () const;
        bool         ToDate            ();
#endif
        void         SetExcelDate      (const char* const a_date);
        
        void         SetFromString     (const std::string& a_value);
        
        static void  SetLocale         (const char* a_locale);
        static void  GetLocale         (std::string& o_locale);
        static bool  NumericSeparator  (std::string& o_point);
    };
    
    inline Term::Term ()
    {
        number_ = 0.0;
        type_   = EUndefined;
    }

    inline Term::Term (int a_type)
    {
        type_   = a_type;
        switch (type_) {
                
            case ENumber:
            case EBoolean:
            case EExcelDate:
            {
                number_ = 0.0;
                break;
            }

#if !defined(CASPER_NO_ICU)
            case EDate:
                Now();
                break;
#endif
            default:
                number_ = 0.0;
                break;
        }        
    }
    
    inline Term::Term (const Term& a_term)
    {
        type_          = a_term.type_;
        number_        = a_term.number_;
        text_          = a_term.text_;
        aux_text_      = a_term.aux_text_;
        aux_condition_ = a_term.aux_condition_;
    }
    
    inline Term::Term (const std::string& a_string)
    {
        type_          = EText;
        number_        = NAN;
        text_          = a_string;
        aux_text_      = "";
        aux_condition_ = "";
    }

    inline Term::~Term ()
    {
        number_ = 0.0;
        type_   = EUndefined;
    }
    
    inline void Term::SetAndHandleError (int a_error)
    {
        /* set invalid values */
        number_        = NAN;
        text_          = "";
        aux_text_      = "";
        aux_condition_ = "";
        type_          = (a_error & EErrorMask);
    }
    
    inline void Term::operator = (const char* a_string)
    {
        text_ = a_string;
        type_ = EText;
    }
    
    inline void Term::operator = (std::string a_string)
    {
        text_ = a_string;
        type_ = EText;
    }
    
    inline void Term::operator = (double a_value)
    {
        number_ = a_value;
        type_   = ENumber;
    }
    
    inline void Term::operator = (bool a_value)
    {
        number_ = a_value ? 1.0 : 0.0;
        type_   = EBoolean;
    }
    
    inline void Term::operator = (const Term& a_term)
    {
        type_          = a_term.type_;
        number_        = a_term.number_;
        text_          = a_term.text_;
        aux_text_      = a_term.aux_text_;
        aux_condition_ = a_term.aux_condition_;
    }
    
    inline bool Term::operator == (const Term& a_term) const
    {
        if ( a_term.type_ != type_ ) {
            
            // null exception:
            if ( IsNull() && casper::Term::EText == a_term.GetType() ) {
                const char* c_str = a_term.GetText();
                if ( NULL == c_str || 0 == strlen(c_str) ) {
                    return true;
                }
            } else if ( a_term.IsNull() && casper::Term::EText == type_ ) {
                const char* c_str = GetText();
                if ( NULL == c_str || 0 == strlen(c_str) ) {
                    return true;
                }
            } else if ( ( true == a_term.IsNull() && ENumber == type_ && true == isnan(number_) ) ||
                        ( true == IsNull() && ENumber == a_term.type_ && true == isnan(a_term.number_) ) ) {
                return true;
            }
            
            return false;
        }
        
        switch (type_) {
                
            case ENumber:
                return a_term.number_ == number_;
                
            case EText:
                return  0 == a_term.text_.compare(text_);
                
            case EDate:
            case EExcelDate:
                return a_term.number_ == number_;
                
            case EBoolean:
                return a_term.number_ == number_;
                
            case ENullError:
                return a_term.type_ == type_;
                
            case ENan:
                return a_term.type_ == type_;
                
            default:
                return false;
        }
    }
    
    inline Term Term::operator - ()
    {
        if ( ENumber != type_ ) {
            throw OSAL_EXCEPTION("Unexpected type '%lu' in operator -!",
                                 type_);
        }
        Term rv = Term(*this);
        rv.number_ = -rv.number_;
        return rv;
    }
    
    inline void Term::Maximum (Term& a_term1, Term& a_term2)
    {
        number_ = MAX(a_term1.ConvertToNumber(), a_term2.ConvertToNumber());
        type_   = a_term1.type_ | a_term2.type_;
    }
    
    inline void Term::Minimum (Term& a_term1, Term& a_term2)
    {
        number_ = MIN(a_term1.ConvertToNumber(), a_term2.ConvertToNumber());
        type_   = a_term1.type_ | a_term2.type_;
    }
    
    inline void Term::Round (Term& a_number, Term& a_num_digits)
    {
        double m = osal::utils::Pow10((int) a_num_digits.ConvertToNumber());
        number_ = round(a_number.ConvertToNumber() * m) / m;
        type_   = a_number.type_ | a_num_digits.type_;
    }
    
    inline void Term::RoundUp (Term& a_number, Term& a_num_digits)
    {
        double m = osal::utils::Pow10((int) a_num_digits.ConvertToNumber());
        if ( a_number.ConvertToNumber() >= 0.0 ) {
            number_ = ceil(a_number.ConvertToNumber() * m)  / m;
        } else {
            number_ = floor(a_number.ConvertToNumber() * m) / m;
        }
        type_   = a_number.type_ | a_num_digits.type_;
    }
    
    inline void Term::RoundDown (Term& a_number, Term& a_num_digits)
    {
        double m = osal::utils::Pow10((int) a_num_digits.ConvertToNumber());
        if ( a_number.number_ >= 0.0 ) {
            number_ = floor(a_number.ConvertToNumber() * m) / m;
        } else {
            number_ = ceil(a_number.ConvertToNumber() * m) / m;
        }
        type_   = a_number.type_ | a_num_digits.type_;
    }
    
    inline void Term::If (Term& a_logical_condition, Term& a_value_if_true, Term& a_value_if_false)
    {
        if ( a_logical_condition.ConvertToNumber() != 0.0 ) {
            *this = a_value_if_true;
        } else {
            *this = a_value_if_false;
        }
    }
    
    inline void Term::If (Term& a_logical_condition, Term& a_value_if_true)
    {
        if ( a_logical_condition.ConvertToNumber() != 0.0 ) {
            *this = a_value_if_true;
        } else {
            *this = 0.0;
        }
    }
    
    inline void Term::If (Term& a_logical_condition)
    {
        if ( a_logical_condition.ConvertToNumber() != 0.0 ) {
            *this = 1.0 ;
        } else {
            *this = 0.0;
        }
    }
    
    inline void Term::Not (Term& a_term)
    {
        if ( a_term.ConvertToNumber() == 0.0 ) {
            number_ = 1.0 ;
            type_   = EBoolean;
        } else {
            number_ = 0.0;
            type_   = EBoolean;
        }
    }
    
    inline void Term::Add (Term& a_term1, Term& a_term2)
    {
        if ( EExcelDate == a_term1.type_ || EExcelDate == a_term2.type_ ) {
            number_ = a_term1.ToNumber() + a_term2.ToNumber();
            type_   = ENumber;
        } else {
            number_ = a_term1.ConvertToNumber() + a_term2.ConvertToNumber();
            type_   = a_term1.type_ | a_term2.type_;
        }
    }
    
    inline void Term::Subtract (Term& a_term1, Term& a_term2)
    {
        if ( EExcelDate == a_term1.type_ || EExcelDate == a_term2.type_ ) {
            number_ = a_term1.ToNumber() - a_term2.ToNumber();
            type_   = ENumber;
        } else {
            number_ = a_term1.ConvertToNumber() - a_term2.ConvertToNumber();
            type_   = a_term1.type_ | a_term2.type_;
        }
    }
    
    inline void Term::Multiply (Term& a_term1, Term& a_term2)
    {
        if ( EExcelDate == a_term1.type_ || EExcelDate == a_term2.type_ ) {
            number_ = a_term1.ToNumber() * a_term2.ToNumber();
            type_   = ENumber;
        } else {
            number_ = a_term1.ConvertToNumber() * a_term2.ConvertToNumber();
            type_   = a_term1.type_ | a_term2.type_;
        }
    }
    
    inline void Term::Divide (Term& a_term1, Term& a_term2)
    {
        if ( EExcelDate == a_term1.type_ || EExcelDate == a_term2.type_ ) {
            const double term2_value = a_term2.ToNumber();
            if ( term2_value != 0.0 ) {
                number_ = a_term1.ToNumber() * term2_value;
                type_   = ENumber;
            } else {
                number_ = NAN;
                type_   = EDiv0;
            }
        } else {
            a_term1.ConvertToNumber();
            a_term2.ConvertToNumber();
            if ( a_term2.number_ != 0.0 ) {
                number_ = a_term1.number_ / a_term2.number_;
                type_   = a_term1.type_ | a_term2.type_;
            } else {
                number_ = NAN;
                type_   = EDiv0;
            }
        }
    }
    
    inline void Term::UnaryMinus (Term& a_term)
    {
        if ( a_term.IsNumber() ) {
            number_ = -a_term.number_;
            type_   = ENumber;
        } else {
            text_  = "-";
            text_ += a_term.ToString();
            type_  = EText;
        }
    }

    inline void Term::FakeJavaAbs (Term& a_term)
    {
        if ( a_term.IsNull() ) {
            number_ = 0.0f;
            type_   = ENumber;
        } else {
            number_ = fabs(a_term.ConvertToNumber());
            if ( isnan(number_) ) {
                type_ = EValue;
            } else {
                type_ = ENumber;
            }
        }
    }

    inline void Term::Absolute (Term& a_term)
    {
        number_ = fabs(a_term.ConvertToNumber());
        if ( isnan(number_) ) {
            type_ = EValue;
        } else {
            type_ = ENumber;
        }
    }
    
    inline void Term::Pow (Term& a_number, Term& a_power)
    {
        number_ = pow(a_number.ConvertToNumber(), a_power.ConvertToNumber());
        type_   = a_number.type_ | a_power.type_;
    }
    
    inline void Term::SetExcelDate (Term& a_year, Term& a_month, Term& a_day)
    {
        double year  = a_year.ToNumber();
        double month = a_month.ToNumber();
        double day   = a_day.ToNumber();
        
        if ( isnan(day) || isnan(month) || isnan(year) ) {
            number_ = NAN;
            type_   = EValue;
        } else {
            number_ = osal::Date::ToExcelDate(static_cast<uint16_t>(year), static_cast<uint8_t>(month), static_cast<uint8_t>(day));
            type_   = EExcelDate;
        }
    }
    
    inline void Term::SetExcelDate (const char* const a_date)
    {
        number_ = osal::Date::ToExcelDate(a_date);
        type_   = EExcelDate;
    }

    inline void Term::ExcelYear (Term& a_date)
    {
        double val = a_date.ToNumber();
        if ( isnan(val) ) {
            number_ = NAN;
            type_   = EValue;
        } else {
            number_ = static_cast<double>(osal::Date::YearFromExcelDate(val));
            type_   = ENumber;
        }
    }
    
    inline void Term::ExcelMonth (Term& a_date)
    {
        double val = a_date.ToNumber();
        if ( isnan(val) ) {
            number_ = NAN;
            type_   = EValue;
        } else {
            number_ = static_cast<double>(osal::Date::MonthFromExcelDate(val));
            type_   = ENumber;
        }
    }
    
    inline void Term::ExcelDay (Term& a_date)
    {
        double val = a_date.ToNumber();
        if ( isnan(val) ) {
            number_ = NAN;
            type_   = EValue;
        } else {
            number_ = static_cast<double>(osal::Date::DayFromExcelDate(val));
            type_   = ENumber;
        }
    }
    
    inline void Term::ExcelHour (Term& a_date)
    {
        double val = a_date.ToNumber();
        if ( isnan(val) ) {
            number_ = NAN;
            type_   = EValue;
        } else {
            number_ = static_cast<double>(osal::Date::HoursFromExcelDate(val));
            type_   = ENumber;
        }
    }
    
    inline void Term::ExcelMinute (Term& a_date)
    {
        double val = a_date.ToNumber();
        if ( isnan(val) ) {
            number_ = NAN;
            type_   = EValue;
        } else {
            number_ = static_cast<double>(osal::Date::MinutesFromExcelDate(val));
            type_   = ENumber;
        }
    }
    
    inline void Term::ExcelSecond (Term& a_date)
    {
        double val = a_date.ToNumber();
        if ( isnan(val) ) {
            number_ = NAN;
            type_   = EValue;
        } else {
            number_ = static_cast<double>(osal::Date::SecondsFromExcelDate(val));
            type_   = ENumber;
        }
    }
    
    inline void Term::SetString (Term& a_term)
    {
        text_ = a_term.text_;
        type_ = EText;
    }
    
    inline void Term::Equal (Term& a_term1, Term& a_term2)
    {
        if ( (a_term1.type_ & EErrorMask) || (a_term2.type_ & EErrorMask) ) {
            if ( (a_term1.type_ & EErrorMask) == (a_term2.type_ & EErrorMask) ){
                number_ = 1.0;
            } else {
                number_ = 0.0;
            }
        } else if ( (a_term1.type_ & (ENumber | EBoolean)) && (a_term2.type_ & (ENumber | EBoolean)) ) {
            number_ = a_term1.number_ == a_term2.number_ ? 1.0 : 0.0;
        } else if ( (a_term1.type_ & (EDate | EExcelDate)) && (a_term2.type_ & (EDate| EExcelDate)) ) {
            number_ = a_term1.number_ == a_term2.number_ ? 1.0 : 0.0;
        } else {
            number_ = strcmp(a_term1.ToString().c_str(), a_term2.ToString().c_str()) == 0 ? 1.0 : 0.0;
        }
        type_   = EBoolean;
    }

    inline bool Term::IsEqual (const Term& a_term) const
    {
        return ToStringBoolAsNum() == a_term.ToStringBoolAsNum();
    }

    inline void Term::NotEqual (Term& a_term1, Term& a_term2)
    {
        if ( (a_term1.type_ & EErrorMask) || (a_term2.type_ & EErrorMask) ) {
            if ( (a_term1.type_ & EErrorMask) != (a_term2.type_ & EErrorMask) ){
                number_ = 1.0;
            } else {
                number_ = 0.0;
            }
        } else if ( (a_term1.type_ & (ENumber | EBoolean)) && (a_term2.type_ & (ENumber | EBoolean)) ) {
            number_ = a_term1.number_ == a_term2.number_ ? 0.0 : 1.0;
        } else if ( (a_term1.type_ & (EDate | EExcelDate)) && (a_term2.type_ & (EDate| EExcelDate)) ) {
            number_ = a_term1.number_ == a_term2.number_ ? 0.0 : 1.0;
        } else {
            number_ = strcmp(a_term1.ToString().c_str(), a_term2.ToString().c_str()) == 0 ? 0.0 : 1.0;
        }
        type_   = EBoolean;
    }
    
    inline void Term::Concatenate (Term& a_term1, Term& a_term2)
    {
        text_  = a_term1.ToString();
        text_ += a_term2.ToString();
        type_  = EText;
    }
    
    inline void Term::Greater (Term& a_term1, Term& a_term2)
    {
        number_ = a_term1.number_ > a_term2.number_ ? 1.0 : 0.0;
        type_   = EBoolean;
    }
    
    inline void Term::Less (Term& a_term1, Term& a_term2)
    {
        number_ = a_term1.number_ < a_term2.number_ ? 1.0 : 0.0;
        type_   = EBoolean;
    }
    
    inline void Term::GreaterOrEqual (Term& a_term1, Term& a_term2)
    {
        number_ = a_term1.number_ >= a_term2.number_ ? 1.0 : 0.0;
        type_   = EBoolean;
    }
    
    inline void Term::LessOrEqual (Term& a_term1, Term& a_term2)
    {
        number_ = a_term1.number_ <= a_term2.number_ ? 1.0 : 0.0;
        type_   = EBoolean;
    }
    
    inline void Term::IsError (Term& a_term)
    {
        if ( (a_term.type_ & EErrorMask) != 0 ) {
            number_ = 1.0;
        } else {
            number_ = 0.0;
        }
        type_   = EBoolean;
    }
    
    inline void Term::IfError (Term& a_value, Term& a_value_if_error)
    {
        if ( (a_value.type_ & EErrorMask) != 0 )  {
            *this = a_value_if_error;
        } else {
            *this = a_value;
        }
    }
    
    inline void Term::Find (Term& a_text, Term& a_within_text)
    {
        Find(*this, a_text, a_within_text, 1);
    }
    
    inline void Term::Find (Term& a_text, Term& a_within_text, Term& a_start_num)
    {
        double offset = a_start_num.ToNumber();
        if ( isnan(offset) == false ) {
            Find(*this, a_text, a_within_text, (int)(offset + 0.5));
        } else {
            number_ = NAN;
            type_   = EValue;
        }
    }
    
    inline void Term::Find (Term& a_result, Term& a_text, Term& a_within_text, int a_offset)
    {
        if (   a_offset >= 1
            && (a_within_text.type_ == ENumber || a_within_text.type_ == EText)
            && (a_text.type_ == ENumber || a_text.type_ == EText) ) {
            
            size_t rv = a_within_text.ToString().find(a_text.ToString(), a_offset - 1);
            if ( rv != std::string::npos ) {
                a_result.type_   = ENumber;
                a_result.number_ = rv + 1;
                return;
            }
        }
        a_result.type_   = EValue;
        a_result.number_ = NAN;
    }
    
    inline void Term::Or (Term& a_term1, Term& a_term2)
    {
        double v1 = a_term1.ConvertToNumber();
        double v2 = a_term2.ConvertToNumber();
        if ( isnan(v1) || isnan(v2) ) {
            type_   = ENan;
            number_ = NAN;
        } else {
            number_ = v1 != 0.0 || v2 != 0.0 ? 1.0 : 0.0;
            type_   = EBoolean;
        }
    }
    
    inline void Term::And (Term& a_term1, Term& a_term2)
    {
        double v1 = a_term1.ConvertToNumber();
        double v2 = a_term2.ConvertToNumber();
        if ( isnan(v1) || isnan(v2) ) {
            type_   = ENan;
            number_ = NAN;
        } else {
            number_ = v1 != 0.0 && v2 != 0.0 ? 1.0 : 0.0;
            type_   = EBoolean;
        }
    }
    
    inline bool Term::ConvertToBoolean ()
    {
        double v = ConvertToNumber();
        if ( isnan(v) || v != 1.0  ) {
            return false;
        } else {
            return true;
        }
    }
    
    inline void Term::Left (Term& a_string, Term& a_num_chars)
    {
        int offset = (int) a_num_chars.ToNumber();
        if ( isnan(offset) || offset < 0 || (a_string.type_ & ~(EText | EBoolean | ENumber)) != 0 ) {
            type_   = EValue;
            number_ = NAN;
        } else {
            text_ = a_string.ToString();
            if ( offset < (int) text_.size() ) {
                text_ = text_.substr(0, offset);
            }
            type_ = EText;
        }
    }
    
    inline void Term::Right (Term& a_string, Term& a_num_chars)
    {
        size_t len;
        
        int offset = (int) a_num_chars.ToNumber();
        if ( isnan(offset) || offset < 0 || (a_string.type_ & ~(EText | EBoolean | ENumber)) != 0 ) {
            type_   = EValue;
            number_ = NAN;
        } else {
            text_ = a_string.ToString();
            len   = text_.size();
            if ( offset < (int) len ) {
                text_ = text_.substr(len - offset, len - 1);
            }
            type_ = EText;
        }
    }
    
    inline void Term::Mid (Term& a_string, Term& a_beginIndex, Term& a_num_chars)
    {
        if ( a_string.type_ & EErrorMask ) {
            SetAndHandleError(a_string.type_);
            return;
        }
        if ( a_string.type_ != EText ) {
            SetAndHandleError(EInvalidType);
            return;
        }
        double begin = a_beginIndex.ToNumber();
        double end   = a_num_chars.ToNumber();
        if ( isnan(begin) || isnan(end) ) {
            SetAndHandleError(ENan);
            return;
        }
        begin -= 1.0;
        end   += begin;
        if ( begin < 0.0 ) {
            SetAndHandleError(EValue);
            return;
        }
        if ( begin >= a_string.text_.size() ) {
            text_ = "";
            type_ = EText;
            return;
        }
        end = MIN(end, a_string.text_.size());
        text_ = a_string.text_.substr((int) begin, (int) (end - begin));
        type_ = EText;
    }
    
    inline bool Term::IsNumber () const
    {
        return type_ & ( ENumber | EBoolean );
    }
    
    inline void Term::SetNan ()
    {
        if ( type_ & (ENumber | EDate | EExcelDate) ) {
            number_ = NAN;
        }
    }
    
    inline bool Term::IsNan () const
    {
        return ( ENumber == type_ || EDate == type_ ) && true == isnan(number_);
    }
    
    inline bool Term::IsDate () const
    {
        return EDate == type_;
    }
    
    inline bool Term::GetBoolean () const
    {
        return 0.0f != number_;
    }

    inline double Term::GetNumber () const
    {
        return number_;
    }

    inline const char* Term::GetText () const
    {
        return text_.c_str();
    }
    
    inline int Term::GetType () const
    {
        return type_;
    }
    
#if !defined(CASPER_NO_ICU)
    inline bool Term::ParseDate (const char* a_date, const char* a_pattern, const U_ICU_NAMESPACE::Locale& a_locale)
    {
        if ( nullptr == a_date || nullptr == a_pattern ) {
            SetAndHandleError(EInvalidType);
            return false;
        }
        
        const size_t len = strlen(a_date);
        
        const char* pattern;
        if ( 8 == len ) {
            pattern = a_pattern;
            if ( 0 == strcmp(a_date, "00000000") ) {
                number_ = 0.0f;
                type_   = EDate;
                return true;
            }
        } else if ( 10 == len ) {
            if ( '-' == a_date[4] && '-' == a_date[7] ) {
                pattern = "yyyy-MM-dd";
            } else if ( '-' == a_date[2] && '-' == a_date[5] ) {
                pattern = "dd-MM-yyyy";
            } else if ( '/' == a_date[4] && '/' == a_date[7] ) {
                pattern = "yyyy/MM/dd";
            } else if ( '/' == a_date[2] && '/' == a_date[5] ) {
                pattern = "dd/MM/yyyy";
            } else {
                SetAndHandleError(EInvalidType);
                return false;
            }
        } else {
            return false;
        }
        

        UErrorCode error_code = U_ZERO_ERROR;
        U_ICU_NAMESPACE::SimpleDateFormat* date_format = new U_ICU_NAMESPACE::SimpleDateFormat(UnicodeString(pattern), a_locale, error_code);
        if ( NULL != date_format  ){
            if ( U_ZERO_ERROR == error_code || U_USING_DEFAULT_WARNING == error_code || U_USING_FALLBACK_WARNING == error_code ) {
                const UDate parsed_date = date_format->parse(UnicodeString(a_date), error_code);
                if ( U_ZERO_ERROR == error_code || U_USING_DEFAULT_WARNING == error_code || U_USING_FALLBACK_WARNING == error_code ) {
                    if ( -3600000 == parsed_date ) {
                        number_ = 0.0f;
                        type_   = EDate;
                    } else {
                        number_ = parsed_date;
                        type_   = EDate;
                    }
                }
            }
            delete date_format;
        }

        return EDate == type_;
    }
#endif
    
    inline double Term::Percentage ()
    {
        if ( ENumber != type_ ) {
            throw OSAL_EXCEPTION("Unexpected type '%lu' in operator -!",
                                 type_);
        }
        number_ /= 100.0f;
        
        return number_;
    }
    
    inline double Term::ToNumber () const
    {
        if ( IsNull() ) {
            return 0.0f;
        } else if ( type_ & (ENumber | EBoolean | EDate | EExcelDate) ) {
            return number_;
        } else if ( type_ == EText ) {
            NumberParser number_parser;
            return number_parser.ParseDouble(text_);
        }
        return NAN;
    }
    
    inline double Term::ConvertToNumber ()
    {
        if ( type_ & EErrorMask ) {
            SetAndHandleError(type_); // do not return here
        }
        if ( IsNull() ) {
            number_ = 0.0f;
            type_   = ENumber;
            return 0.0f;
        }
        if ( type_ & (ENumber | EBoolean | EExcelDate) ) {
            return number_;
        } else if ( type_ == EText ) {
            
            NumberParser number_parser;
            number_ = number_parser.ParseDouble(text_);
            if ( false == isnan(number_) ) {
                type_ = ENumber;
                return number_;
            }
        }
        number_ = NAN;
        type_   = ENan;
        return NAN;
    }
    
#if !defined(CASPER_NO_ICU)
    
    inline void Term::Now ()
    {
        NewDate();
    }
    
    inline bool Term::ConvertToDate (const char* a_pattern, const U_ICU_NAMESPACE::Locale& a_locale)
    {
        if ( type_ & EErrorMask ) {
            SetAndHandleError(type_); // do not return here
        }
        
        if ( EDate == type_ ) {
            return true;
        }
        
        if ( EText == type_ ) {
            if ( ParseDate(text_.c_str(), a_pattern, a_locale) ) {
                return true;
            }
        }
        
        number_ = NAN;
        type_   = ENan;
        return false;
    }
    
    inline Term Term::ToDate (const char* a_pattern, const U_ICU_NAMESPACE::Locale& a_locale) const
    {
        Term term;
        if ( type_ & EText ) {
            term.ParseDate(text_.c_str(), a_pattern, a_locale);
        } else if ( type_ & EDate ) {
            term.type_   = type_;
            term.number_ = number_;
        }
        return term;
    }
    
    inline bool Term::ToDate ()
    {
        if ( type_ & EErrorMask ) {
            SetAndHandleError(type_); // do not return here
        } else if ( EDate == type_ ) {
            return true;
        } else if ( ENumber == type_ ) {
            type_ = EDate;
            return true;
        }
        return false;
    }
    
    inline Term Term::ToNow () const
    {
        Term term;
        term.Now();
        return term;
    }
    
#endif
    
    inline std::string Term::ToString ()
    {
        if ( type_ & EErrorMask ) {
            SetAndHandleError(type_); // do not return here
        }
        if ( type_ == EText ) {
            return text_;
        } else if ( type_ == ENumber ) {
            char buf[40];
            
            int len = sprintf(buf, "%lf", number_);
            for ( int i = len - 1; i; --i ) {
                if ( buf[i] == '0' ) {
                    buf[i] = 0;
                    continue;
                } else if ( buf[i] == '.' ) {
                    buf[i] = 0;
                    break;
                } else {
                    break;
                }
            }
            return buf;
        } else if ( type_ == EBoolean ) {
            return number_ != 0.0 ? "true" : "false";
        } else if ( (type_ & EErrorMask) != 0 ) {
            return kErrors[type_ >> 28];
        } else if ( type_ & EExcelDate ) {
            if ( 0.0 == number_ ) {
                return "";
            } else {
                return osal::Date::ExcelDateToISO8601(number_);
            }
        } else {
            return "???";
        }
    }
    
    inline std::string Term::ToStringBoolAsNum () const
    {
        if ( (type_ & EErrorMask) != 0 ) {
            return kErrors[type_ >> 28];
        } else if ( type_ == EText ) {
            return text_;
        } else if ( type_ & (ENumber | EBoolean) ) {
            char buf[40];
            
            int len = sprintf(buf, "%lf", number_);
            for ( int i = len - 1; i; --i ) {
                if ( buf[i] == '0' ) {
                    buf[i] = 0;
                    continue;
                } else if ( buf[i] == '.' ) {
                    buf[i] = 0;
                    break;
                } else {
                    break;
                }
            }
            return buf;
        } else {
            return "???";
        }
    }
    
    inline void Term::BooleanTrue ()
    {
        number_ = 1.0;
        type_   = EBoolean;
    }
    
    inline void Term::BooleanFalse ()
    {
        number_ = 0.0;
        type_   = EBoolean;
    }
    
    inline void Term::IsNan (Term& a_term)
    {
        if ( (a_term.type_ & EErrorMask) == ENan || isnan(a_term.number_) ) {
            number_ = 0.0;
        } else {
            number_ = 1.0;
        }
        type_   = EBoolean;
    }
    
    inline void Term::SetNull ()
    {
        number_ = 0.0;
        type_   = ENullError;
    }
    
    inline bool Term::IsNull () const
    {
        return type_ == ENullError;
    }
    
    inline void Term::ParseBoolean (Term& a_term)
    {
        if ( a_term.type_ & EErrorMask ) {
            number_ = 0.0;
        } else if ( strcasecmp(a_term.ToString().c_str(), "true") == 0 ) {
            number_ = 1.0;
        } else {
            number_ = 0.0;
        }
        type_ = EBoolean;
    }
    
    inline void Term::IsEmpty (Term& a_string)
    {
        if ( ( a_string.type_ & EErrorMask ) == ENullError ) {
            number_ = 1.0f;
            type_   = EBoolean;
            return;
        }
        if ( a_string.type_ & EErrorMask ) {
            SetAndHandleError(a_string.type_);
            return;
        }
        if ( a_string.type_ != EText ) {
            SetAndHandleError(EInvalidType);
            return;
        }
        number_ = a_string.text_.length() == 0 ? 1.0 : 0.0;
        type_   = EBoolean;
    }
    
    inline void Term::Length (Term& a_string)
    {
        if ( a_string.type_ & EErrorMask ) {
            SetAndHandleError(a_string.type_);
            return;
        }
        if ( a_string.type_ != EText ) {
            SetAndHandleError(EInvalidType);
            return;
        }
        number_ = a_string.text_.length();
        type_   = ENumber;
    }
    
    inline void Term::ToUpperCase (Term& a_string)
    {
        if ( a_string.type_ & EErrorMask ) {
            SetAndHandleError(a_string.type_);
            return;
        }
        if ( a_string.type_ & EErrorMask ) {
            SetAndHandleError(a_string.type_);
            return;
        }
        if ( a_string.type_ != EText ) {
            SetAndHandleError(EInvalidType);
            return;
        }
#if defined(CASPER_NO_ICU)
        const char* src = a_string.text_.c_str();
        char* tmp = (char*) alloca(a_string.text_.size());
        for ( size_t i = 0; i < a_string.text_.size(); ++i) {
            tmp[i] = (char) toupper(src[i]);
        }
        text_ = tmp;
#else
        text_ = "";
        UnicodeString tmp = UnicodeString::fromUTF8(a_string.text_.c_str());
        UnicodeString tmp_2 = tmp.toUpper();
        tmp_2.toUTF8String(text_);
#endif
    }
    
    inline void Term::ToLowerCase (Term& a_string)
    {
        if ( a_string.type_ & EErrorMask ) {
            SetAndHandleError(a_string.type_);
            return;
        }
        if ( a_string.type_ != EText ) {
            SetAndHandleError(EInvalidType);
            return;
        }
#if defined(CASPER_NO_ICU)
        const char* src = a_string.text_.c_str();
        char* tmp = (char*) alloca(a_string.text_.size());
        for ( size_t i = 0; i < a_string.text_.size(); ++i) {
            tmp[i] = (char) tolower(src[i]);
        }
        text_ = tmp;
#else
        text_ = "";
        UnicodeString tmp = UnicodeString::fromUTF8(a_string.text_.c_str());
        UnicodeString tmp_2 = tmp.toLower();
        tmp_2.toUTF8String(text_);
#endif
    }
    
    inline void Term::Substring (Term& a_string, Term& a_beginIndex, Term& a_endIndex)
    {
        if ( a_string.type_ & EErrorMask ) {
            SetAndHandleError(a_string.type_);
            return;
        }
        if ( a_string.type_ != EText ) {
            SetAndHandleError(EInvalidType);
            return;
        }
        double begin = a_beginIndex.ToNumber();
        double end   = a_endIndex.ToNumber();
        if ( isnan(begin) || isnan(end) ) {
            SetAndHandleError(ENan);
            return;
        }
        if ( begin < 0.0 || end > a_string.text_.size() || begin > end  ) {
            SetAndHandleError(EIndexError);
            return;
        }
        text_ = a_string.text_.substr((int) begin, (int) (end - begin));
    }
    
    inline void Term::Substring (Term& a_string, Term& a_beginIndex)
    {
        if ( a_string.type_ & EErrorMask ) {
            SetAndHandleError(a_string.type_);
            return;
        }
        if ( a_string.type_ != EText ) {
            SetAndHandleError(EInvalidType);
            return;
        }
        double begin = a_beginIndex.ToNumber();
        if ( isnan(begin) ) {
            SetAndHandleError(ENan);
            return;
        }
        if ( begin < 0.0 || begin > a_string.text_.size()) {
            SetAndHandleError(EIndexError);
            return;
        }
        text_ = a_string.text_.substr((int) begin, std::string::npos);
    }
    
    inline void Term::Replace (Term& a_string, Term& a_from, Term& a_to)
    {
        if ( a_string.type_ & EErrorMask ) {
            SetAndHandleError(a_string.type_);
            return;
        }
        if ( a_string.type_ != EText ) {
            SetAndHandleError(EInvalidType);
            return;
        }
        if ( a_from.type_ & EErrorMask ) {
            SetAndHandleError(a_from.type_);
            return;
        }
        if ( a_from.type_ != EText ) {
            SetAndHandleError(EInvalidType);
            return;
        }
        if ( a_to.type_ & EErrorMask ) {
            SetAndHandleError(a_to.type_);
            return;
        }
        if ( a_to.type_ != EText ) {
            SetAndHandleError(EInvalidType);
            return;
        }
        text_ = a_string.text_;
        type_ = EText;
        
        size_t pos = 0;
        while ((pos = text_.find(a_from.text_, pos)) != std::string::npos) {
            text_.replace(pos, a_from.text_.length(), a_to.text_);
            pos += a_to.text_.length();
        }
    }

    inline void Term::IndexOf (Term& a_string, Term& a_substring)
    {
        if ( a_string.type_ & EErrorMask ) {
            SetAndHandleError(a_string.type_);
            return;
        }
        if ( a_string.type_ != EText ) {
            SetAndHandleError(EInvalidType);
            return;
        }
        if ( a_substring.type_ & EErrorMask ) {
            SetAndHandleError(a_substring.type_);
            return;
        }
        if ( a_substring.type_ != EText ) {
            SetAndHandleError(EInvalidType);
            return;
        }
        const char* sub = strstr(a_string.text_.c_str(), a_substring.text_.c_str());
        if ( sub == NULL ) {
            number_ = -1.0f;
        } else {
            number_= sub - a_string.text_.c_str();
        }
        type_ = ENumber;
    }

    inline void Term::ParseDouble (Term& a_term)
    {
        if ( a_term.type_ & EErrorMask ) {
            SetAndHandleError(a_term.type_);
            return;
        }
        number_ = a_term.ToNumber();
        if ( isnan(number_) ) {
            SetAndHandleError(ENan);
            return;
        } else {
            type_ = ENumber;
        }
    }
    
    inline void Term::DoubleToString (Term& a_number)
    {
        if ( a_number.type_ & EErrorMask ) {
            SetAndHandleError(a_number.type_);
            return;
        }
        if ( (a_number.type_ & (EBoolean | ENumber)) == 0 ) {
            SetAndHandleError(EInvalidType);
            return;
        }
        text_ = a_number.ToString();
        type_ = EText;
    }
    
    inline void Term::ParseInteger (Term& a_term)
    {
        if ( a_term.type_ & EErrorMask ) {
            SetAndHandleError(a_term.type_);
            return;
        }
        number_ = a_term.ToNumber();
        if ( isnan(number_) ) {
            SetAndHandleError(ENan);
            return;
        } else {
            number_ = (double)((int) number_);
            type_ = ENumber;
        }
    }
    
    inline void Term::ToI (Term& a_term)
    {
        if ( a_term.IsNull() ) {
            number_ = 0.0f;
            type_   = ENumber;
        } else {
            number_ = a_term.ToNumber();
            if ( isnan(number_) ) {
                SetAndHandleError(ENan);
                return;
            } else {
                number_ = (double)((int) number_);
                type_ = ENumber;
            }
        }
    }

    inline void Term::ToF (Term& a_term)
    {
        if ( a_term.IsNull() ) {
            number_ = 0.0f;
            type_   = ENumber;
        } else {
            number_ = a_term.ToNumber();
            if ( isnan(number_) ) {
                SetAndHandleError(ENan);
                return;
            } else {
                number_ = (double) number_;
                type_ = ENumber;
            }
        }
    }

    inline void Term::IntegerToString (Term& a_number)
    {
        if ( a_number.type_ & EErrorMask ) {
            SetAndHandleError(a_number.type_);
            return;
        }
        if ( (a_number.type_ & (EBoolean | ENumber)) == 0 ) {
            SetAndHandleError(EInvalidType);
            return;
        }
        number_ = (int) a_number.number_;
        type_   = ENumber;
        text_   = ToString();
        type_   = EText;
    }    
    
    inline void Term::AddNumOrText (Term& a_term1, Term& a_term2)
    {
        if ( a_term1.type_ & EErrorMask ) {
            SetAndHandleError(a_term1.type_);
            return;
        }
        if ( a_term2.type_ & EErrorMask ) {
            SetAndHandleError(a_term2.type_);
            return;
        }
        
        if ( EText == a_term1.type_ || EText == a_term2.type_ ) {
            text_  = a_term1.ToString();
            text_ += a_term2.ToString();
            type_  = EText;
            return;
        }
        
        double num1 = a_term1.ToNumber();
        double num2 = a_term2.ToNumber();
        if ( isnan(num1) == false && isnan(num2) == false ) {
            number_ = num1 + num2;
            type_   = ENumber;
        } else {
            text_  = a_term1.ToString();
            text_ += a_term2.ToString();
            type_  = EText;
        }
    }
    
    inline bool Term::IsBoolean () const
    {
        if ( type_ & EBoolean ) {
            return true;
        } else {
            return false;
        }
    }
    
    inline bool Term::ToBoolean () const
    {
        if ( type_ & EBoolean ) {
            return number_ != 0.0 ? true : false;
        } else {
            return false;
        }
    }
    
    inline  bool Term::HasError () const
    {
        return (type_ & EErrorMask) != 0;
    }
    
} // namespace casper

#endif // NRS_CASPER_CASPER_TERM_H
