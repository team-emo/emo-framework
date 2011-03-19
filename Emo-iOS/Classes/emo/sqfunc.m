#include <stdarg.h>
#include <stdio.h>

#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdaux.h>

#include <common.h>
#include <sqfunc.h>

/*
 * Logging
 */
extern void LOGI(const SQChar* msg);
extern void LOGW(const SQChar* msg);
extern void LOGE(const SQChar* msg);

void initSQVM(HSQUIRRELVM v) {
	sqstd_seterrorhandlers(v);
	sq_setprintfunc(v, sq_printfunc, sq_errorfunc);
}

SQInteger sqCompileBuffer(HSQUIRRELVM v, const char* script, const char* sourcename) {
	if (SQ_SUCCEEDED(sq_compilebuffer(v, script, scstrlen(script), sourcename, SQTrue))) {
		sq_pushroottable(v);
		if (SQ_FAILED(sq_call(v, 1, SQFalse, SQTrue))) {
			return ERR_SCRIPT_CALL_ROOT;
		}
	} else {
		return ERR_SCRIPT_COMPILE;
	}
	return EMO_NO_ERROR;
}

/*
 * Call Squirrel function with no parameter
 * Returns SQTrue if sq_call succeeds.
 */
SQBool callSqFunctionNoParam(HSQUIRRELVM v, const SQChar* name) {
	SQBool result = SQFalse;

	SQInteger top = sq_gettop(v);
	sq_pushroottable(v);
	sq_pushstring(v, name, -1);
	if(SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushroottable(v);
		result = SQ_SUCCEEDED(sq_call(v, 1, SQFalse, SQTrue));
	}
	sq_settop(v,top);

	return result;
}
/*
 * Call Squirrel function with one string parameter
 * Returns SQTrue if sq_call succeeds.
 */
SQBool callSqFunctionString(HSQUIRRELVM v, const SQChar* name, const SQChar* param) {
	SQBool result = SQFalse;

	SQInteger top = sq_gettop(v);
	sq_pushroottable(v);
	sq_pushstring(v, name, -1);
	if(SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushroottable(v);
		sq_pushstring(v, param, -1);
		result = SQ_SUCCEEDED(sq_call(v, 2, SQFalse, SQTrue));
	}
	sq_settop(v,top);

	return result;
}
/*
 * Call Squirrel function with no parameter, returns integer
 * Returns default value if sq_call failed.
 */
SQInteger callSqFunctionNoParam_Int(HSQUIRRELVM v, const SQChar* name, SQInteger defaultValue) {
	SQInteger result = defaultValue;

	SQInteger top = sq_gettop(v);
	sq_pushroottable(v);
	sq_pushstring(v, name, -1);
	if(SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushroottable(v);
		if (SQ_SUCCEEDED(sq_call(v, 1, SQTrue, SQTrue))) {
			sq_getinteger(v, sq_gettop(v), &result);
		}
	}
	sq_settop(v,top);

	return result;
}
/*
 * Call Squirrel function with no parameter, returns boolean
 * Returns the default value if sq_call failed.
 */
SQBool callSqFunctionNoParam_Bool(HSQUIRRELVM v, const SQChar* name, SQBool defaultValue) {
	SQBool result = defaultValue;

	SQInteger top = sq_gettop(v);
	sq_pushroottable(v);
	sq_pushstring(v, name, -1);
	if(SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushroottable(v);
		if (SQ_SUCCEEDED(sq_call(v, 1, SQTrue, SQTrue))) {
			sq_getbool(v, sq_gettop(v), &result);
		}
	}
	sq_settop(v,top);

	return result;
}

/*
 * Call Squirrel function with one integer parameter, returns integer
 * Returns default value if sq_call failed.
 */
SQInteger callSqFunctionInt_Int(HSQUIRRELVM v, const SQChar* name, SQInteger param, SQInteger defaultValue) {
	SQInteger result = defaultValue;

	SQInteger top = sq_gettop(v);
	sq_pushroottable(v);
	sq_pushstring(v, name, -1);
	if(SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushroottable(v);
		sq_pushinteger(v, param);
		if (SQ_SUCCEEDED(sq_call(v, 2, SQTrue, SQTrue))) {
			sq_getinteger(v, sq_gettop(v), &result);
		}
	}
	sq_settop(v,top);

	return result;
}
/*
 * print function
 */
void sq_printfunc(HSQUIRRELVM v, const SQChar *s,...) {
	static SQChar text[2048];
	va_list args;
    va_start(args, s);
    scvsprintf(text, s, args);
    va_end(args);

    LOGI(text);
}
/*
 * error function
 */
void sq_errorfunc(HSQUIRRELVM v, const SQChar *s,...) {
	static SQChar text[2048];
	va_list args;
    va_start(args, s);
    scvsprintf(text, s, args);
    va_end(args);

    callSqFunctionString(v, "onError", text);

    LOGW(text);
}
/*
 * Register global function
 * Must be called before loading script files
 */
void register_global_func(HSQUIRRELVM v, SQFUNCTION f, const char *fname) {
    sq_pushroottable(v);
    sq_pushstring(v, fname, -1);
    sq_newclosure(v, f, 0);
    sq_createslot(v, -3);
    sq_pop(v,1);
}
