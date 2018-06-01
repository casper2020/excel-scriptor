/**
 * @file fake_java_expression.cc
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

#include "casper/js_compiler/interpreter.h"

int criteria_cnt = 0;


casper::js_compiler::Interpreter::Interpreter ()
: jscanner_(ast_),
parser_(ast_, jscanner_, *this)
{
    /* empty */
}

casper::js_compiler::Interpreter::~Interpreter ()
{
    /* empty */
}


const std::string& casper::js_compiler::Interpreter::ConvertSum (const std::string& a_expression, std::set<std::string> a_precs)
{
    return ConvertSum(a_expression.c_str(), a_expression.size(), a_precs);
}

const std::string& casper::js_compiler::Interpreter::ConvertSum (const char* a_expression, size_t a_len, std::set<std::string> a_precs)
{
    tmp_ss_.str("");
    tmp_expression_ = "";
    try {
        ast_.Reset();
        jscanner_.SetInput(a_expression, a_len);
        parser_.parse();

        BuildStringSum(ast_.root_, a_precs);
        ast_.sum_criterias_.clear();
        tmp_expression_ = tmp_ss_.str();
        ast_.Reset();
    } catch (const std::runtime_error& a_error) {
        ast_.Reset();
        throw a_error;
    }
    return tmp_expression_;
}

void casper::js_compiler::Interpreter::BuildStringSum (casper::js_compiler::AstNode* a_node, std::set<std::string> a_precs)
{
    if ( nullptr == a_node ) {
        return;
    }

    else {
        std::stringstream varname_ss;

        varname_ss << "f_";
        if(nullptr != a_node->getArg1()){
            varname_ss  << a_node->getLeft()->getText() << "_" << a_node->getArg1()->getText();
        }
        else {
            varname_ss  << a_node->getLeft()->getLeft()->getText() << "_";
            varname_ss  << a_node->getLeft()->getArg1()->getText();
        }

        std::string varname = varname_ss.str();

        var_names_.push_back(varname);

        tmp_ss_ << varname << "=0;";
        for(auto prec : a_precs) {
            tmp_ss_ << " " <<varname << "="  << varname << " + " << prec << ".x;";
        }
    }
}

const std::string& casper::js_compiler::Interpreter::ConvertSumIfs (const std::string& a_expression, std::set<std::string> a_precs)
{
    return ConvertSumIfs(a_expression.c_str(), a_expression.size(), a_precs);
}

const std::string& casper::js_compiler::Interpreter::ConvertSumIfs (const char* a_expression, size_t a_len, std::set<std::string> a_precs)
{
    tmp_ss_.str("");
    tmp_expression_ = "";
    try {
        ast_.Reset();
        jscanner_.SetInput(a_expression, a_len);
        parser_.parse();
        BuildStringSumIfs(ast_.root_, a_precs);
        ast_.sum_criterias_.clear();
        tmp_expression_ = tmp_ss_.str();
        ast_.Reset();
    } catch (const std::runtime_error& a_error) {
        ast_.Reset();
        throw a_error;
    }
    return tmp_expression_;
}

void casper::js_compiler::Interpreter::BuildStringSumIfs (casper::js_compiler::AstNode* a_node, std::set<std::string> a_precs)
{
    if ( nullptr == a_node ) {
        return;
    }

    else {

        std::stringstream varname_ss;

        if(nullptr != a_node->getArg1()){
            varname_ss  << a_node->getLeft()->getText() << "_" << a_node->getArg1()->getText();
        }
        else {
            varname_ss  << a_node->getLeft()->getLeft()->getText() << "_";
            varname_ss  << a_node->getLeft()->getArg1()->getText();
        }

        std::string varname = varname_ss.str();

        var_names_.push_back(varname);

        tmp_ss_ << varname << " = [";
        for(auto prec : a_precs) {
            tmp_ss_ << "\'" << prec << "\',";
        }
        tmp_ss_.seekp(-1,tmp_ss_.cur); tmp_ss_ << ']';

    }
}

const std::string& casper::js_compiler::Interpreter::Convert (const std::string& a_expression)
{
    return Convert(a_expression.c_str(), a_expression.size());
}

const std::string& casper::js_compiler::Interpreter::Convert (const char* a_expression, size_t a_len)
{
    tmp_ss_.str("");
    tmp_expression_ = "";

    try {
        ast_.Reset();
        jscanner_.SetInput(a_expression, a_len);
        parser_.parse();
        if(!ast_.sum_criterias_.empty()){
            tmp_ss_ << "var criteria" << criteria_cnt << " = {};\n";
            for (auto const& x : ast_.sum_criterias_) {
                tmp_ss_ << "criteria" << criteria_cnt << "[";
                tmp_ss_ << "\'" << x.first->getArg1()->getText() << "\'";
                tmp_ss_ << "] = ";
                BuildString(x.second);
                tmp_ss_ << ";\n";
            }
            tmp_ss_ << "\n";
            BuildString(ast_.root_);
            criteria_cnt++;
        }
        else BuildString(ast_.root_);
        ast_.sum_criterias_.clear();
        tmp_expression_ = tmp_ss_.str();
        ast_.Reset();
    } catch (const std::runtime_error& a_error) {
        ast_.Reset();
        throw a_error;
    }

    return tmp_expression_;
}



void casper::js_compiler::Interpreter::BuildString (casper::js_compiler::AstNode* a_node)
{

    if ( nullptr == a_node ) {
        return;
    }

    if ( a_node->getType()==casper::js_compiler::AstNode::TNum ) {

        //
        // Numbers
        //

        tmp_ss_ << a_node->getVal();

    } else if ( a_node->getType()==casper::js_compiler::AstNode::TText ) {

        //
        // Text
        //

        tmp_ss_  << "(\"";
        std::string node_text = a_node->getText();
        for(int i=0; i<node_text.size(); i++){
            if(node_text[i]=='\n') tmp_ss_ << "\\\n";
            else tmp_ss_ << node_text[i];
        }
        tmp_ss_  << "\")";

    } else if ( a_node->getType()==casper::js_compiler::AstNode::TBool ) {

        //
        // Bool
        //
        tmp_ss_ << ( a_node->getBool() ? "true" : "false" );

    }else if ( a_node->getType()==casper::js_compiler::AstNode::TExpr ) {

        //
        // Expressions
        //

        if ( ! a_node->getOp().compare("!") ) {
            tmp_ss_  << " !";
            BuildString(a_node->getLeft());
        } else if ( ! a_node->getOp().compare("UM") ) {
            tmp_ss_  << " -";
            BuildString(a_node->getLeft());
        } else if ( ! a_node->getOp().compare("&") ) {
            tmp_ss_  << "(";
            tmp_ss_  << "\'\' + ";
            BuildString(a_node->getLeft());
            tmp_ss_  << " + ";
            BuildString(a_node->getRight());
            tmp_ss_  << ")";
        } else if ( ! a_node->getOp().compare("MIN") ) {
            tmp_ss_  << "Math.min(";
            BuildString(a_node->getLeft());
            tmp_ss_  << ",";
            BuildString(a_node->getRight());
            tmp_ss_  << ")";
        } else if ( ! a_node->getOp().compare("MAX") ) {
            tmp_ss_  << "Math.max(";
            BuildString(a_node->getLeft());
            tmp_ss_  << ",";
            BuildString(a_node->getRight());
            tmp_ss_  << ")";
        } else if ( ! a_node->getOp().compare("RND") ) {
            tmp_ss_  << "round(";
            BuildString(a_node->getLeft());
            tmp_ss_ << ",";
            BuildString(a_node->getRight());
            tmp_ss_  << ")";
        } else if ( ! a_node->getOp().compare("RNDUP") ) {
            tmp_ss_  << "ceil(";
            BuildString(a_node->getLeft());
            tmp_ss_ << ",";
            BuildString(a_node->getRight());
            tmp_ss_  << ")";
        } else if ( ! a_node->getOp().compare("RNDDN") ) {
            tmp_ss_  << "floor(";
            BuildString(a_node->getLeft());
            tmp_ss_ << ",";
            BuildString(a_node->getRight());
            tmp_ss_  << ")";
        } else if ( ! a_node->getOp().compare("PERCENT") ) {
            tmp_ss_  << "(";
            BuildString(a_node->getLeft());
            tmp_ss_  << "/100)";
        } else {
            if ( a_node->getPare() ) {
                tmp_ss_  << "(";
                BuildString(a_node->getLeft());
                tmp_ss_  << " " << a_node->getOp() << " ";
                BuildString(a_node->getRight());
                tmp_ss_  << ")";
            } else {
                BuildString(a_node->getLeft());
                tmp_ss_  << " " << a_node->getOp() << " ";
                BuildString(a_node->getRight());
            }
        }

    } else if ( a_node->getType()==casper::js_compiler::AstNode::TOps ) {

        //
        // Operations
        //

        if ( ! a_node->getOp().compare("Lookup") ) {
            if(! a_node->getArg1()->getOp().compare("&")){
                tmp_ss_ << "lookup(\'INDIRECT" ;
                tmp_ss_ << "\',";
                BuildString(a_node->getLeft());
                tmp_ss_ << ",";
                BuildString(a_node->getArg1());
            } else {
                tmp_ss_ << "lookup(\'" << a_node->getArg1()->getLeft()->getText();
                tmp_ss_ << "\',";
                BuildString(a_node->getLeft());
                if(nullptr != a_node->getArg1()->getArg1()){
                    tmp_ss_ << ",\'" << a_node->getArg1()->getArg1()->getText();
                } else tmp_ss_ << ",\'??";
            }
            if(! a_node->getArg2()->getOp().compare("&")){
                tmp_ss_ << ",";
                BuildString(a_node->getArg2());
            } else {
                if(nullptr != a_node->getArg2()->getArg1()){
                    tmp_ss_ << "\',\'" << a_node->getArg2()->getArg1()->getText();
                } else tmp_ss_ << "\',\'??";
                tmp_ss_ << "\'";
            }
            tmp_ss_ << ")";
        } else if ( ! a_node->getOp().compare("VLookup") ) {
            tmp_ss_ << "vlookup(" << a_node->getArg1()->getLeft()->getText();
            tmp_ss_ << ",";
            BuildString(a_node->getLeft());
            tmp_ss_ << ",\'";
            tmp_ss_ << a_node->getArg1()->getArg1()->getText();
            tmp_ss_ << "\',";
            BuildString(a_node->getArg2());
            if(nullptr != a_node->getRight()){
                tmp_ss_ << ",";
                BuildString(a_node->getRight());
            } else {
                tmp_ss_ << ",0";
            }
            tmp_ss_ << ")";
        } else if ( ! a_node->getOp().compare("SUM1") ) {
            tmp_ss_  << "f_" <<a_node->getLeft()->getLeft()->getText() << "_";
            tmp_ss_  << a_node->getLeft()->getArg1()->getText();
        } else if ( ! a_node->getOp().compare("SUM2") ) {
            std::string begin = a_node->getLeft()->getText();
            std::string end = a_node->getArg1()->getText();
            tmp_ss_  << "f_" << begin << "_" << end;
        } else if ( ! a_node->getOp().compare("SUMIF") ) {
            tmp_ss_ << "sumif(\'NOT_LINES\'," << a_node->getLeft()->getLeft()->getText()<<",\'";
            tmp_ss_ << a_node->getLeft()->getArg1()->getText() << "\',\'";
            tmp_ss_ << a_node->getArg1()->getArg1()->getText() << "\',\'";
            tmp_ss_ << a_node->getArg1()->getLeft()->getText() << "\')";
        } else if ( ! a_node->getOp().compare("SUMIFS") ) {

            if( ! a_node->getLeft()->getLeft()->getText().compare("LINES") ){
                tmp_ss_ << "sumifs(\'LINES\',LINES_"<< a_node->getLeft()->getArg1()->getText();
                tmp_ss_ << ",\'" << a_node->getLeft()->getArg1()->getText();
                tmp_ss_ << "\',criteria" << criteria_cnt << ")";
            } else {
                tmp_ss_ << "sumifs(\'NOTLINES\'," << a_node->getLeft()->getLeft()->getText();
                tmp_ss_ << ",\'" << a_node->getLeft()->getArg1()->getText();
                tmp_ss_ << "\',criteria" << criteria_cnt << ")";
            }
        } else if ( ! a_node->getOp().compare("MATCH") ) {
            tmp_ss_  << "match(" << a_node->getArg1()->getText();
            tmp_ss_  << ",\'" << a_node->getLeft()->getText();
            tmp_ss_  << "\')+1";
        } else if ( ! a_node->getOp().compare("Vector") ) {
            if(nullptr != a_node->getArg2()){
                tmp_ss_ << a_node->getLeft()->getText() << "[";
                tmp_ss_ << a_node->getArg1()->getText() << " , ";
                tmp_ss_ << a_node->getArg2()->getText() << "]";
            } else{
                tmp_ss_ << a_node->getLeft()->getText() << "[";
                tmp_ss_ << a_node->getArg1()->getText() << "]";
            }
        } else if ( ! a_node->getOp().compare("isError") ) {
            tmp_ss_ << "(isError(";
            BuildString(a_node->getLeft());
            tmp_ss_ << ") ? true : false)";
        } else if ( ! a_node->getOp().compare("ifError") ) {
            tmp_ss_ << "(isError(";
            BuildString(a_node->getLeft());
            tmp_ss_ << ") ? (";
            BuildString(a_node->getArg1());
            tmp_ss_ << ") : (";
            BuildString(a_node->getLeft());
            tmp_ss_ << "))";
        } else if ( ! a_node->getOp().compare("OFFSET") ) {
            tmp_ss_ << " Left -> " << a_node->getLeft()->getText();
            tmp_ss_ << " Arg1 -> " << a_node->getArg1()->getVal();
            tmp_ss_ << " Arg2 -> " << a_node->getArg2()->getVal();
        } else if ( ! a_node->getOp().compare("LinesRef") ) {
            tmp_ss_ << "lines(" << a_node->getLeft()->getText();
            tmp_ss_ << ".y,\'" << a_node->getArg1()->getText() << "\')";
        } else {
            tmp_ss_ << a_node->getOp() << "(";
            BuildString(a_node->getLeft());
            tmp_ss_   << ")";
        }

    } else if ( a_node->getType()==casper::js_compiler::AstNode::TStrOps ) {

        //
        // String Ops
        //

        if ( ! a_node->getOp().compare("LFT") ) {
            BuildString(a_node->getLeft());
            tmp_ss_  << ".substring(0 , ";
            BuildString(a_node->getArg1());
            tmp_ss_  << ")";
        } else if ( ! a_node->getOp().compare("RGT") ) {
            BuildString(a_node->getLeft());
            tmp_ss_  << ".substring(";
            BuildString(a_node->getLeft());
            tmp_ss_  << ".length-";
            BuildString(a_node->getArg1());
            tmp_ss_  << " , ";
            BuildString(a_node->getLeft());
            tmp_ss_  << ".length";
            tmp_ss_  << ")";
        } else if ( ! a_node->getOp().compare("MID") ) {
            BuildString(a_node->getLeft());
            tmp_ss_  << ".substring(";
            BuildString(a_node->getArg1());
            tmp_ss_  << "-1 , ";
            BuildString(a_node->getArg1());
            tmp_ss_  << " + ";
            BuildString(a_node->getArg2());
            tmp_ss_  << ")";
        } else if ( ! a_node->getOp().compare("indexOf") ) {
            if ( a_node->getArg2() == nullptr ) {
                tmp_ss_  << "find(";
                BuildString(a_node->getLeft());
                tmp_ss_ << ",";
                BuildString(a_node->getArg1());
                tmp_ss_ << ",1)";
            }else{
                tmp_ss_  << "find(";
                BuildString(a_node->getLeft());
                tmp_ss_ << ",";
                BuildString(a_node->getArg1());
                tmp_ss_ << ",";
                BuildString(a_node->getArg2());
                tmp_ss_ << ")";
            }
        } else tmp_ss_ << "error";

    } else if ( a_node->getType()==casper::js_compiler::AstNode::TIf ) {

        //
        //Ifs
        //
        if (a_node->getArg2() == nullptr){
            if (a_node->getArg1() == nullptr){
                BuildString(a_node->getLeft());
            }
            else {
                tmp_ss_ << "(";
                BuildString(a_node->getLeft());
                tmp_ss_ << " ? ";
                BuildString(a_node->getArg1());
                tmp_ss_ << " : false";
                tmp_ss_ << ")";
            }
        }
        else {
            tmp_ss_ << "(";
            BuildString(a_node->getLeft());
            tmp_ss_ << " ? ";
            BuildString(a_node->getArg1());
            tmp_ss_ << " : ";
            BuildString(a_node->getArg2());
            tmp_ss_ << ")";
        }

    } else if ( a_node->getType()==casper::js_compiler::AstNode::TVar ) {

        //
        // Vars
        //

        if ( ! a_node->getOp().compare("AddVar") ) {
            tmp_ss_ << a_node->getLeft()->getText() << ".x = ";
            BuildString(a_node->getRight());
        }else if (! a_node->getOp().compare("GetVar")) {
            tmp_ss_ << a_node->getLeft()->getText() << ".x";
        }

    } else if ( a_node->getType()==casper::js_compiler::AstNode::TNull ) {

        //
        // Null
        //

        tmp_ss_ << "null";

    } else if ( a_node->getType()==casper::js_compiler::AstNode::TDate ) {

        //
        // Date
        //

        if(a_node->getOp().compare("newDate") == 0){
            tmp_ss_ << "new Date";
        }else if(a_node->getOp().compare("ND") == 0){
            tmp_ss_ << "newDate(";
            BuildString(a_node->getLeft());
            tmp_ss_ << " , ";
            BuildString(a_node->getArg1());
            tmp_ss_ << ", ";
            BuildString(a_node->getArg2());
            tmp_ss_ << ")";
        }else if(a_node->getOp().compare("ND1") == 0){
            tmp_ss_ << "toDate(";
            BuildString(a_node->getLeft());
            tmp_ss_ << ")";
        }else {
            tmp_ss_ << a_node->getOp() << "(";
            BuildString(a_node->getLeft());
            tmp_ss_ << ")";
        }
    }
}
