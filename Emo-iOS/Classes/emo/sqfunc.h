#include <squirrel.h>

void initSQVM(HSQUIRRELVM v); 

SQInteger sqCompileBuffer(HSQUIRRELVM v, const char* script, const char* sourcename);

SQBool callSqFunction(HSQUIRRELVM v, const char* name);
SQInteger callSqFunction_Int(HSQUIRRELVM v, const char* name, int defaultValue);
SQBool callSqFunction_Bool(HSQUIRRELVM v, const char* name, SQBool defaultValue);
SQBool callSqFunction_Bool_Floats(HSQUIRRELVM v, const char* name, float param[], int count, SQBool defaultValue);
SQInteger callSqFunction_Int_Int(HSQUIRRELVM v, const char* name, int param, int defaultValue);
SQBool callSqFunction_Bool_String(HSQUIRRELVM v, const char* name, const char* value);
SQBool callSqFunction_Bool_Int(HSQUIRRELVM v, const char* name, int value);
SQBool callSqFunction_Bool_Bool(HSQUIRRELVM v, const char* name, SQBool value);

void sq_printfunc(HSQUIRRELVM v,const char *s,...);
void sq_errorfunc(HSQUIRRELVM v,const char *s,...);
void register_global_func(HSQUIRRELVM v, SQFUNCTION f, const char *fname);
void register_class(HSQUIRRELVM v, const char *cname);
void register_class_with_namespace(HSQUIRRELVM v, const char *nname, const char *cname);
void register_class_func(HSQUIRRELVM v, const char* cname, const char* fname, SQFUNCTION func);
void register_class_func_with_namespace(HSQUIRRELVM v, const char* nname, const char* cname, const char* fname, SQFUNCTION func);
void register_table(HSQUIRRELVM v, const char *name);
