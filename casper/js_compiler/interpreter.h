/**
 * @file fake_java_expression.h
 *
 * Copyright (c) 2011-2018 Cloudware S.A. All rights reserved.
 *
 * This file is part of jayscriptor.
 *
 * jayscriptor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jayscriptor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with jayscriptor.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef NRS_CASPER_CASPER_JAVA_FAKE_JAVA_EXPRESSION_H_
#define NRS_CASPER_CASPER_JAVA_FAKE_JAVA_EXPRESSION_H_

#include "casper/js_compiler/js_scanner.h"
#include "casper/js_compiler/js_parser.hh"
#include "casper/js_compiler/ast.h"
#include <map>
#include <set>
#include <sstream>

namespace casper
{
    
    namespace js_compiler
    {
        
        /**
         * @brief Expression evaluator for Excel expressions
         */
        class Interpreter
        {
            
            friend class JsParser;
            
        public:
            
            Ast         ast_;     //!<
            JsScanner   jscanner_; //!< Scanner for Excel expressions
            JsParser    parser_;  //!< Expression parser for Excel expressions
            std::vector<std::string> var_names_;
            
        private: // Data
            
            std::stringstream tmp_ss_;
            std::string       tmp_expression_;
            
        public: // Methods
            
            Interpreter ();
            virtual ~Interpreter ();
            
            const std::string& Convert (const std::string& a_expression);
            const std::string& Convert (const char* a_expression, size_t a_len);
            
            const std::string& ConvertSum (const std::string& a_expression, std::set<std::string> a_precs);
            const std::string& ConvertSum (const char* a_expression, size_t a_len, std::set<std::string> a_precs);
            
            const std::string& ConvertSumIfs (const std::string& a_expression, std::set<std::string> a_precs);
            const std::string& ConvertSumIfs (const char* a_expression, size_t a_len, std::set<std::string> a_precs);
            
        protected: // Method(s) / Function(s)
            
            void BuildStringSum (casper::js_compiler::AstNode* a_node, std::set<std::string> a_precs);
            void BuildString (casper::js_compiler::AstNode* a_node);
            void BuildStringSumIfs (casper::js_compiler::AstNode* a_node, std::set<std::string> a_precs);
            
        }; // end of class 'Interpreter'
        
    } // end of namespace 'js_compiler'
    
} // end of namespace 'casper'

#endif

