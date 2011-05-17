#include <squirrel.h>

void initSQVM(HSQUIRRELVM v); 

SQInteger sqCompileBuffer(HSQUIRRELVM v, const char* script, const char* sourcename);

SQBool callSqFunction(HSQUIRRELVM v, const char* nname, const char* name);
SQBool callSqFunction_Bool_Floats(HSQUIRRELVM v, const char* nname, const char* name, float param[], int count, SQBool defaultValue);
SQBool callSqFunction_Bool_String(HSQUIRRELVM v, const char* nname, const char* name, const SQChar* value, SQBool defaultValue);
SQBool callSqFunction_Bool_Float(HSQUIRRELVM v, const char* nname, const char* name, SQFloat value, SQBool defaultValue);
SQBool callSqFunction_Bool_TwoStrings(HSQUIRRELVM v, const SQChar* nname, const SQChar* name, const SQChar* value1, const SQChar* value2, SQBool defaultValue);

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
bool getInstanceMemberAsFloat(HSQUIRRELVM v, int idx, const char *name, SQFloat* value);
bool getInstanceMemberAsBool(HSQUIRRELVM v, int idx, const char *name, bool* value);
bool getInstanceMemberAsTable(HSQUIRRELVM v, int idx, const char *cname, const char *name, SQFloat* value);
bool getInstanceMemberAsUserPointer(HSQUIRRELVM v, int idx, const char *cname, const char *name, SQUserPointer* value);
SQInteger createSQObject(HSQUIRRELVM v, const char* package_name, const char* name, SQUserPointer ptr, SQRELEASEHOOK releaseHook);
