#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdaux.h>
#include <sqstdblob.h>
#include <sqstdio.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdsystem.h>

#include <common.h>
#include <sqfunc.h>

/*
 * Logging
 */
extern void LOGI(const SQChar* msg);
extern void LOGW(const SQChar* msg);
extern void LOGE(const SQChar* msg);

/*
 * Initialize Squirrel VM
 */
void initSQVM(HSQUIRRELVM v) {
	sqstd_seterrorhandlers(v);
	sq_setprintfunc(v, sq_printfunc, sq_errorfunc);
	sq_pushroottable(v);
	sqstd_register_systemlib(v);
	sqstd_register_iolib(v);
	sqstd_register_bloblib(v);
	sqstd_register_mathlib(v);
	sqstd_register_stringlib(v);
}

/*
 * Comple script buffers
 */
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
SQBool callSqFunction(HSQUIRRELVM v, const SQChar* nname, const SQChar* name) {
	SQBool result = SQFalse;

	SQInteger top = sq_gettop(v);
	sq_pushroottable(v);
	sq_pushstring(v, nname, -1);
	if (SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushstring(v, name, -1);
		if(SQ_SUCCEEDED(sq_get(v, -2))) {
			sq_pushroottable(v);
			result = SQ_SUCCEEDED(sq_call(v, 1, SQFalse, SQTrue));
		}
		sq_settop(v,top);
	}
	
	return result;
}

/*
 * Call Squirrel function with multiple float parameters, returns boolean
 * Returns default value if sq_call failed.
 */
SQBool callSqFunction_Bool_Floats(HSQUIRRELVM v, const SQChar* nname, const SQChar* name, SQFloat param[], int count, SQBool defaultValue) {
	SQBool   result = defaultValue;

	SQInteger top = sq_gettop(v);
	sq_pushroottable(v);
	sq_pushstring(v, nname, -1);
	if (SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushstring(v, name, -1);
		if(SQ_SUCCEEDED(sq_get(v, -2))) {
			sq_pushroottable(v);
			for (int i = 0; i < count; i++) {
				sq_pushfloat(v, param[i]);
			}
			if (SQ_SUCCEEDED(sq_call(v, count + 1, SQTrue, SQTrue))) {
				sq_getbool(v, sq_gettop(v), &result);
			}
		}
	}
	sq_settop(v,top);

	return result;
}

/*
 * Call Squirrel function with one string parameter
 * Returns SQTrue if sq_call succeeds.
 */
SQBool callSqFunction_Bool_String(HSQUIRRELVM v, const SQChar* nname, const SQChar* name, const SQChar* value) {
	SQBool result = SQFalse;

	SQInteger top = sq_gettop(v);
	sq_pushroottable(v);
	sq_pushstring(v, name, -1);
	if(SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushroottable(v);
		sq_pushstring(v, value, -1);
		result = SQ_SUCCEEDED(sq_call(v, 2, SQFalse, SQTrue));
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

    callSqFunction_Bool_String(v, EMO_NAMESPACE, EMO_FUNC_ONERROR, text);

    LOGE(text);
}
/*
 * Register global function.
 * Must be called before loading script files
 */
void register_global_func(HSQUIRRELVM v, SQFUNCTION func, const char *fname) {
    sq_pushroottable(v);
    sq_pushstring(v, fname, -1);
    sq_newclosure(v, func, 0);
    sq_createslot(v, -3);
    sq_pop(v,1);
}

/*
 * Register new class.
 * Must be called before loading script files
 */
void register_class(HSQUIRRELVM v, const char *cname) {
    sq_pushroottable(v);
    sq_pushstring(v, cname, -1);
    sq_newclass(v, false);
    sq_createslot(v, -3);
    sq_pop(v, 1);
}

/*
 * Register new class with namespace.
 * Must be called before loading script files
 */
void register_class_with_namespace(HSQUIRRELVM v, const char *nname, const char *cname) {
    sq_pushroottable(v);
    sq_pushstring(v, nname, -1);
    if(SQ_SUCCEEDED(sq_get(v, -2))) {
        sq_pushstring(v, cname, -1);
        sq_newclass(v, false);
        sq_createslot(v, -3);
    }
    sq_pop(v, 1);
}

/*
 * Register class method.
 * Must be called before loading script files
 */
void register_class_func(HSQUIRRELVM v, const char* cname, const char* fname, SQFUNCTION func) {
    sq_pushroottable(v);
    sq_pushstring(v, cname, -1);
    if(SQ_SUCCEEDED(sq_get(v, -2))) {
        sq_pushstring(v, fname, -1);
        sq_newclosure(v, func, 0);
        sq_newslot(v, -3, true);
    }
    sq_pop(v, 1);
}

/*
 * Register class method with namespace
 * Must be called before loading script files
 */
void register_class_func_with_namespace(HSQUIRRELVM v, const char* nname, const char* cname, const char* fname, SQFUNCTION func) {
    sq_pushroottable(v);
    sq_pushstring(v, nname, -1);
    if(SQ_SUCCEEDED(sq_get(v, -2))) {
    	sq_pushstring(v, cname, -1);
    	if(SQ_SUCCEEDED(sq_get(v, -2))) {
    		sq_pushstring(v, fname, -1);
    		sq_newclosure(v, func, 0);
    		sq_newslot(v, -3, true);
    	}
    }
    sq_pop(v, 1);
}

/*
 * Register new table.
 * Must be called before loading script files
 */
void register_table(HSQUIRRELVM v, const char *name) {
    sq_pushroottable(v);
    sq_pushstring(v, name, -1);
    sq_newtable(v);
    sq_createslot(v, -3);
    sq_pop(v, 1);
}
