/**
 * @file js_scanner.rl Implementation of expression tokenizer aka scanner
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

#include "osal/osalite.h"
#include "osal/utils/pow10.h"
#include "casper/js_compiler/js_scanner.h"
#include "casper/js_compiler/ast.h"
#include "casper/js_compiler/ast_node.h"

%%{
    machine jrxml_js_scanner;
    include generic_number_parser "../../osal/ragelib/generic_number_parser.rlh";
    alphtype char;
    write data;

    ws = [ \t\n];

    variable_name = [a-zA-Z_][a-zA-Z0-9_]+;

    escaped_quote = ('""' %{ has_quotes = true; });

    not_quote     = [^\"];

    text_literal = '"' ( escaped_quote | not_quote )* '"';

    main := |*

        #
        # Keywords
        #
        'ABS'         => { ret = JsParser::token::ABS         ; fbreak; };
        'MAX'         => { ret = JsParser::token::MAX         ; fbreak; };
        'MIN'         => { ret = JsParser::token::MIN         ; fbreak; };
        'IF'          => { ret = JsParser::token::IF          ; fbreak; };
        'LOOKUP'      => { ret = JsParser::token::LOOKUP      ; fbreak; };
        'VLOOKUP'     => { ret = JsParser::token::VLOOKUP     ; fbreak; };
        'YEAR'        => { ret = JsParser::token::YEAR        ; fbreak; };
        'DATE'        => { ret = JsParser::token::DATE        ; fbreak; };
        'ROUND'       => { ret = JsParser::token::ROUND       ; fbreak; };
        'ROUNDDOWN'   => { ret = JsParser::token::ROUNDDOWN   ; fbreak; };
        'ROUNDUP'     => { ret = JsParser::token::ROUNDUP     ; fbreak; };
        'DATE'        => { ret = JsParser::token::DATE        ; fbreak; };
        'YEAR'        => { ret = JsParser::token::YEAR        ; fbreak; };
        'MONTH'       => { ret = JsParser::token::MONTH       ; fbreak; };
        'DAY'         => { ret = JsParser::token::DAY         ; fbreak; };
        'HOUR'        => { ret = JsParser::token::HOUR        ; fbreak; };
        'MINUTE'      => { ret = JsParser::token::MINUTE      ; fbreak; };
        'SECOND'      => { ret = JsParser::token::SECOND      ; fbreak; };
        'SUM'         => { ret = JsParser::token::SUM         ; fbreak; };
        'INDIRECT'    => { ret = JsParser::token::INDIRECT    ; fbreak; };
        'ISERROR'     => { ret = JsParser::token::ISERROR     ; fbreak; };
        'SUMIFS'      => { ret = JsParser::token::SUMIFS      ; fbreak; };
        'SUMIF'       => { ret = JsParser::token::SUMIF       ; fbreak; };
        'FIND'        => { ret = JsParser::token::FIND        ; fbreak; };
        'IFERROR'     => { ret = JsParser::token::IFERROR     ; fbreak; };
        'AND'         => { ret = JsParser::token::AND         ; fbreak; };
        'OR'          => { ret = JsParser::token::OR          ; fbreak; };
        'RIGHT'       => { ret = JsParser::token::RIGHT       ; fbreak; };
        'LEFT'        => { ret = JsParser::token::LEFT        ; fbreak; };
        'MID'         => { ret = JsParser::token::MID         ; fbreak; };
        'DATEVALUE'   => { ret = JsParser::token::DATEVALUE   ; fbreak; };
        'MATCH'       => { ret = JsParser::token::MATCH       ; fbreak; };
        'OFFSET'      => { ret = JsParser::token::OFFSET      ; fbreak; };
        'TRUE'        => { ret = JsParser::token::TK_TRUE     ; fbreak; };
        'FALSE'       => { ret = JsParser::token::TK_FALSE    ; fbreak; };
        'CONCATENATE' => { ret = JsParser::token::CONCATENATE ; fbreak; };
        'excel_date' => { ret = JsParser::token::EXCEL_DATE  ; fbreak; };

        #
        # Parts of structured references
        #
        '[#All]'	  => { ret = JsParser::token::WHOLE_TABLE   ; fbreak; };
        '[#Data]'     => { ret = JsParser::token::TABLE_DATA    ; fbreak; };
        '[#Headers]'  => { ret = JsParser::token::TABLE_HEADERS ; fbreak; };
        '[#Totals]'   => { ret = JsParser::token::TABLE_TOTALS  ; fbreak; };
        '[#This Row]' => { ret = JsParser::token::THIS_ROW      ; fbreak; };

        #
        # Operators and other "atomics" that we like to see as they are in the grammar
        #
        '['  => { ret = (JsParser::token_type) '[' ; fbreak; };
        ']'  => { ret = (JsParser::token_type) ']' ; fbreak; };
        '('  => { ret = (JsParser::token_type) '(' ; fbreak; };
        ')'  => { ret = (JsParser::token_type) ')' ; fbreak; };
        '+'  => { ret = (JsParser::token_type) '+' ; fbreak; };
        '-'  => { ret = (JsParser::token_type) '-' ; fbreak; };
        '*'  => { ret = (JsParser::token_type) '*' ; fbreak; };
        '/'  => { ret = (JsParser::token_type) '/' ; fbreak; };
        '^'  => { ret = (JsParser::token_type) '^' ; fbreak; };
        '%'  => { ret = (JsParser::token_type) '%' ; fbreak; };
        '='  => { ret = (JsParser::token_type) '=' ; fbreak; };
        ';'  => { ret = (JsParser::token_type) ';' ; fbreak; };
        '&'  => { ret = (JsParser::token_type) '&' ; fbreak; };
        ','  => { ret = (JsParser::token_type) ',' ; fbreak; };
        ':'  => { ret = (JsParser::token_type) ':' ; fbreak; };
        '!'  => { ret = (JsParser::token_type) '!' ; fbreak; };
        '>'  => { ret = (JsParser::token_type) '>' ; fbreak; };
        '<'  => { ret = (JsParser::token_type) '<' ; fbreak; };

        '<=' => { ret = JsParser::token::LE        ; fbreak; };
        '>=' => { ret = JsParser::token::GE        ; fbreak; };
        '<>' => { ret = JsParser::token::NE        ; fbreak; };

        #
        # Tokens that rely on regular expressions
        #
        variable_name   => { ret = JsParser::token::VAR; (*value) = std::string(ts_, te_ - ts_)        ; fbreak; };
        positive_number => { ret = JsParser::token::NUM; (*value) = double_value_                      ; fbreak; };
        text_literal    => {
                             if ( (int)(te_ - ts_) - 2 < 0 ) {
                                (*value) = "";
                                ret = (JsParser::token_type) '"';
                                fbreak;
                             } else {
                                if ( has_quotes ) {
                                    char* buf = (char*) malloc(te_ - ts_ - 1);
                                    if ( buf == NULL ) {
                                        ret = (JsParser::token_type) '"'; // Ooops we got out of memory, force parse error
                                    }

                                    char* src = (char*) ts_ + 1;
                                    char* dst = buf;
                                    while ( src <= te_ ) {
                                       if ( *(src) == '"' ) {
                                          ++src;
                                          *(dst++) = *(src++);
                                       } else {
                                          *(dst++) = *(src++);
                                       }
                                    }
                                    *(dst) = 0;
                                    (*value) = buf;
                                    free(buf);
                                } else {
                                    (*value) = std::string(ts_ + 1, te_ - ts_ - 2);
                                }
                                ret = JsParser::token::TEXTLITERAL;
                                fbreak;
                             }
                           };

        ws;

    *|;
}%%

/**
 * @brief Constructor
 */
casper::js_compiler::JsScanner::JsScanner (casper::js_compiler::Ast& a_ast)
 : ast_(a_ast)
{
    %% write init;
    OSAL_UNUSED_PARAM(jrxml_js_scanner_first_final);
    OSAL_UNUSED_PARAM(jrxml_js_scanner_error);
    OSAL_UNUSED_PARAM(jrxml_js_scanner_en_main);
}

void casper::js_compiler::JsScanner::SetInput (const char* a_expression, size_t a_lenght)
{
    casper::Scanner::SetInput(a_expression, a_lenght);
    %% write init;
}

/**
 * @brief Destructor
 */
casper::js_compiler::JsScanner::~JsScanner ()
{
    /* empty */
}

casper::js_compiler::JsParser::token_type casper::js_compiler::JsScanner::Scan (casper::js_compiler::JsParser::semantic_type* o_val, casper::js_compiler::location* a_location)
{
    casper::js_compiler::JsParser::token_type ret = casper::js_compiler::JsParser::token::END;
    bool has_quotes = false;

    (*o_val) = ast_.NewAstNode();

    casper::js_compiler::JsParser::semantic_type value = (*o_val);

    %% write exec;

    a_location->begin.line   = 1;
    a_location->begin.column = (int)(ts_ - input_);
    a_location->end.line     = 1;
    a_location->end.column   = (int)(te_ - input_ - 1);

    return ret;
}
