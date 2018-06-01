#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include "v8_shell.h"

int main(int argc, char* argv[]) {

    auto main_start = std::chrono::high_resolution_clock::now();

    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    v8::Platform* platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);
    v8::V8::Initialize();
    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
    v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);
    run_shell = (argc == 1);
    int result;
    {
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Context> context = CreateShellContext(isolate);
        if (context.IsEmpty()) {
            fprintf(stderr, "Error creating context\n");
            return 1;
        }

        //------------------------------------------------------------------------------

        v8::Context::Scope context_scope(context);

        //Check deps and compile into javascript
        std::system("../excelscriptor");

        //Load required files

        char* files_to_load[63];
        files_to_load[0] =(char*)"../tables_mod/COMP_TXT.js";
        files_to_load[1] =(char*)"../tables_mod/DIAS_UTEIS_2014.js";
        files_to_load[2] =(char*)"../tables_mod/FICHA_FUNC.js";
        files_to_load[3] =(char*)"../tables_mod/IRS_ACN1N.js";
        files_to_load[4] =(char*)"../tables_mod/IRS_ACN1S.js";
        files_to_load[5] =(char*)"../tables_mod/IRS_ACS1N.js";
        files_to_load[6] =(char*)"../tables_mod/IRS_ACS1S.js";
        files_to_load[7] =(char*)"../tables_mod/IRS_ACS2N.js";
        files_to_load[8] =(char*)"../tables_mod/IRS_ACS2S.js";
        files_to_load[9] =(char*)"../tables_mod/IRS_MAN1N.js";
        files_to_load[10] =(char*)"../tables_mod/IRS_MAN1S.js";
        files_to_load[11] =(char*)"../tables_mod/IRS_MAS1N.js";
        files_to_load[12] =(char*)"../tables_mod/IRS_MAS1S.js";
        files_to_load[13] =(char*)"../tables_mod/IRS_MAS2N.js";
        files_to_load[14] =(char*)"../tables_mod/IRS_MAS2S.js";
        files_to_load[15] =(char*)"../tables_mod/IRS_PTN1N.js";
        files_to_load[16] =(char*)"../tables_mod/IRS_PTN1S.js";
        files_to_load[17] =(char*)"../tables_mod/IRS_PTS1N.js";
        files_to_load[18] =(char*)"../tables_mod/IRS_PTS1S.js";
        files_to_load[19] =(char*)"../tables_mod/IRS_PTS2N.js";
        files_to_load[20] =(char*)"../tables_mod/IRS_PTS2S.js";
        files_to_load[21] =(char*)"../tables_mod/LIMITE_DESLOCACOES.js";
        files_to_load[22] =(char*)"../tables_mod/LIM_AJ_CUST_INTERNAC.js";
        files_to_load[23] =(char*)"../tables_mod/LIM_AJ_CUST_INTERNAC_MEMB_GOVRN.js";
        files_to_load[24] =(char*)"../tables_mod/LIM_AJ_CUST_NAC.js";
        files_to_load[25] =(char*)"../tables_mod/LIM_AJ_CUST_NAC_MEMB_GOVRN.js";
        files_to_load[26] =(char*)"../tables_mod/LIM_REFEICAO.js";
        files_to_load[27] =(char*)"../tables_mod/LISTA_ABONOS.js";
        files_to_load[28] =(char*)"../tables_mod/LISTA_DESC_VENC.js";
        files_to_load[29] =(char*)"../tables_mod/LISTA_ENTIDADES.js";
        files_to_load[30] =(char*)"../tables_mod/REGRAS_DESC_ENT.js";
        files_to_load[31] =(char*)"../tables_mod/RESULTADO_ARRD_DMR.js";
        files_to_load[32] =(char*)"../tables_mod/RUAS_T13_MAPPING.js";
        files_to_load[33] =(char*)"../tables_mod/SBtx_IRS_DEFAULT_0.js";
        files_to_load[34] =(char*)"../tables_mod/SBtx_IRS_casado_1titular_2016.js";
        files_to_load[35] =(char*)"../tables_mod/SBtx_IRS_casado_1titular_2017_06.js";
        files_to_load[36] =(char*)"../tables_mod/SBtx_IRS_casado_1titular_2017_11.js";
        files_to_load[37] =(char*)"../tables_mod/SBtx_IRS_geral_2016.js";
        files_to_load[38] =(char*)"../tables_mod/SBtx_IRS_geral_2017_06.js";
        files_to_load[39] =(char*)"../tables_mod/SBtx_IRS_geral_2017_11.js";
        files_to_load[40] =(char*)"../tables_mod/TABELA_10_RUAS.js";
        files_to_load[41] =(char*)"../tables_mod/TABELA_ALTERACOES.js";
        files_to_load[42] =(char*)"../tables_mod/TABELA_ALTERACOES_DESC.js";
        files_to_load[43] =(char*)"../tables_mod/TABELA_RETROATIVOS.js";
        files_to_load[44] =(char*)"../tables_mod/TIPO_CESSACOES.js";
        files_to_load[45] =(char*)"../tables_mod/form_encerramento.js";
        files_to_load[46] =(char*)"../tables_mod/month_name.js";
        files_to_load[47] =(char*)"../tables_mod/payroll_employee_types.js";
        files_to_load[48] =(char*)"../tables_mod/payroll_enumerations_globals.js";
        files_to_load[49] =(char*)"../tables_mod/payroll_enumerations_lookup_insurance_companies.js";
        files_to_load[50] =(char*)"../tables_mod/payroll_enumerations_marital_statuses.js";
        files_to_load[51] =(char*)"../tables_mod/payroll_enumerations_statement_code_DMR.js";
        files_to_load[52] =(char*)"../tables_mod/payroll_enumerations_statement_code_DRF.js";
        files_to_load[53] =(char*)"../tables_mod/payroll_enumerations_statement_code_DRI.js";
        files_to_load[54] =(char*)"../tables_mod/payroll_enumerations_tax_offices.js";
        files_to_load[55] =(char*)"../tables_mod/payroll_global_settings.js";
        files_to_load[56] =(char*)"../tables_mod/payroll_grant_types.js";
        files_to_load[57] =(char*)"../tables_mod/scale_ias.js";
        files_to_load[58] =(char*)"../aux_functions.js";
        files_to_load[59] =(char*)"initial_vars.js";
        files_to_load[60] =(char*)"aliases.js";
        files_to_load[61] =(char*)"lines.js";
        files_to_load[62] =(char*)"compiled_formulas_v8.js";

        result = RunMain(isolate, platform, 63, files_to_load);

        //Declarations

        v8::Local<v8::String> func_name =
        v8::String::NewFromUtf8(context->GetIsolate(), "calculate", v8::NewStringType::kNormal).ToLocalChecked();
        v8::Local<v8::String> func_name2 =
        v8::String::NewFromUtf8(context->GetIsolate(), "checkNaN", v8::NewStringType::kNormal).ToLocalChecked();
        v8::Local<v8::Object> global = context->Global();
        v8::Local<v8::Value> value = global->Get(context, func_name).ToLocalChecked();;
        v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(value);
        v8::Local<v8::Value> value2 = global->Get(context, func_name2).ToLocalChecked();;
        v8::Local<v8::Function> func2 = v8::Local<v8::Function>::Cast(value2);
        v8::Local<v8::Value> js_result;
        v8::Local<v8::Value> args[3];

        //Calculate the expressions-------------------------------------------------

        std::string ano_proc = "2018";
        std::string mes_proc = "3";

        const char* arg1 = ano_proc.c_str();
        args[1] = v8::String::NewFromUtf8(context->GetIsolate(), arg1, v8::NewStringType::kNormal).ToLocalChecked();

        const char* arg2 = mes_proc.c_str();
        args[2] = v8::String::NewFromUtf8(context->GetIsolate(), arg2, v8::NewStringType::kNormal).ToLocalChecked();


        for(int k=0; k<20; k++){
          for(int kk=0; kk<1; kk++){
            for(int i=1; i<21; i++){
                std::string cod_func = std::to_string(i);
                const char* arg0 = cod_func.c_str();
                args[0] = v8::String::NewFromUtf8(context->GetIsolate(), arg0, v8::NewStringType::kNormal).ToLocalChecked();

                //auto start1 = std::chrono::high_resolution_clock::now();

                if (!func->Call(context, context->Global(), 3, args).ToLocal(&js_result))
                    printf("error..\n");
                else{
                    v8::String::Utf8Value str_2(js_result);
                    const char* cstr = ToCString(str_2);
                    //printf("%s\n", cstr);
                    if (!func2->Call(context, context->Global(), 0, args).ToLocal(&js_result))
                        printf("error..\n");
                    else{
                        v8::String::Utf8Value str_3(js_result);
                        const char* cstr2 = ToCString(str_3);
                        if(cstr2[0]!='0') printf("ERROR in %s, COD_FUNC = %d\n", cstr2, i);
                        //else printf("%s\n", cstr);
                    }
                }

                //auto end1 = std::chrono::high_resolution_clock::now();

                // auto elapsed_seconds1 = std::chrono::duration_cast<std::chrono::microseconds>( end1 - start1 ).count();
                // std::cout << "\nThis calculation took: " << (elapsed_seconds1/1000.0) << " ms\n";


            }
          }
            std::system("../excelscriptor");
            result = RunMain(isolate, platform, 63, files_to_load);
        }

    }
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete platform;
    delete create_params.array_buffer_allocator;

    auto main_end = std::chrono::high_resolution_clock::now();

    auto elapsed_main = std::chrono::duration_cast<std::chrono::microseconds>( main_end - main_start ).count();
    std::cout << "\nTotal time: " << (elapsed_main/1000.0) << " ms\n";


    return result;
}
