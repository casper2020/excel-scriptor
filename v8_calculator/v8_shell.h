#include "/Users/bruno/work/v8/depot_tools/v8/include/libplatform/libplatform.h"
#include "/Users/bruno/work/v8/depot_tools/v8/include/v8.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

/**
 * This sample program shows how to implement a simple javascript shell
 * based on V8.  This includes initializing V8 with command line options,
 * creating global functions, compiling and executing strings.
 *
 * For a more sophisticated shell, consider using the debug shell D8.
 */

v8::Local<v8::Context> CreateShellContext(v8::Isolate* isolate);
void RunShell(v8::Local<v8::Context> context, v8::Platform* platform);
int RunMain(v8::Isolate* isolate, v8::Platform* platform, int argc,
            char* argv[]);
bool ExecuteString(v8::Isolate* isolate, v8::Local<v8::String> source,
                   v8::Local<v8::Value> name, bool print_result,
                   bool report_exceptions);
const char* ExecuteString2(v8::Isolate* isolate, v8::Local<v8::String> source,
                  v8::Local<v8::Value> name, bool print_result,
                  bool report_exceptions);
const char* ExecuteString3(v8::Isolate* isolate, v8::Local<v8::String> source,
                   v8::Local<v8::Value> name, bool print_result,
                   bool report_exceptions);
void Print(const v8::FunctionCallbackInfo<v8::Value>& args);
void Read(const v8::FunctionCallbackInfo<v8::Value>& args);
void Load(const v8::FunctionCallbackInfo<v8::Value>& args);
void Quit(const v8::FunctionCallbackInfo<v8::Value>& args);
void Version(const v8::FunctionCallbackInfo<v8::Value>& args);
v8::MaybeLocal<v8::String> ReadFile(v8::Isolate* isolate, const char* name);
void ReportException(v8::Isolate* isolate, v8::TryCatch* handler);
const char* ToCString(const v8::String::Utf8Value& value);

static bool run_shell;
