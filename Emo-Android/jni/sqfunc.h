#include <GLES/gl.h>

SQInteger sq_lexer(SQUserPointer asset);
SQBool callSqFunctionNoParam(HSQUIRRELVM v, const SQChar* name);
SQBool callSqFunctionString(HSQUIRRELVM v, const SQChar* name, const SQChar* param);
SQInteger callSqFunctionNoParam_Int(HSQUIRRELVM v, const SQChar* name);
SQInteger callSqFunctionInt_Int(HSQUIRRELVM v, const SQChar* name, SQInteger param);
void sq_printfunc(HSQUIRRELVM v,const SQChar *s,...);
void sq_errorfunc(HSQUIRRELVM v,const SQChar *s,...);
