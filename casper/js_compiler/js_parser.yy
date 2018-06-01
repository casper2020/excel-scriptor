%skeleton "lalr1.cc"
%require "3.0"
%defines
%define api.namespace  { casper::js_compiler }
%define api.value.type {class AstNode* }
%define parser_class_name { JsParser }

%code requires {
  namespace casper {
      namespace js_compiler {
          class JsScanner;
          class Interpreter;
      }
  }
  #include <cmath>
  #include <iostream>
  #include "casper/js_compiler/ast.h"
  #include "osal/exception.h"
}

%parse-param { Ast& ast } { casper::js_compiler::JsScanner& jscanner_ } { casper::js_compiler::Interpreter& expr_ }

%code {
  #include "casper/js_compiler/js_scanner.h"
  #include "casper/js_compiler/interpreter.h"
  #define yylex jscanner_.Scan
}

%token END           0
%token NUM           "Number"
%token TK_TRUE       "TRUE"
%token TK_FALSE      "FALSE"
%token VAR           "Variable"
%token IF            "IF"
%token MAX           "MAX"
%token MIN           "MIN"
%token CONCATENATE   "CONCATENATE"
%token LOOKUP        "LOOKUP"
%token VLOOKUP       "VLOOKUP"
%token ISERROR       "ISERROR"
%token IFERROR       "IFERROR"
%token FIND          "FIND"
%token SUMIFS        "SUMIFS"
%token SUMIF         "SUMIF"
%token DATE          "DATE"
%token ROUND         "ROUND"
%token ROUNDDOWN     "ROUNDDOWN"
%token ROUNDUP       "ROUNDUP"
%token YEAR          "YEAR"
%token MONTH         "MONTH"
%token DAY           "DAY"
%token HOUR          "HOUR"
%token MINUTE        "MINUTE"
%token SECOND        "SECOND"
%token DATEVALUE     "DATEVALUE"
%token EXCEL_DATE    "excel_date"
%token SUM           "SUM"
%token INDIRECT      "INDIRECT"
%token MATCH         "MATCH"
%token OFFSET        "OFFSET"
%token RIGHT         "RIGHT"
%token LEFT          "LEFT"
%token MID           "MID"
%token ABS           "ABS"
%token TEXTLITERAL   "Literal string"
%token WHOLE_TABLE   "WHOLE_TABLE"
%token TABLE_DATA    "TABLE_DATA"
%token TABLE_HEADERS "TABLE_HEADERS"
%token TABLE_TOTALS  "TABLE_TOTALS"
%token THIS_ROW      "THIS_ROW"
%token AND           "AND"
%token OR            "OR"
%token LE            "<="
%token GE            ">="
%token NE            "<>"

%left '-' '+' '&' '<' '>' '=' NE LE GE
%left '*' '/'
%right '%'
%precedence NEG   /* negation--unary minus */

/*
 * The grammar follows
 */
%%
    input:
        term  END                       { ast.root_ = $1;                               }
      | VAR  '=' term END               { $$ = ast.AddVar($1, $3);   ast.root_ = $$;    }

    criteria:
        vector_ref ',' term             {  ast.sum_criterias_[$1] = $3;                 }
      | vector_ref ',' vector_ref       {  ast.sum_criterias_[$1] = $3;                 }
      ;

    criteria_list:
        criteria
      | criteria_list ',' criteria;

    term:
        VAR                                                     { $$ = ast.GetVar($1);                                 }
      //TODO: OFFSET is not implemented.
      | VLOOKUP '(' term ',' vector_ref ',' term ')'            { $$ = ast.Operation("VLookup",$3,$5,$7);              }
      | VLOOKUP '(' term ',' vector_ref ',' term ',' term ')'   { $$ = ast.Operation("VLookup",$3,$5,$7,$9);           }
      | LOOKUP '(' term ',' vector_ref ',' vector_ref ')'       { $$ = ast.Operation("Lookup", $3, $5, $7);            }
      | SUM         '(' vector_ref ')'                          { $$ = ast.Operation("SUM1", $3);                      }
      | SUM         '(' VAR ':' VAR ')'                         { $$ = ast.Operation("SUM2", $3, $5);                  }
      | SUMIF       '(' vector_ref ',' sum_if_criteria ')'      { $$ = ast.Operation("SUMIF", $3, $5);                 }
      | SUMIFS      '(' vector_ref ',' criteria_list ')'        { $$ = ast.Operation("SUMIFS", $3);                    }
      | TEXTLITERAL                                             { $$ = $1;                                             }
      | NUM                                                     { $$ = $1;                                             }
      | TK_TRUE                                                 { $$ = ast.Bool(true);                                 }
      | TK_FALSE                                                { $$ = ast.Bool(false);                                }
      | MAX         '(' max_list ')'                            { $$ = $3;                                             }
      | MIN         '(' min_list ')'                            { $$ = $3;                                             }
      | CONCATENATE '(' concatenate_list ')'                    { $$ = $3;                                             }
      | ROUND       '(' term ',' term ')'                       { $$ = ast.Expression("RND",$3,$5);                    }
      | ROUNDUP     '(' term ',' term ')'                       { $$ = ast.Expression("RNDUP",$3,$5);                  }
      | ROUNDDOWN   '(' term ',' term ')'                       { $$ = ast.Expression("RNDDN",$3,$5);                  }
      | IF          '(' term ',' term ',' term ')'              { $$ = ast.If($3, $5, $7);                             }
      | IF          '(' term ',' term ')'                       { $$ = ast.If($3, $5, nullptr);                        }
      | IF          '(' term ')'                                { $$ = ast.If($3, nullptr, nullptr);                   }
      | DATE        '(' term ',' term ',' term ')'              { $$ = ast.DateOp("ND", $3, $5, $7);                   }
      | YEAR        '(' term ')'                                { $$ = ast.DateOp("getYear", $3);                      }
      | MONTH       '(' term ')'                                { $$ = ast.DateOp("getMonth", $3);                     }
      | DAY         '(' term ')'                                { $$ = ast.DateOp("getDay", $3);                       }
      | HOUR        '(' term ')'                                { $$ = ast.DateOp("getHours", $3);                     }
      | MINUTE      '(' term ')'                                { $$ = ast.DateOp("getMinutes", $3);                   }
      | SECOND      '(' term ')'                                { $$ = ast.DateOp("getSeconds", $3);                   }
      | DATEVALUE   '(' term ')'                                { $$ = ast.DateOp("ND1",$3);                           }
      | EXCEL_DATE  '(' term ')'                                { $$ = ast.DateOp("ND1",$3);                           }
      | ABS         '(' term  ')'                               { $$ = ast.Operation("Math.abs",$3);                   }
      | INDIRECT    '(' term ')'                                { $$ = $3;                                             }
      | ISERROR     '(' term ')'                                { $$ = ast.Operation("isError",$3);                    }
      | IFERROR     '(' term ',' term ')'                       { $$ = ast.Operation("ifError", $3, $5);               }
      | FIND        '(' term ',' term ',' term ')'              { $$ = ast.StrOp("indexOf",$3,$5,$7);                  }
      | FIND        '(' term ',' term ')'                       { $$ = ast.StrOp("indexOf",$3,$5);                     }
      | AND         '(' and_list ')'                            { $$ = $3;                                             }
      | OR          '(' or_list ')'                             { $$ = $3;                                             }
      | LEFT        '(' term ',' term ')'                       { $$ = ast.StrOp("LFT",$3,$5);                         }
      | RIGHT       '(' term ',' term ')'                       { $$ = ast.StrOp("RGT",$3,$5);                         }
      | MID         '(' term ',' term ',' term ')'              { $$ = ast.StrOp("MID",$3,$5,$7);                      }
      | MATCH       '(' term ',' VAR TABLE_HEADERS ',' term ')' { $$ = ast.Operation("MATCH",$3,$5);                   }
      | OFFSET      '(' table_cell_ref ',' term ',' term ')'    { $$ = ast.Operation("OFFSET",$3,$5,$7);               }
      | term '+' term                                           { $$ = ast.Expression("+",$1,$3);                      }
      | term '-' term                                           { $$ = ast.Expression("-",$1,$3);                      }
      | term '*' term                                           { $$ = ast.Expression("*",$1,$3);                      }
      | term '/' term                                           { $$ = ast.Expression("/",$1,$3);                      }
      | '-' term  %prec NEG                                     { $$ = ast.Expression("UM",$2,$2);                     }
      | '+' term  %prec NEG                                     { $$ = $2;                                             }
      | term '^' term                                           { $$ = ast.Expression("^",$1,$3);                      }
      | term '%'                                                { $$ = ast.Expression("PERCENT",$1,nullptr);           }
      | '(' term ')'                                            { $2->pare_ = true; $$ = $2;                           }
      | term '=' term                                           { $$ = ast.Expression("==",$1,$3);                     }
      | term '&' term                                           { $$ = ast.Expression("&",$1,$3);                      }
      | term '>' term                                           { $$ = ast.Expression(">",$1,$3);                      }
      | term '<' term                                           { $$ = ast.Expression("<",$1,$3);                      }
      | term GE  term                                           { $$ = ast.Expression(">=",$1,$3);                     }
      | term LE  term                                           { $$ = ast.Expression("<=",$1,$3);                     }
      | term NE  term                                           { $$ = ast.Expression("!=",$1,$3);                     }
      | table_cell_ref

    min_list: term                                              { $$ = $1;                                             }
            | min_list ',' term                                 { $$ = ast.Expression("MIN",$1,$3);                    }

    max_list: term                                              { $$ = $1;                                             }
            | max_list ',' term                                 { $$ = ast.Expression("MAX",$1,$3);                    }

    and_list: term                                              { $$ = $1;                                             }
            | and_list ',' term                                 { $$ = ast.Expression("&&",$1,$3);                     }

    or_list: term
           | or_list  ',' term                                  { $$ = ast.Expression("||",$1,$3);                     }

    concatenate_list: term
                    | concatenate_list ',' term                 { $$ = ast.Expression("&",$1,$3);                      }

    vector_ref:
       table_cell_ref
      | VAR '[' VAR ']'                                         { $$ = ast.Operation("Vector",$1,$3);                  }
      | VAR '[' ']'                                             { $$ = ast.Operation("Vector",$1,ast.NewAstNode("genvar1")); }
      | VAR                                                     { $$ = $1;                                             }
      | INDIRECT '(' term ')'                                   { $$ = $3;                                             }

    sum_if_criteria:
         TEXTLITERAL '&' table_cell_ref                         { $$ = ast.Operation("COND",$1,$3);                   }
       | TEXTLITERAL '&' VAR '[' VAR ']'                        { $$ = ast.Operation("COND",$1,$5);                 }

    table_cell_ref:
        VAR '[' THIS_ROW ',' '[' VAR ']' ']'                    { std::stringstream test(jscanner_.GetInput());
                                                                  std::string segment;
                                                                  std::vector<std::string> seglist;
                                                                  while(std::getline(test, segment, '=')) {
                                                                     seglist.push_back(segment);}
                                                                  $$ = ast.Operation("LinesRef",ast.NewAstNode(seglist[0]),$6); }

    // TODO: This is used by the OFFSET function
    // table_cell_ref_for_offset:
    //     VAR '[' THIS_ROW ',' '[' VAR ']' ']'                    {
    //                                                                 const bool for_dependencies_check = see_.check_dependencies_;
    //                                                                 see_.check_dependencies_ = false;
    //                                                                 see_.GetLinesTableValue($$, $6);
    //                                                                 see_.check_dependencies_ = for_dependencies_check;
    //                                                                 $$.aux_text_ = $6.text_;
    //                                                             }

%%

void casper::js_compiler::JsParser::error (const location_type& a_location, const std::string& a_msg)
{
    throw OSAL_EXCEPTION("%s:\n"
                         "   %*.*s\n"
                         "   %*.*s^~~~~~\n",
                         a_msg.c_str(),
                         jscanner_.pe_ - jscanner_.input_, jscanner_.pe_ - jscanner_.input_, jscanner_.input_,
                         a_location.begin.column, a_location.begin.column, " ");
}
