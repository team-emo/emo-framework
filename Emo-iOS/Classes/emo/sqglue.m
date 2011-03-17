#include "squirrel.h"
#include "sqstdio.h"
#include "sqstdaux.h"
#include "sqglue.h"
#include "common.h";
#include "sqfunc.h";

/*
 * Logging
 */
extern void LOGI(const SQChar* msg);
extern void LOGW(const SQChar* msg);
extern void LOGE(const SQChar* msg);

HSQUIRRELVM g_sqvm;	

bool initSQGlue() {
	g_sqvm = sq_open(SQUIRREL_VM_INITIAL_STACK_SIZE);
	
	sqstd_seterrorhandlers(g_sqvm);
	sq_setprintfunc(g_sqvm, sq_printfunc, sq_errorfunc);
		
	return true;
}
bool stopSQGlue() {
	sq_close(g_sqvm);
	g_sqvm = NULL;
	return true;
}

bool sqGlue_compile(const char* script, const char* sourcename) {
	return SQ_SUCCEEDED(sq_compilebuffer(g_sqvm, script, scstrlen(script), sourcename, SQTrue));
}

bool callSqGlueFunctionNoParam(const char* name) {
	return SQ_SUCCEEDED(callSqFunctionNoParam(g_sqvm, name));
}

bool callSqGlueFunctionString(const char* name, const char* param) {
	return SQ_SUCCEEDED(callSqFunctionString(g_sqvm, name, param));
}
int  callSqGlueFunctionNoParam_Int(const char* name, int defaultValue) {
	return callSqFunctionNoParam_Int(g_sqvm, name, defaultValue);
}
bool callSqGlueFunctionNoParam_Bool(const char* name, bool defaultValue) {
	return callSqFunctionNoParam_Bool(g_sqvm, name, defaultValue);
}
int  callSqGlueFunctionInt_Int(const char* name, int param, int defaultValue) {
	return callSqFunctionInt_Int(g_sqvm, name, param, defaultValue);
}
