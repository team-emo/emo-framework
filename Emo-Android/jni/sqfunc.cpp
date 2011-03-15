#include <stdio.h>
#include <android/asset_manager.h>

#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdaux.h>

#include <common.h>
#include <sqfunc.h>

/*
 * Squirrel Basic Functions
 */

/*
 * Read script callback
 */
SQInteger sq_lexer(SQUserPointer asset) {
	SQChar c;
		if(AAsset_read((AAsset*)asset, &c, 1) > 0) {
			return c;
		}
	return 0;
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
void sq_printfunc(HSQUIRRELVM v,const SQChar *s,...) {
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
void sq_errorfunc(HSQUIRRELVM v,const SQChar *s,...) {
	static SQChar text[2048];
	va_list args;
    va_start(args, s);
    scvsprintf(text, s, args);
    va_end(args);

    callSqFunctionString(v, "onError", text);

    LOGW(text);
}
