// Copyright (c) 2011 emo-framework project
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the project nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
#if __cplusplus
extern "C" {
#endif
	
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

#include <Constants.h>
#include <VmFunc.h>

extern void LOGI(const char* msg);
extern void LOGW(const char* msg);
extern void LOGE(const char* msg);

/*
 * Initialize Squirrel VM
 */
void initSQVM(HSQUIRRELVM v) {
	sqstd_seterrorhandlers(v);
	sq_setprintfunc(v, sq_printfunc, sq_errorfunc);
	sq_pushroottable(v);
	sqstd_register_systemlib(v);
#ifndef EMO_WITH_SANDBOX
	sqstd_register_iolib(v);
#endif
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
SQBool callSqFunction_Bool_String(HSQUIRRELVM v, const SQChar* nname, const SQChar* name, const SQChar* value, SQBool defaultValue) {
	SQBool result = defaultValue;

	SQInteger top = sq_gettop(v);
	sq_pushroottable(v);
	sq_pushstring(v, nname, -1);
	if (SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushstring(v, name, -1);
		if(SQ_SUCCEEDED(sq_get(v, -2))) {
			sq_pushroottable(v);
			sq_pushstring(v, value, -1);
			result = SQ_SUCCEEDED(sq_call(v, 2, SQFalse, SQTrue));
		}
	}
	sq_settop(v,top);

	return result;
}

/*
 * Call Squirrel function with strings parameter
 * Returns SQTrue if sq_call succeeds.
 */
SQBool callSqFunction_Bool_Strings(HSQUIRRELVM v, const SQChar* nname, const SQChar* name, 
			const SQChar* value1, const SQChar* value2,
			const SQChar* value3, const SQChar* value4, SQBool defaultValue) {
	SQBool result = defaultValue;
	SQInteger top = sq_gettop(v);
	sq_pushroottable(v);
	sq_pushstring(v, nname, -1);
	if (SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushstring(v, name, -1);
		if(SQ_SUCCEEDED(sq_get(v, -2))) {
			sq_pushroottable(v);
			sq_pushstring(v, value1, -1);
			sq_pushstring(v, value2, -1);
			sq_pushstring(v, value3, -1);
			sq_pushstring(v, value4, -1);
			result = SQ_SUCCEEDED(sq_call(v, 5, SQFalse, SQTrue));
		}
	}
	sq_settop(v,top);
	
	return result;
}
	
/*
 * Call Squirrel function with one float parameter
 * Returns SQTrue if sq_call succeeds.
 */
SQBool callSqFunction_Bool_Float(HSQUIRRELVM v, const SQChar* nname, const SQChar* name, SQFloat value, SQBool defaultValue) {
	SQBool   result = SQFalse;

	SQInteger top = sq_gettop(v);
	sq_pushroottable(v);
	sq_pushstring(v, nname, -1);
	if (SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pushstring(v, name, -1);
		if(SQ_SUCCEEDED(sq_get(v, -2))) {
			sq_pushroottable(v);
			sq_pushfloat(v, value);
			if (SQ_SUCCEEDED(sq_call(v, 2, SQTrue, SQTrue))) {
				sq_getbool(v, sq_gettop(v), &result);
			}
		}
	}
	sq_settop(v,top);

	return result;
}

/*
 * print function
 */
void sq_printfunc(HSQUIRRELVM v, const SQChar *s,...) {
	va_list args;
    va_start(args, s);
    SQChar* str = va_arg(args, SQChar*);
    LOGI(str);
    va_end(args);
}

/*
 * error function
 */
void sq_errorfunc(HSQUIRRELVM v, const SQChar *s,...) {
	va_list args;
    va_start(args, s);
    SQChar* str = va_arg(args, SQChar*);
    callSqFunction_Bool_String(v, EMO_NAMESPACE, EMO_FUNC_ONERROR, str, SQFalse);
    LOGE(str);
    va_end(args);
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
	
/*
 * Register new class for emo framework
 */
void registerClass(HSQUIRRELVM v, const char *cname) {
	register_class_with_namespace(v, EMO_NAMESPACE, cname);
}
	
/*
 * Register class method for emo framework
 */
void registerClassFunc(HSQUIRRELVM v, const char *cname, const char *fname, SQFUNCTION func) {
	register_class_func_with_namespace(
				   v, EMO_NAMESPACE, cname, fname, func);
}

/*
 * get boolean value from the stack.
 * this function accepts both integer(1,0) and bool(true,false).
 * returns true if succeeds.
 */
bool getBool(HSQUIRRELVM v, int idx, SQBool* value) {
	if (sq_gettype(v, idx) == OT_INTEGER) {
		SQInteger val;
		sq_getinteger(v, idx, &val);
		*value = (val == 1);
	} else if (sq_gettype(v, idx) == OT_BOOL) {
		sq_getbool(v, idx, value);
	} else {
		return false;
	}
	
	return true;
}

/*
 * get instance member value as float
 */
bool getInstanceMemberAsInteger(HSQUIRRELVM v, int idx, const char *name, SQInteger* value) {
	if (sq_gettype(v, idx) == OT_NULL) return false;
	sq_pushstring(v, name, -1);
	if (!SQ_SUCCEEDED(sq_get(v, idx))) {
		sq_pop(v, 1);
		return false;
	}
	if (sq_gettype(v, -1) == OT_NULL) {
		return false;
		sq_pop(v, 1);
	}
	sq_getinteger(v, -1, value);
	sq_pop(v, 1);
	
	return true;
}

/*
 * get instance member value as float
 */
bool getInstanceMemberAsFloat(HSQUIRRELVM v, int idx, const char *name, SQFloat* value) {
	if (sq_gettype(v, idx) == OT_NULL) return false;
	sq_pushstring(v, name, -1);
	if (!SQ_SUCCEEDED(sq_get(v, idx))) {
		sq_pop(v, 1);
		return false;
	}
	if (sq_gettype(v, -1) == OT_NULL) {
		sq_pop(v, 1);
		return false;
	}
	sq_getfloat(v, -1, value);
	sq_pop(v, 1);
	
	return true;
}
	
/*
 * get instance member value as bool
 * this function accepts both integer(1,0) and bool(true,false).
 */
bool getInstanceMemberAsBool(HSQUIRRELVM v, int idx, const char *name, bool* value) {
	if (sq_gettype(v, idx) == OT_NULL) return false;
	sq_pushstring(v, name, -1);
	if (!SQ_SUCCEEDED(sq_get(v, idx))) {
		sq_pop(v, 1);
		return false;
	}
	
	SQBool flag;
	if (!getBool(v, -1, &flag)) {
		sq_pop(v, 1);
		return false;
	}
	
	*value = flag;
	
	sq_pop(v, 1);
	return true;
}

/*
 * get instance member value as table
 */
bool getInstanceMemberAsTable(HSQUIRRELVM v, int idx, const char *cname, const char *name, SQFloat* value) {
	if (sq_gettype(v, idx) == OT_NULL) return false;
	sq_pushstring(v, cname, -1);
	if (!SQ_SUCCEEDED(sq_get(v, idx))) {
		sq_pop(v, 1);
		return false;
	}
	sq_pushstring(v, name, -1);
	if (!SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pop(v, 1);
		return false;
	}
	if (sq_gettype(v, -1) == OT_NULL) {
		sq_pop(v, 1);
		return false;
	}
	sq_getfloat(v, -1, value);
	sq_pop(v, 1);
	return true;
}

/*
 * get instance member value as user pointer
 */
bool getInstanceMemberAsUserPointer(HSQUIRRELVM v, int idx, 
					const char *cname, const char *name, SQUserPointer* value) {
	if (sq_gettype(v, idx) == OT_NULL) return false;
	sq_pushstring(v, cname, -1);
	if (!SQ_SUCCEEDED(sq_get(v, idx))) {
		sq_pop(v, 1);
		return false;
	}
	sq_pushstring(v, name, -1);
	if (!SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pop(v, 2);
		return false;
	}
	if (sq_gettype(v, -1) != OT_USERPOINTER) {
		sq_pop(v, 2);
		return false;
	}
	sq_getuserpointer(v, -1, value);
	sq_pop(v, 2);
	return true;
}
	
/*
 * get instance member pointer from instance
 */
bool getInstanceMemberAsInstance(HSQUIRRELVM v, int idx, 
			const char *cname, const char *name, SQUserPointer* value) {
	if (sq_gettype(v, idx) == OT_NULL) return false;
	sq_pushstring(v, cname, -1);
	if (!SQ_SUCCEEDED(sq_get(v, idx))) {
		sq_pop(v, 1);
		return false;
	}
	sq_pushstring(v, name, -1);
	if (!SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pop(v, 2);
		return false;
	}
	if (sq_gettype(v, -1) != OT_INSTANCE) {
		sq_pop(v, 2);
		return false;
	}
	sq_getinstanceup(v, -1, value, 0);
	sq_pop(v, 2);
	return true;
}

SQInteger createSQObject(HSQUIRRELVM v, 
				const char* package_name, const char* name,
				SQUserPointer ptr, SQRELEASEHOOK releaseHook) {
	sq_pushroottable(v);
	
	sq_pushstring(v, package_name, -1);
	if (!SQ_SUCCEEDED(sq_get(v, -2))) {
		sq_pop(v, 1);
		return 0;
	}
	if (name != NULL) {
		sq_pushstring(v, name, -1);
		if (!SQ_SUCCEEDED(sq_get(v, -2))) {
			sq_pop(v, 2);
			return 0;
		}
	}
	
	sq_createinstance(v, -1);
	sq_setinstanceup(v, -1, ptr);
	if (releaseHook != NULL) {
		sq_setreleasehook(v, -1, releaseHook);
	}
	sq_remove(v, -2);
	sq_remove(v, -2);

	return 1;
}
#if __cplusplus
}   // Extern C
#endif

