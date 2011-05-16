#ifndef EMO_RUNTIME_H
#define EMO_RUNTIME_H

#include <android_native_app_glue.h>
#include <squirrel.h>

void initRuntimeFunctions();

int32_t app_handle_input(struct android_app* app, AInputEvent* event);
void app_handle_cmd(struct android_app* app, int32_t cmd);

void LOGI(const char* msg);
void LOGW(const char* msg);
void LOGE(const char* msg);

void clearGLErrors(const char* msg);
bool printGLErrors(const char* msg);

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
SQInteger emoRuntimeStopwatchStart(HSQUIRRELVM v);
SQInteger emoRuntimeStopwatchStop(HSQUIRRELVM v);
SQInteger emoRuntimeStopwatchElapsed(HSQUIRRELVM v);

SQInteger emoRegisterSensors(HSQUIRRELVM v);
SQInteger emoEnableSensor(HSQUIRRELVM v);
SQInteger emoDisableSensor(HSQUIRRELVM v);
SQInteger emoEnableOnDrawCallback(HSQUIRRELVM v);
SQInteger emoDisableOnDrawCallback(HSQUIRRELVM v);

SQInteger emoEnableOnUpdateCallback(HSQUIRRELVM v);
SQInteger emoDisableOnUpdateCallback(HSQUIRRELVM v);
SQInteger emoEnableOnFpsCallback(HSQUIRRELVM v);
SQInteger emoDisableOnFpsCallback(HSQUIRRELVM v);
SQInteger emoRuntimeGC(HSQUIRRELVM v);
#endif
