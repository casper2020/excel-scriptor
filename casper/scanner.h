/**
 * @file lexer.h declaration of the expression gramar tokenizer / scanner
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
#ifndef NRS_CASPER_CASPER_SCANNER_H
#define NRS_CASPER_CASPER_SCANNER_H

#include <stdint.h>
#include <sys/types.h> // size_t

namespace casper
{
    /**
     * @brief Base class for lexical parsers build with ragel
     */
    class Scanner
    {
    protected:
        
        static const uint8_t k_max_fractional_digits_;
        
    protected: // Attributes
        
        char const* input_;
        
        /*
         * Interface to ragel scanner
         */
        char const* p_;
        char const* pe_;
        char const* eof_;
        int         cs_;
        char const* ts_;
        char const* te_;
        int         act_;
        
        /*
         * Data model for number scanner
         */
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
        
    public: // methods
        
        Scanner ();
        virtual ~Scanner();
        
    protected: // methods
        
        virtual void SetInput (const char* a_expression, size_t a_lenght);
        
    public:
        
        char const* GetInput () const;
        
    };
    
    inline char const* Scanner::GetInput () const
    {
        return input_;
    }
    
} // namespace casper

#endif // NRS_CASPER_CASPER_SCANNER_H
