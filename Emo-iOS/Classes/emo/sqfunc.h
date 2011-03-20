void initSQVM(HSQUIRRELVM v); 

SQInteger sqCompileBuffer(HSQUIRRELVM v, const char* script, const char* sourcename);

SQBool callSqFunction(HSQUIRRELVM v, const SQChar* name);
SQInteger callSqFunction_Int(HSQUIRRELVM v, const SQChar* name, SQInteger defaultValue);
SQBool callSqFunction_Bool(HSQUIRRELVM v, const SQChar* name, SQBool defaultValue);
SQBool callSqFunction_Bool_Floats(HSQUIRRELVM v, const SQChar* name, SQFloat param[], SQBool defaultValue);
SQInteger callSqFunction_Int_Int(HSQUIRRELVM v, const SQChar* name, SQInteger param, SQInteger defaultValue);
SQBool callSqFunction_Bool_String(HSQUIRRELVM v, const SQChar* name, const SQChar* value);
SQBool callSqFunction_Bool_Int(HSQUIRRELVM v, const SQChar* name, SQInteger value);
SQBool callSqFunction_Bool_Bool(HSQUIRRELVM v, const SQChar* name, SQBool value);

void sq_printfunc(HSQUIRRELVM v,const char *s,...);
void sq_errorfunc(HSQUIRRELVM v,const char *s,...);
void register_global_func(HSQUIRRELVM v, SQFUNCTION f, const char *fname);
