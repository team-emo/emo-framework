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
#include <squirrel.h>

void initSQVM(HSQUIRRELVM v); 

SQInteger sqCompileBuffer(HSQUIRRELVM v, const char* script, const char* sourcename);

SQBool callSqFunction(HSQUIRRELVM v, const char* nname, const char* name);
SQBool callSqFunction_Bool_Floats(HSQUIRRELVM v, const char* nname, const char* name, float param[], int count, SQBool defaultValue);
SQBool callSqFunction_Bool_String(HSQUIRRELVM v, const char* nname, const char* name, const SQChar* value, SQBool defaultValue);
SQBool callSqFunction_Bool_Float(HSQUIRRELVM v, const char* nname, const char* name, SQFloat value, SQBool defaultValue);
SQBool callSqFunction_Bool_Strings(HSQUIRRELVM v, const SQChar* nname, const SQChar* name,
	const SQChar* value1, const SQChar* value2, const SQChar* value3, const SQChar* value4, SQBool defaultValue);
void sq_printfunc(HSQUIRRELVM v,const char *s,...);
void sq_errorfunc(HSQUIRRELVM v,const char *s,...);
void register_global_func(HSQUIRRELVM v, SQFUNCTION f, const char *fname);
void register_class(HSQUIRRELVM v, const char *cname);
void register_class_with_namespace(HSQUIRRELVM v, const char *nname, const char *cname);
void register_class_func(HSQUIRRELVM v, const char* cname, const char* fname, SQFUNCTION func);
void register_class_func_with_namespace(HSQUIRRELVM v, const char* nname, const char* cname, const char* fname, SQFUNCTION func);
void register_table(HSQUIRRELVM v, const char *name);
void registerClass(HSQUIRRELVM v, const char *cname);
void registerClassFunc(HSQUIRRELVM v, const char *cname, const char *fname, SQFUNCTION func);
bool getInstanceMemberAsInteger(HSQUIRRELVM v, int idx, const char *name, SQInteger* value);
bool getInstanceMemberAsFloat(HSQUIRRELVM v, int idx, const char *name, SQFloat* value);
bool getInstanceMemberAsBool(HSQUIRRELVM v, int idx, const char *name, bool* value);
bool getInstanceMemberAsTable(HSQUIRRELVM v, int idx, const char *cname, const char *name, SQFloat* value);
bool getInstanceMemberAsUserPointer(HSQUIRRELVM v, int idx, const char *cname, const char *name, SQUserPointer* value);
bool getInstanceMemberAsInstance(HSQUIRRELVM v, int idx, const char *cname, const char *name, SQUserPointer* value);
bool getBool(HSQUIRRELVM v, int idx, SQBool* value);
SQInteger createSQObject(HSQUIRRELVM v, const char* package_name, const char* name, SQUserPointer ptr, SQRELEASEHOOK releaseHook);
