%skeleton "lalr1.cc"
%require "3.0"
%defines
%define api.namespace  { casper::see }
%define api.value.type {class casper::Term }
%define parser_class_name { Parser }

%code requires {
  namespace casper {
      namespace see {
          class Scanner;
          class See;
      }
  }
  #include <cmath> 
  #include <iostream>
  #include "casper/term.h"
  #include "casper/see/sum.h"
}

%parse-param { casper::see::Scanner& a_scanner} { casper::see::See& see_}

%code {
  #include "casper/see/see.h"
  #define yylex a_scanner.Scan
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
        VAR  '=' term END                                     { $$ = $3; see_.SetVariable($1, $$, @1); see_.result_ = $$;   }
      | term  END                                             { $$ = $1; see_.result_ = $$;                                 }

    criteria:
            vector_ref ',' term
            {
                see_.sum_criterias_[$$.aux_text_] = $3;
            }

        |  vector_ref ',' vector_ref
            {
               see_.sum_criterias_[$$.aux_text_] = $3;
            }
        ;

    criteria_list: criteria | criteria_list ',' criteria;

    term:
        VAR                                                     { see_.GetVariable($$, $1, @1);                             }
      | VLOOKUP '(' term ',' vector_ref ',' term ')'            {
                                                                    see_.Vlookup($$, $3, $5, $7, false,
                                                                                 a_scanner.GetInput() + @1.begin.column, @8.begin.column - @1.begin.column);
                                                                }
      | VLOOKUP '(' term ',' vector_ref ',' term ',' term ')'   {
                                                                    see_.Vlookup($$, $3, $5, $7, $9.ConvertToNumber() != 0,
                                                                                 a_scanner.GetInput() + @1.begin.column, @8.begin.column - @1.begin.column);
                                                                }
      | LOOKUP '(' term ',' vector_ref ',' vector_ref ')'       { see_.Lookup($$, $3, $5, $7);                              }
      | SUM         '(' vector_ref ')'                          { see_.Sum($$, $3);                                         }
      | SUM         '(' VAR ':' VAR ')'                         { see_.Sum($$, $3, $5);                                     }
      | SUMIF       '(' vector_ref ',' sum_if_criteria ')'                             {
                                                                                            see_.SumIf($$, $3, $5,
                                                                                                       a_scanner.GetInput() + @1.begin.column, @6.begin.column - @1.begin.column);
                                                                                       }
//      | SUMIF       '(' vector_ref ',' sum_if_criteria ',' vector_ref ')'              {
//                                                                                            see_.SumIf($$, $3, $5, $7,
//                                                                                                        a_scanner.GetInput() + @1.begin.column, @8.begin.column - @1.begin.column);
//                                                                                        }
      | SUMIFS      '(' vector_ref ',' criteria_list ')'        { see_.SumIfs($$, $3);                                      }
      | TEXTLITERAL                                             { $$.SetString($1);                                         }
      | NUM                                                     { $$ = $1;                                                  }
      | TK_TRUE                                                 { $$.BooleanTrue();                                         }
      | TK_FALSE                                                { $$.BooleanFalse();                                        }
      | MAX         '(' max_list ')'                            { $$ = $3;                                                  }
      | MIN         '(' min_list ')'                            { $$ = $3;                                                  }
      | CONCATENATE '(' concatenate_list ')'                    { $$ = $3;                                                  }
      | ROUND       '(' term ',' term ')'                       { $$.Round($3, $5);                                         }
      | ROUNDUP     '(' term ',' term ')'                       { $$.RoundUp($3, $5);                                       }
      | ROUNDDOWN   '(' term ',' term ')'                       { $$.RoundDown($3, $5);                                     }
      | IF          '(' term ',' term ',' term ')'              { $$.If($3, $5, $7);                                        }
      | IF          '(' term ',' term ')'                       { $$.If($3, $5);                                            }
      | IF          '(' term ')'                                { $$.If($3);                                                }
      | DATE        '(' term ',' term ',' term ')'              { $$.SetExcelDate($3, $5, $7);                              }
      | YEAR        '(' term ')'                                { $$.ExcelYear($3);                                         }
      | MONTH       '(' term ')'                                { $$.ExcelMonth($3);                                        }
      | DAY         '(' term ')'                                { $$.ExcelDay($3);                                          }
      | HOUR        '(' term ')'                                { $$.ExcelHour($3);                                         }
      | MINUTE      '(' term ')'                                { $$.ExcelMinute($3);                                       }
      | SECOND      '(' term ')'                                { $$.ExcelSecond($3);                                       }
      | DATEVALUE   '(' term ')'                                { $$.ExcelDateValue($3);                                    }
      | EXCEL_DATE  '(' term ')'                                { $$.ExcelDateValue($3);                                    }
      | ABS         '(' term  ')'                               { $$.Absolute($3);                                          }
      | INDIRECT    '(' term ')'                                { $$ = $3;                                                  }
      | ISERROR     '(' term ')'                                { $$.IsError($3);                                           }
      | IFERROR     '(' term ',' term ')'                       { $$.IfError($3, $5);                                       }
      | FIND        '(' term ',' term ',' term ')'              { $$.Find($3, $5, $7);                                      }
      | FIND        '(' term ',' term ')'                       { $$.Find($3, $5);                                          }
      | AND         '(' and_list ')'                            { $$ = $3;                                                  }
      | OR          '(' or_list ')'                             { $$ = $3;                                                  }
      | LEFT        '(' term ',' term ')'                       { $$.Left($3,$5);                                           }
      | RIGHT       '(' term ',' term ')'                       { $$.Right($3,$5);                                          }
      | MID         '(' term ',' term ',' term ')'              { $$.Mid($3,$5,$7);                                         }
      | MATCH       '(' term ',' VAR TABLE_HEADERS ',' term ')' { see_.TableHeaderMatch($$,$3,$5);                          }
      | OFFSET      '(' table_cell_ref_for_offset ',' term ',' term ')'    { see_.Offset($$,$3,$5,$7);                                 }
      | term '+' term                                           { $$.Add($1,$3);                                            }
      | term '-' term                                           { $$.Subtract($1,$3);                                       }
      | term '*' term                                           { $$.Multiply($1,$3);                                       }
      | term '/' term                                           { $$.Divide($1,$3);                                         }
      | '-' term  %prec NEG                                     { $$.UnaryMinus($2);                                        }
      | '+' term  %prec NEG                                     { $$ = $2;                                                  }
      | term '^' term                                           { $$.Pow($1,$3);                                            }
      | term '%'                                                { $$ = $1.Percentage();                                     }
      | '(' term ')'                                            { $$ = $2;                                                  }
      | term '=' term                                           { $$.Equal($1,$3);                                          }
      | term '&' term                                           { $$.Concatenate($1, $3);                                   }
      | term '>' term                                           { $$.Greater($1, $3);                                       }
      | term '<' term                                           { $$.Less($1, $3);                                          }
      | term GE  term                                           { $$.GreaterOrEqual($1, $3);                                }
      | term LE  term                                           { $$.LessOrEqual($1, $3);                                   }
      | term NE  term                                           { $$.NotEqual($1,$3);                                       }
      | table_cell_ref

    min_list: term                                              { $$ = $1;                                                  }
            | min_list ',' term                                 { $$.Minimum($1, $3);                                       }

    max_list: term                                              { $$ = $1;                                                  }
            | max_list ',' term                                 { $$.Maximum($1, $3);                                       }

    and_list: term                                              { $$ = $1;                                                  }
            | and_list ',' term                                 { $$.And($1, $3);                                           }

    or_list: term
           | or_list  ',' term                                  { $$.Or($1, $3);                                            }

    concatenate_list: term
                    | concatenate_list ',' term                 { $$.Concatenate($1, $3);                                   }

    vector_ref:
        table_cell_ref
      | VAR '[' VAR ']'                                         { $$ = $1; $$.aux_text_ = $3.text_;                         }
      | VAR '[' ']'                                             { $$ = $1; $$.aux_text_ = "";                               }
      | VAR                                                     { $$ = $1; $$.aux_text_ = "";                               }
      | INDIRECT '(' term ')'                                   { $$ = $3; $$.aux_text_ = "";                               }

    sum_if_criteria:
         TEXTLITERAL '&' table_cell_ref                         { $$ = $3; $$.aux_condition_ = $1.text_;                       }
       | TEXTLITERAL '&' VAR '[' VAR ']'                        { $$ = $3; $$.aux_condition_ = $2.text_; $$.aux_text_ = $5.text_; }

    table_cell_ref:
        VAR '[' THIS_ROW ',' '[' VAR ']' ']'                    { see_.GetLinesTableValue($$, $6); $$.aux_text_ = $6.text_; }

    table_cell_ref_for_offset:
        VAR '[' THIS_ROW ',' '[' VAR ']' ']'                    {
                                                                    const bool for_dependencies_check = see_.check_dependencies_;
                                                                    see_.check_dependencies_ = false;
                                                                    see_.GetLinesTableValue($$, $6);
                                                                    see_.check_dependencies_ = for_dependencies_check;
                                                                    $$.aux_text_ = $6.text_;
                                                                }

%%

void casper::see::Parser::error (const location_type& a_location, const std::string& a_msg)
{
    throw OSAL_EXCEPTION("%s:\n"
                         "   %*.*s\n"
                         "   %*.*s^~~~~~\n",
                         a_msg.c_str(),
                         a_scanner.pe_ - a_scanner.input_, a_scanner.pe_ - a_scanner.input_, a_scanner.input_,
                         a_location.begin.column, a_location.begin.column, " ");
}
