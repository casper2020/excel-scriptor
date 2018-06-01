
/**
 * @file ast_node.cc
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

#include "casper/js_compiler/ast_node.h"

casper::js_compiler::AstNode::AstNode ()
{
    right_ = nullptr;
    left_  = nullptr;
    bool_  = false;
    pare_  = false;
    type_  = TUndef;
    num_   = 0;
    arg1_  = nullptr;
    arg2_  = nullptr;
}

casper::js_compiler::AstNode::AstNode (casper::js_compiler::AstNode::Type a_type, casper::js_compiler::AstNode* a_left, casper::js_compiler::AstNode* a_right)
{
    right_ = a_right;
    left_  = a_left;
    bool_  = false;
    pare_  = false;
    type_  = a_type;
    num_   = 0;
    arg1_  = nullptr;
    arg2_  = nullptr;
}

casper::js_compiler::AstNode::AstNode (Type a_type, const std::string& a_value)
{
    right_ = nullptr;
    left_  = nullptr;
    bool_  = false;
    pare_  = false;
    type_  = a_type;
    num_   = 0;
    arg1_  = nullptr;
    arg2_  = nullptr;
    text_  = a_value;
}

casper::js_compiler::AstNode::AstNode (const double& a_value)
{
    right_ = nullptr;
    left_  = nullptr;
    bool_  = false;
    pare_  = false;
    type_  = TNum;
    num_   = a_value;
    arg1_  = nullptr;
    arg2_  = nullptr;
}


casper::js_compiler::AstNode::AstNode (const std::string& a_value)
{
    right_ = nullptr;
    left_  = nullptr;
    bool_  = false;
    pare_  = false;
    type_  = TText;
    num_   = 0;
    arg1_  = nullptr;
    arg2_  = nullptr;
    text_  = a_value;
}


casper::js_compiler::AstNode::~AstNode ()
{
    // left_, right_, arg1_ and arg2_ - are managed by Ast class
}
