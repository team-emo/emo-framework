
SQInteger sq_lexer(SQUserPointer asset);
SQBool callSqFunctionNoParam(HSQUIRRELVM v, const SQChar* name);
SQBool callSqFunctionOneString(HSQUIRRELVM v, const SQChar* name, const SQChar* param);
void sq_printfunc(HSQUIRRELVM v,const SQChar *s,...);
void sq_errorfunc(HSQUIRRELVM v,const SQChar *s,...);

