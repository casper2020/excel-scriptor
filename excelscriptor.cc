#include <iostream>
#include <fstream>
#include <sstream>
#include "casper/js_compiler/interpreter.h"
#include "casper/see/see.h"

void getInitVars(std::map<std::string, casper::Term> ref_symtab, std::map<std::string, casper::Term> line_values){

    std::stringstream output;

    for (auto const& x : ref_symtab) {
        if(x.second.GetType()==1)
            output << "var " << x.first << " = { x : "
            << x.second.GetNumber() << ", y : \"" << x.first << "\" };\n";
        else if(x.second.GetType()==2)
            output << "var " << x.first << " = { x : \""
            << x.second.GetText() << "\", y : \"" << x.first << "\" };\n";
        else
            output << "var " << x.first << " = { x : "
            << x.second.GetBoolean() << ", y : \"" << x.first << "\" };\n";
    }

    for (auto const& x : line_values) {
        if(x.second.GetType()==1)
            output << "var " << x.first << " = { x : "
            << x.second.GetNumber() << ", y : \"" << x.first << "\" };\n";
        else if(x.second.GetType()==2)
            output << "var " << x.first << " = { x : \""
            << x.second.GetText() << "\", y : \"" << x.first << "\" };\n";
        else
            output << "var " << x.first << " = { x : "
            << x.second.GetBoolean() << ", y : \"" << x.first << "\" };\n";
    }

    std::ofstream outFile;
    outFile.open ("initial_vars.js");
    outFile << output.rdbuf();
    outFile.close();
}

void getAliases(std::map<std::string, std::string> alias_list){

    std::stringstream output;

    for (auto const& x : alias_list) {
        output << "var " << x.first << " = { x : undefined , y : \""
        << x.first << "\" };\n";
        if(x.first != x.second)
            output << "var " << x.second << " = " << x.first << ";\n";
    }

    std::ofstream outFile;
    outFile.open ("aliases.js");
    outFile << output.rdbuf();
    outFile.close();
}


void getColumnIndex(std::map<int, std::string> a_column_list, int row_cnt, int first_row){

    std::stringstream output;

    output << "function getColumnRef(a_col){\n";
    output << "switch(a_col) {\n";
    for (auto const& x : a_column_list) {
        output << "case \'" << x.second << "\':\n";
        output << "return " << x.first << ";\n";
    }
    output << "default: return -1;}\n}\n";

    output << "\nvar columnNames = [\'\',\'\'";

    for (auto const& x : a_column_list) {
        output << ",\'" << x.second << "\'";
    }

    output << "];\n\n";

    output << "var sizeOfLines = " << a_column_list.size() << ";\n\n";
    output << "var firstRow = " << first_row << ";\n\n";
    output << "var rowCount = " << row_cnt << ";\n\n";

    std::ofstream outFile;
    outFile.open ("lines.js");
    outFile << output.rdbuf();
    outFile.close();
}

void compileModelChrome(std::vector<casper::see::Formula*> formula_list){

    std::stringstream output;

    casper::js_compiler::Interpreter interpreter;

    output << "function calculate(){\n\n";

    output << "var new_cod_func = document.getElementById(\"cod_func\").value;\n";
    output << "var new_ano = document.getElementById(\"ano\").value;\n";
    output << "var new_mes = document.getElementById(\"mes\").value;\n\n";

    output << "COD_FUNC.x = new_cod_func;\n";
    output << "ANO_PROC.x = new_ano;\n";
    output << "MES_PROC.x = new_mes;\n\n";

    output << "var tt0 = performance.now();\n\n";

    for(int j=0; j<formula_list.size(); j++){
        std::string formula = formula_list[j]->getFormula();

        if(formula_list[j]->IsSum()){
            std::set<std::string> precs = formula_list[j]->getPrecedents();
            try{
                output << interpreter.ConvertSum(formula, precs).c_str() << ";\n\n";
            } catch (osal::Exception& a_exception){
                std::cout << a_exception.Message() << "\n";
            }
        } else if(formula_list[j]->IsSumIfs()){
            std::set<std::string> precs = formula_list[j]->getPrecedents();
            try{
                output << interpreter.ConvertSumIfs(formula, precs).c_str() << ";\n\n";
            } catch (osal::Exception& a_exception){
                std::cout << a_exception.Message() << "\n";
            }
        } else{
            try{
                output << interpreter.Convert(formula).c_str() << ";\n\n";
            } catch (osal::Exception& a_exception){
                std::cout << a_exception.Message() << "\n";
            }
        }
    }

    output << "var tt1 = performance.now();\n";
    output << "document.getElementById(\"calc\").innerHTML = (\"Calculations took \" + (tt1 - tt0) + \" milliseconds.<br><br>\");\n";
    output << "document.getElementById(\"ac_a\").innerHTML =(\"AC_ABONOS= \" + AC_ABONOS.x + \"<br><br>\");\n";
    output << "document.getElementById(\"ac_d\").innerHTML =(\"AC_DESCONTOS= \" + AC_DESCONTOS.x+ \"<br><br>\");\n";
    output << "document.getElementById(\"t_irs\").innerHTML =(\"T_IRS= \" + T_IRS.x + \"<br><br>\");\n";
    output << "document.getElementById(\"t_s_irs\").innerHTML =(\"T_SOBRETX_IRS= \" + T_SOBRETX_IRS.x + \"<br><br>\");\n";
    output << "document.getElementById(\"vsegsocial\").innerHTML =(\"V_SEG_SOCIAL= \" + V_SEG_SOCIAL.x + \"<br><br>\");\n";
    output << "document.getElementById(\"liq\").innerHTML =(\"LIQUIDO= \" + LIQUIDO.x + \"<br><br>\");\n\n}\n\n";

    for(int i=0; i<interpreter.var_names_.size(); i++){
        output << "var " << interpreter.var_names_[i] << ";\n";
    }

    output << "\nfunction checkNaN(){\n";
    for(int i=0; i<interpreter.var_names_.size(); i++){
        if(interpreter.var_names_[i][0]=='f'){
            output << "if (typeof " << interpreter.var_names_[i] << " === \'string\' || ";
            output << interpreter.var_names_[i] << " instanceof String) return \'" << interpreter.var_names_[i] << "\';\n";
        }
    }
    output << "return 0;}\n";

    std::ofstream outFile;
    outFile.open ("compiled_formulas.js");
    outFile << output.rdbuf();
    outFile.close();
}

void compileModelV8(std::vector<casper::see::Formula*> formula_list){

    std::stringstream output;

    casper::js_compiler::Interpreter interpreter;

    output << "function calculate(new_cod_func,new_ano,new_mes){\n\n";

    output << "COD_FUNC.x = new_cod_func;\n";
    output << "ANO_PROC.x = new_ano;\n";
    output << "MES_PROC.x = new_mes;\n\n";

    for(int j=0; j<formula_list.size(); j++){
        std::string formula = formula_list[j]->getFormula();

        if(formula_list[j]->IsSum()){
            std::set<std::string> precs = formula_list[j]->getPrecedents();
            try{
                output << interpreter.ConvertSum(formula, precs).c_str() << ";\n\n";
            } catch (osal::Exception& a_exception){
                std::cout << a_exception.Message() << "\n";
            }
        } else if(formula_list[j]->IsSumIfs()){
            std::set<std::string> precs = formula_list[j]->getPrecedents();
            try{
                output << interpreter.ConvertSumIfs(formula, precs).c_str() << ";\n\n";
            } catch (osal::Exception& a_exception){
                std::cout << a_exception.Message() << "\n";
            }
        } else{
            try{
                output << interpreter.Convert(formula).c_str() << ";\n\n";
            } catch (osal::Exception& a_exception){
                std::cout << a_exception.Message() << "\n";
            }
        }
    }

    //output << "return \'DONE\';\n}\n\n";
    output << "return (\'Calculations for \' + COD_FUNC.x + \' \'+ ANO_PROC.x + \' \' + MES_PROC.x + \'\\n\\nAC_ABONOS = \'";
    output << "+ AC_ABONOS.x +\'\\n\\nAC_DESCONTOS = \' + AC_DESCONTOS.x + \'\\n\\nT_IRS = \' + T_IRS.x +";
    output << "\'\\n\\nT_SOBRETX_IRS = \' + T_SOBRETX_IRS.x + \'\\n\\nV_SEG_SOCIAL = \' + V_SEG_SOCIAL.x +";
    output << "\'\\n\\nLIQUIDO = \' + LIQUIDO.x);\n}\n\n";


    for(int i=0; i<interpreter.var_names_.size(); i++){
        output << "var " << interpreter.var_names_[i] << ";\n";
    }

    output << "\nfunction checkNaN(){\n";
    for(int i=0; i<interpreter.var_names_.size(); i++){
        if(interpreter.var_names_[i][0]=='f'){
            output << "if (typeof " << interpreter.var_names_[i] << " === \'string\' || ";
            output << interpreter.var_names_[i] << " instanceof String) return \'" << interpreter.var_names_[i] << "\';\n";
        }
    }
    output << "return 0;}\n";

    std::ofstream outFile;
    outFile.open ("compiled_formulas_v8.js");
    outFile << output.rdbuf();
    outFile.close();
}

void printTest(){
    casper::js_compiler::Interpreter teste;

    const std::string& input3  = "SUMIF(FICHA_FUNC[employee_id],\">1000\" & FICHA_FUNC[salary_amount])";
    const std::string& input4  = "D573=\"No caso de cessação de contratos sem termo celebrados entre 01/11/2011 e 30/09/2013,  a compensação corresponderá \"&\" (A) a 20 dias de RB (e diuturnidades, se aplicável), por cada ano completo de duração do contrato de trabalho (ou fracção de ano, na respectiva proporção) até  30/09/2013; \"&\"(B) i)18 dias de RB e diuturnidades por cada ano completo de antiguidade, no que respeita aos 3 primeiros anos de duração do contrato; ii) 12 dias de RB e diuturnidades por cada ano completo de antiguidade, nos anos subsequentes.\"&\" LIMITES:  Caso a compensação calculada com referência a 30 de Setembro de 2013 (A) , seja superior a 12 vezes a retribuição base e diuturnidades do trabalhador ou a 240 vezes a RMMG (\" & V_LIM_240XORDENADO_MIN_NAC & \"€), a compensação devida estará apurada nesta fase, não havendo que avançar para as fase (B).\nSe a compensação for inferior aos valores indicados passar-se-á para a fase B, sempre tendo em consideração aquele limite máximo\"";

    try{
        printf("3: %s\n", teste.Convert(input3).c_str());
        printf("4: %s\n", teste.Convert(input4).c_str());
    }catch (osal::Exception& a_exception){
        std::cout << a_exception.Message() << "\n";
    }
}



int main() {

    const char* path_to_json = "";

    casper::StringMultiHash str_scalar_dict;

    casper::see::See* see = new casper::see::See();

    see->SetJsonDataPath(path_to_json);

    try {
        see->LoadModel(str_scalar_dict);
    } catch (osal::Exception& a_exception) {
        std::cout << a_exception.Message() << "\n";
    } catch (...) {
        /* fall through */
    }

    std::map<std::string, casper::Term> ref_symtab = see->getRefSymtab();

    std::map<std::string, casper::Term> line_values = see->getLineValues();

    std::vector<casper::see::Formula*> formula_list = see->getFormulas();

    std::map<std::string, std::string> alias_list = see->getAliases();

    std::map<int, std::string> column_list = see->getColumnNameIndex();

    int row_cnt = see->row_count_;
    int first_row = see->table_header_row_;


    getInitVars(ref_symtab,line_values);

    getAliases(alias_list);

    getColumnIndex(column_list, row_cnt, first_row);

    compileModelChrome(formula_list);

    //compileModelV8(formula_list);

    //printTest();

    return 0;
}
