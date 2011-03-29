#include <squirrel.h>

void LOGI(const SQChar* msg);
void LOGW(const SQChar* msg);
void LOGE(const SQChar* msg);

SQInteger emoImportScript(HSQUIRRELVM v);
SQInteger emoSetOptions(HSQUIRRELVM v);
SQInteger emoRuntimeEcho(HSQUIRRELVM v);
SQInteger emoRuntimeLog(HSQUIRRELVM v);
SQInteger emoRuntimeLogInfo(HSQUIRRELVM v);
SQInteger emoRuntimeLogError(HSQUIRRELVM v);
SQInteger emoRuntimeLogWarn(HSQUIRRELVM v);
SQInteger emoRuntimeShutdown(HSQUIRRELVM v);

SQInteger emoRegisterSensors(HSQUIRRELVM v);
SQInteger emoEnableSensor(HSQUIRRELVM v);
SQInteger emoDisableSensor(HSQUIRRELVM v);
SQInteger emoEnableOnDrawCallback(HSQUIRRELVM v);
SQInteger emoDisableOnDrawCallback(HSQUIRRELVM v);

void emoUpdateOptions(SQInteger value);
SQBool loadScriptFromAsset(const char* fname);

