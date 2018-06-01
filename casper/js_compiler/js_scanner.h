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
#ifndef NRS_CASPER_CASPER_JS_SCANNER_H
#define NRS_CASPER_CASPER_JS_SCANNER_H

#include "casper/scanner.h"
#include "casper/js_compiler/js_parser.hh"
#include "casper/js_compiler/location.hh"

#include <stdint.h>

namespace casper
{
    namespace js_compiler
    {
        /**
         * @brief Base class for lexical parsers build with ragel
         */
        class JsScanner : public casper::Scanner
        {
            friend class JsParser;
            
        private: // Data
            
            Ast& ast_;
            
        public: // Methods
            
            JsScanner (Ast& a_ast);
            virtual ~JsScanner ();
            
            JsParser::token_type Scan (JsParser::semantic_type*, casper::js_compiler::location* a_location);
            
            virtual void SetInput (const char* a_expression, size_t a_lenght);
            
        protected: // Attributes
            
            /*
             * Data model for cell reference scanner
             */
            int row_;
            int col_;
            
        };
        
    } // namespace js_compiler
    
} // namespace casper

#endif // NRS_CASPER_CASPER_SEE_SCANNER_H

