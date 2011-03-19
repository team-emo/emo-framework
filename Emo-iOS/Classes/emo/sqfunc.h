void initSQVM(HSQUIRRELVM v); 
SQInteger sqCompileBuffer(HSQUIRRELVM v, const char* script, const char* sourcename);

SQBool callSqFunctionNoParam(HSQUIRRELVM v, const SQChar* name);
SQBool callSqFunctionString(HSQUIRRELVM v, const SQChar* name, const SQChar* param);
SQInteger callSqFunctionNoParam_Int(HSQUIRRELVM v, const SQChar* name, SQInteger defaultValue);
SQBool callSqFunctionNoParam_Bool(HSQUIRRELVM v, const SQChar* name, SQBool defaultValue);
SQInteger callSqFunctionInt_Int(HSQUIRRELVM v, const SQChar* name, SQInteger param, SQInteger defaultValue);

void sq_printfunc(HSQUIRRELVM v,const SQChar *s,...);
void sq_errorfunc(HSQUIRRELVM v,const SQChar *s,...);
void register_global_func(HSQUIRRELVM v, SQFUNCTION f, const char *fname);
