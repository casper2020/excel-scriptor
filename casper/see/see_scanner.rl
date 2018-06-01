/**
 * @file see_scanner.rl Implementation of expression tokenizer aka scanner
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
#include "casper/see/see_scanner.h"

%%{
    machine jrxml_see_scanner;
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
        'ABS'         => { ret = Parser::token::ABS         ; fbreak; };
        'MAX'         => { ret = Parser::token::MAX         ; fbreak; };
        'MIN'         => { ret = Parser::token::MIN         ; fbreak; };
        'IF'          => { ret = Parser::token::IF          ; fbreak; };
        'LOOKUP'      => { ret = Parser::token::LOOKUP      ; fbreak; };
        'VLOOKUP'     => { ret = Parser::token::VLOOKUP     ; fbreak; };
        'YEAR'        => { ret = Parser::token::YEAR        ; fbreak; };
        'DATE'        => { ret = Parser::token::DATE        ; fbreak; };
        'ROUND'       => { ret = Parser::token::ROUND       ; fbreak; };
        'ROUNDDOWN'   => { ret = Parser::token::ROUNDDOWN   ; fbreak; };
        'ROUNDUP'     => { ret = Parser::token::ROUNDUP     ; fbreak; };
        'DATE'        => { ret = Parser::token::DATE        ; fbreak; };
        'YEAR'        => { ret = Parser::token::YEAR        ; fbreak; };
        'MONTH'       => { ret = Parser::token::MONTH       ; fbreak; };
        'DAY'         => { ret = Parser::token::DAY         ; fbreak; };
        'HOUR'        => { ret = Parser::token::HOUR        ; fbreak; };
        'MINUTE'      => { ret = Parser::token::MINUTE      ; fbreak; };
        'SECOND'      => { ret = Parser::token::SECOND      ; fbreak; };
        'SUM'         => { ret = Parser::token::SUM         ; fbreak; };
        'INDIRECT'    => { ret = Parser::token::INDIRECT    ; fbreak; };
        'ISERROR'     => { ret = Parser::token::ISERROR     ; fbreak; };
        'SUMIFS'      => { ret = Parser::token::SUMIFS      ; fbreak; };
        'SUMIF'       => { ret = Parser::token::SUMIF       ; fbreak; };
        'FIND'        => { ret = Parser::token::FIND        ; fbreak; };
        'IFERROR'     => { ret = Parser::token::IFERROR     ; fbreak; };
        'AND'         => { ret = Parser::token::AND         ; fbreak; };
        'OR'          => { ret = Parser::token::OR          ; fbreak; };
        'RIGHT'       => { ret = Parser::token::RIGHT       ; fbreak; };
        'LEFT'        => { ret = Parser::token::LEFT        ; fbreak; };
        'MID'         => { ret = Parser::token::MID         ; fbreak; };
        'DATEVALUE'   => { ret = Parser::token::DATEVALUE   ; fbreak; };
        'MATCH'       => { ret = Parser::token::MATCH       ; fbreak; };
        'OFFSET'      => { ret = Parser::token::OFFSET      ; fbreak; };
        'TRUE'        => { ret = Parser::token::TK_TRUE     ; fbreak; };
        'FALSE'       => { ret = Parser::token::TK_FALSE    ; fbreak; };
        'CONCATENATE' => { ret = Parser::token::CONCATENATE ; fbreak; };
        'excel_date_' => { ret = Parser::token::EXCEL_DATE  ; fbreak; };

        #
        # Parts of structured references
        #
        '[#All]'	  => { ret = Parser::token::WHOLE_TABLE   ; fbreak; };
        '[#Data]'     => { ret = Parser::token::TABLE_DATA    ; fbreak; };
        '[#Headers]'  => { ret = Parser::token::TABLE_HEADERS ; fbreak; };
        '[#Totals]'   => { ret = Parser::token::TABLE_TOTALS  ; fbreak; };
        '[#This Row]' => { ret = Parser::token::THIS_ROW      ; fbreak; };

        #
        # Operators and other "atomics" that we like to see as they are in the grammar
        #
        '['  => { ret = (Parser::token_type) '[' ; fbreak; };
        ']'  => { ret = (Parser::token_type) ']' ; fbreak; };
        '('  => { ret = (Parser::token_type) '(' ; fbreak; };
        ')'  => { ret = (Parser::token_type) ')' ; fbreak; };
        '+'  => { ret = (Parser::token_type) '+' ; fbreak; };
        '-'  => { ret = (Parser::token_type) '-' ; fbreak; };
        '*'  => { ret = (Parser::token_type) '*' ; fbreak; };
        '/'  => { ret = (Parser::token_type) '/' ; fbreak; };
        '^'  => { ret = (Parser::token_type) '^' ; fbreak; };
        '%'  => { ret = (Parser::token_type) '%' ; fbreak; };
        '='  => { ret = (Parser::token_type) '=' ; fbreak; };
        ';'  => { ret = (Parser::token_type) ';' ; fbreak; };
        '&'  => { ret = (Parser::token_type) '&' ; fbreak; };
        ','  => { ret = (Parser::token_type) ',' ; fbreak; };
        ':'  => { ret = (Parser::token_type) ':' ; fbreak; };
        '!'  => { ret = (Parser::token_type) '!' ; fbreak; };
        '>'  => { ret = (Parser::token_type) '>' ; fbreak; };
        '<'  => { ret = (Parser::token_type) '<' ; fbreak; };

        '<=' => { ret = Parser::token::LE        ; fbreak; };
        '>=' => { ret = Parser::token::GE        ; fbreak; };
        '<>' => { ret = Parser::token::NE        ; fbreak; };

        #
        # Tokens that rely on regular expressions
        #
        variable_name   => { ret = Parser::token::VAR;  *o_val = std::string(ts_, te_ - ts_)        ; fbreak; };
        positive_number => { ret = Parser::token::NUM;  *o_val = double_value_                      ; fbreak; };
        text_literal    => {
                             if ( (int)(te_ - ts_) - 2 < 0 ) {
                                *o_val = "";
                                ret = (Parser::token_type) '"';
                                fbreak;
                             } else {
                                if ( has_quotes ) {
                                    char* buf = (char*) malloc(te_ - ts_ - 1);
                                    if ( buf == NULL ) {
                                        ret = (Parser::token_type) '"'; // Ooops we got out of memory, force parse error
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
                                    *o_val = buf;
                                    free(buf);
                                } else {
                                    *o_val = std::string(ts_ + 1, te_ - ts_ - 2);
                                }
                                ret = Parser::token::TEXTLITERAL;
                                fbreak;
                             }
                           };

        ws;

    *|;
}%%

/**
 * @brief Constructor
 */
casper::see::Scanner::Scanner ()
{
    %% write init;
    OSAL_UNUSED_PARAM(jrxml_see_scanner_first_final);
    OSAL_UNUSED_PARAM(jrxml_see_scanner_error);
    OSAL_UNUSED_PARAM(jrxml_see_scanner_en_main);
}

void casper::see::Scanner::SetInput (const char* a_expression, size_t a_lenght)
{
    casper::Scanner::SetInput(a_expression, a_lenght);
    %% write init;
}

/**
 * @brief Destructor
 */
casper::see::Scanner::~Scanner ()
{
    /* empty */
}

casper::see::Parser::token_type casper::see::Scanner::Scan (casper::see::Parser::semantic_type* o_val, casper::see::location* a_location)
{
    casper::see::Parser::token_type ret = casper::see::Parser::token::END;
    bool has_quotes = false;

    %% write exec;

    a_location->begin.line   = 1;
    a_location->begin.column = (int)(ts_ - input_);
    a_location->end.line     = 1;
    a_location->end.column   = (int)(te_ - input_ - 1);
    return ret;
}
