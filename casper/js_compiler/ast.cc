/**
 * @file ast.cc
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

#include <iostream>
#include <sstream>
#include "casper/js_compiler/ast.h"

#ifdef __APPLE__
#pragma mark -
#endif

casper::js_compiler::Ast::Ast ()
{
    root_ = nullptr;
}

casper::js_compiler::Ast::~Ast ()
{
    Reset();
}

#ifdef __APPLE__
#pragma mark -
#endif

casper::js_compiler::AstNode* casper::js_compiler::Ast::NewAstNode ()
{
    AstNode* node = new AstNode();
    
    allocated_nodes_.push_back(node);
    
    return node;
}


casper::js_compiler::AstNode* casper::js_compiler::Ast::NewAstNode (const casper::js_compiler::AstNode::Type a_type)
{
    AstNode* node = new AstNode(a_type);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::NewAstNode (double a_num)
{
    AstNode* node = new AstNode(a_num);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::NewAstNode (const std::string& a_text)
{
    AstNode* node = new AstNode(a_text);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::NewAstNode (const casper::js_compiler::AstNode::Type a_type, const std::string& a_text)
{
    AstNode* node = new AstNode(a_type, a_text);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::Expression (const std::string& a_op, casper::js_compiler::AstNode* a_left, casper::js_compiler::AstNode* a_right)
{
    AstNode* node = new AstNode(AstNode::TExpr, a_left, a_right);
    
    node->setOp(a_op);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::Operation (const std::string& a_op, casper::js_compiler::AstNode* a_left)
{
    AstNode* node = new AstNode(AstNode::TOps, a_left);
    
    node->setOp(a_op);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::Operation  (const std::string& a_op, casper::js_compiler::AstNode* a_left, casper::js_compiler::AstNode* a_right)
{
    AstNode* node = new AstNode(AstNode::TOps, a_left);
    
    //std::cout << a_left->getText() << " " << a_right->getText() << " " << a_op << " 1 \n";
    
    node->setOp(a_op);
    node->setArg1(a_right);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::Operation  (const std::string& a_op, casper::js_compiler::AstNode* a_left, casper::js_compiler::AstNode* a_right_1, casper::js_compiler::AstNode* a_right_2)
{
    AstNode* node = new AstNode(AstNode::TOps, a_left);
    
    node->setOp(a_op);
    node->setArg1(a_right_1);
    node->setArg2(a_right_2);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::Operation  (const std::string& a_op, casper::js_compiler::AstNode* a_left, casper::js_compiler::AstNode* a_right_1, casper::js_compiler::AstNode* a_right_2, casper::js_compiler::AstNode* a_right_3)
{
    AstNode* node = new AstNode(AstNode::TOps, a_left);
    
    node->setOp(a_op);
    node->setRight(a_right_3);
    node->setArg1(a_right_1);
    node->setArg2(a_right_2);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::StrOp (const std::string& a_op, casper::js_compiler::AstNode* a_left)
{
    AstNode* node = new AstNode(AstNode::TStrOps, a_left);
    
    node->setOp(a_op);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::StrOp (const std::string& a_op, casper::js_compiler::AstNode* a_left, casper::js_compiler::AstNode* a_right)
{
    AstNode* node = new AstNode(AstNode::TStrOps, a_left);
    
    node->setOp(a_op);
    node->setArg1(a_right);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::StrOp (const std::string& a_op, casper::js_compiler::AstNode* a_left, casper::js_compiler::AstNode* a_right_1, casper::js_compiler::AstNode* a_right_2)
{
    AstNode* node = new AstNode(AstNode::TStrOps, a_left);
    
    node->setOp(a_op);
    node->setArg1(a_right_1);
    node->setArg2(a_right_2);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::If (casper::js_compiler::AstNode* a_left, casper::js_compiler::AstNode* a_right_1, casper::js_compiler::AstNode* a_right_2)
{
    AstNode* node = new AstNode(AstNode::TIf, a_left);
    
    node->setOp("if");
    node->setArg1(a_right_1);
    node->setArg2(a_right_2);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::Bool(bool a_bool)
{
    AstNode* node = new AstNode(AstNode::TBool);
    node->setBool(a_bool);
    node->setVal(true == a_bool ? 1 : 0);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::DateOp(const std::string& a_op, casper::js_compiler::AstNode* a_left, casper::js_compiler::AstNode* a_right_1, casper::js_compiler::AstNode* a_right_2)
{
    AstNode* node = new AstNode(AstNode::TDate);
    
    node->setOp(a_op);
    node->setLeft(a_left);
    node->setArg1(a_right_1);
    node->setArg2(a_right_2);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::DateOp(const std::string& a_op, casper::js_compiler::AstNode* a_right_1, casper::js_compiler::AstNode* a_right_2)
{
    AstNode* node = new AstNode(AstNode::TDate);
    
    node->setOp(a_op);
    node->setArg1(a_right_1);
    node->setArg2(a_right_2);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::DateOp(const std::string& a_op, casper::js_compiler::AstNode* a_left)
{
    AstNode* node = new AstNode(AstNode::TDate);
    
    node->setOp(a_op);
    node->setLeft(a_left);
    
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::DateOp(const std::string& a_op)
{
    AstNode* node = new AstNode(AstNode::TDate);
    
    node->setOp(a_op);
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::AddVar(casper::js_compiler::AstNode* a_varname, casper::js_compiler::AstNode* a_result)
{
    AstNode* node = new AstNode(AstNode::TVar);
    
    node->setOp("AddVar");
    node->setLeft(a_varname);
    node->setRight(a_result);
    allocated_nodes_.push_back(node);
    
    return node;
}

casper::js_compiler::AstNode* casper::js_compiler::Ast::GetVar(casper::js_compiler::AstNode* a_varname)
{
    AstNode* node = new AstNode(AstNode::TVar);
    
    node->setOp("GetVar");
    node->setLeft(a_varname);
    allocated_nodes_.push_back(node);
    
    return node;
}

