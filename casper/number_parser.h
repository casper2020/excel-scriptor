/**
 * @file number_parser.h - Generic number parser using RAGEL.
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
#ifndef NRS_CASPER_CASPER_NUMBER_PARSER_H_
#define NRS_CASPER_CASPER_NUMBER_PARSER_H_

#include <string>
#include <stdint.h>
#include <math.h> // NAN

namespace casper {

    class NumberParser
    {
        
    public: // Static Data
        
        static const uint8_t k_max_fractional_digits_;
        
    private: // Data
        
        double number_;
        
    private: // Parser Data
        
        int         cs_;
        char const* ts_;
        char const* te_;
        int         act_;
        
        uint64_t integer_;
        uint64_t fractional_;
        uint8_t  fractional_digits_cnt_;
        uint64_t divider_;
        uint32_t div_log10_;
        uint64_t exponent_;
        int64_t  integer_value_;
        double   double_value_;
        bool     negative_exponent_;
        bool     negative_;        
        
    public: // Constructor(s) / Destructor
        
        NumberParser ();
        virtual ~NumberParser ();
        
    public: // Method(s) / Function(s)
        
        double ParseDouble (const std::string& a_number);
        float  ParseFloat  (const std::string& a_number);
        int    ParseInt    (const std::string& a_number);

    protected: // Method(s) / Function(s)
        
        void Parse         (const std::string& a_number);

    public: // Accessors
        
        double DoubleValue ();
        float  FloatValue  ();
        int    IntValue    ();
        bool   IsNAN       ();
        
    public: // Static Method(s) / Function(s)
        
        static bool ParseDouble (const std::string& a_value, double& o_value);
        static bool ParseFloat  (const std::string& a_value, float& o_value);
        static bool ParseInt    (const std::string& a_value, int& o_value);

    }; // end of class NumberParser
    
    /**
     * @return
     *         @li The number value as double.
     *         @li NAN if it's not a number
     */
    inline double NumberParser::DoubleValue ()
    {
        return number_;
    }
    
    /**
     * @return 
     *         @li The number value as float.
     *         @li NAN if it's not a number
     */
    inline float NumberParser::FloatValue ()
    {
        return static_cast<float>(number_);
    }
    
    /**
     * @return
     *         @li The number value as int.
     *         @li NAN if it's not a number
     */
    inline int NumberParser::IntValue ()
    {
        return static_cast<int>(number_);
    }
    
    /*
     * @brief Check if the parsed number is a number or not.
     *
     * @return
     *        @li True if we've a valid number
     *        @li False if we don't have a valid number.
     */
    inline bool NumberParser::IsNAN ()
    {
        return isnan(number_);
    }
    
} // end of namespace casper

#endif // NRS_CASPER_CASPER_NUMBER_PARSER_H_
