void initSQVM(HSQUIRRELVM v); 

SQInteger sqCompileBuffer(HSQUIRRELVM v, const char* script, const char* sourcename);

SQBool callSqFunctionNoParam(HSQUIRRELVM v, const char* name);
SQBool callSqFunctionString(HSQUIRRELVM v, const char* name, const char* param);
SQInteger callSqFunctionNoParam_Int(HSQUIRRELVM v, const char* name, SQInteger defaultValue);
SQBool callSqFunctionNoParam_Bool(HSQUIRRELVM v, const char* name, SQBool defaultValue);
SQInteger callSqFunctionInt_Int(HSQUIRRELVM v, const char* name, SQInteger param, SQInteger defaultValue);

void sq_printfunc(HSQUIRRELVM v,const char *s,...);
void sq_errorfunc(HSQUIRRELVM v,const char *s,...);
void register_global_func(HSQUIRRELVM v, SQFUNCTION f, const char *fname);
