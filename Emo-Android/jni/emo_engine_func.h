#include <squirrel.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

void LOGI(const SQChar* msg);
void LOGW(const SQChar* msg);
void LOGE(const SQChar* msg);

bool loadPngSizeFromAsset(const char *fname, int *width, int *height);
bool loadPngFromAsset(const char* fname, struct ImageInfo* imageInfo);

void emoUpdateOptions(SQInteger value);
bool loadScriptFromAsset(const char* fname);

SQInteger emoImportScript(HSQUIRRELVM v);
SQInteger emoSetOptions(HSQUIRRELVM v);
SQInteger emoRuntimeEcho(HSQUIRRELVM v);
SQInteger emoRuntimeLog(HSQUIRRELVM v);
SQInteger emoRuntimeLogInfo(HSQUIRRELVM v);
SQInteger emoRuntimeLogError(HSQUIRRELVM v);
SQInteger emoRuntimeLogWarn(HSQUIRRELVM v);
SQInteger emoRuntimeFinish(HSQUIRRELVM v);
SQInteger emoRuntimeGetOSName(HSQUIRRELVM v);

SQInteger emoRegisterSensors(HSQUIRRELVM v);
SQInteger emoEnableSensor(HSQUIRRELVM v);
SQInteger emoDisableSensor(HSQUIRRELVM v);
SQInteger emoEnableOnDrawCallback(HSQUIRRELVM v);
SQInteger emoDisableOnDrawCallback(HSQUIRRELVM v);

