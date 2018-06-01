
/**
 * @file ast_node.h
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

#ifndef CASPER_JAVA_AST_NODE_H_
#define CASPER_JAVA_AST_NODE_H_

#include <stdio.h>

#include <string>

namespace casper
{
    
    namespace js_compiler
    {
        
        class JsParser;
        
        class AstNode
        {
            
            friend class JsParser;
            
        public: // enumerations
            
            enum Type : uint8_t {
                TUndef  = 0,
                TNum    = 1,
                TText   = 2,
                TExpr   = 3,
                TOps    = 4,
                TStrOps = 5,
                TIf     = 6,
                TBool   = 7,
                TVar    = 8,
                TParam  = 9,
                TField  = 10,
                TNull   = 11,
                TDate   = 12
            };
            
        protected:
            
            AstNode*      right_; // managed by Ast class
            AstNode*      left_;  // managed by Ast class
            bool          bool_;
            bool          pare_;
            Type          type_;
            double        num_;
            std::string   text_;
            std::string   op_;
            AstNode*      arg1_; // managed by Ast class
            AstNode*      arg2_; // managed by Ast class
            
        public: // Constructor(s) / Destructor
            
            AstNode ();
            AstNode (Type a_type, AstNode* a_left = nullptr, AstNode* a_right = nullptr);
            AstNode (Type a_type, const std::string& a_value);
            AstNode (const double& a_value);
            AstNode (const std::string& a_value);
            
            AstNode (const AstNode &obj) = delete;
            
            virtual ~AstNode ();
            
        public:
            
            //setters
            void        setRight(AstNode* t_right);
            void        setLeft(AstNode* t_left);
            void        setOp(std::string t_op);
            void        setVal(double t_val);
            void        setPare(bool a_pare);
            void        setText(std::string t_text);
            void        setArg1(AstNode* t_arg1);
            void        setArg2(AstNode* t_arg2);
            
            //getters
            AstNode*    getRight();
            AstNode*    getLeft();
            std::string getOp();
            double      getVal();
            bool        getPare();
            std::string getText();
            AstNode*    getArg1();
            AstNode*    getArg2();
            
            bool  getBool  ()              { return bool_; };
            void  setBool  (bool t_bool)   { bool_ = t_bool; };
            Type   getType ()              { return type_; };
            void   setType (Type a_type)   { type_ = a_type; };
            
            void          operator =(const char* const a_string);
            void          operator =(const std::string& a_string);
            void          operator =(double a_value);
            void          operator =(bool t_bool);
            
        public: // Operator(s)
            
            AstNode& operator =(const AstNode &obj) = delete;
            
        };
        
        inline void AstNode::operator = (const char* const a_string) {
            text_ = a_string;
            type_ = TText;
        }
        
        inline void AstNode::operator =(const std::string& a_string) {
            text_ = a_string;
            type_ = TText;
        }
        
        inline void AstNode::operator =(bool t_bool) {
            if(t_bool) num_=1;
            else num_=0;
            bool_ = t_bool;
            type_ = TBool;
        }
        
        inline void AstNode::operator =(double a_value) {
            num_    = a_value;
            type_   = TNum;
        }
        
        inline void AstNode::setRight(AstNode* t_right)
        {
            right_=t_right;
        }
        
        inline void AstNode::setLeft(AstNode* t_left)
        {
            left_=t_left;
        }
        
        inline void AstNode::setOp(std::string t_op)
        {
            op_=t_op;
        }
        
        inline void AstNode::setVal(double t_val)
        {
            num_=t_val;
        }
        
        inline void AstNode::setPare(bool a_pare)
        {
            pare_=a_pare;
        }
        
        inline void AstNode::setText(std::string t_text)
        {
            text_=t_text;
        }
        
        inline void AstNode::setArg1(AstNode* t_arg1)
        {
            arg1_=t_arg1;
        }
        
        inline void AstNode::setArg2(AstNode* t_arg2)
        {
            arg2_=t_arg2;
        }
        
        inline AstNode* AstNode::getRight ()
        {
            return right_;
        }
        
        inline AstNode* AstNode::getLeft ()
        {
            return left_;
        }
        
        inline std::string AstNode::getOp (){
            return op_;
        }
        
        inline double AstNode::getVal ()
        {
            return num_;
        }
        
        inline bool AstNode::getPare ()
        {
            return pare_;
        }
        
        inline std::string AstNode::getText ()
        {
            return text_;
        }
        
        inline AstNode* AstNode::getArg1 ()
        {
            return arg1_;
        }
        
        inline AstNode* AstNode::getArg2 ()
        {
            return arg2_;
        }
    }
}

#endif // CASPER_JAVA_AST_NODE_H_
