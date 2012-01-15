// Copyright (c) 2011 emo-framework project
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the project nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
#ifndef EMO_RUNTIME_H
#define EMO_RUNTIME_H

#include <string>
#include <../native_app_glue.h>
#include <squirrel.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

void initRuntimeFunctions();

int32_t app_handle_input(struct android_app* app, AInputEvent* event);
void app_handle_cmd(struct android_app* app, int32_t cmd);

void LOGI(const char* msg);
void LOGW(const char* msg);
void LOGE(const char* msg);

void clearGLErrors(const char* msg);
bool printGLErrors(const char* msg);

void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
               GLfloat centerx, GLfloat centery, GLfloat centerz,
               GLfloat upx, GLfloat upy, GLfloat upz);

void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);

void emoUpdateOptions(SQInteger value);
bool loadScriptFromAsset(const char* fname);
bool loadScriptFromDocument(const char* fname);
bool loadScript(const char* fname);

bool endsWith(std::string const &fullString, std::string const &ending);
std::string loadContentFromAsset(std::string fname);

SQInteger emoImportScript(HSQUIRRELVM v);
SQInteger emoSetOptions(HSQUIRRELVM v);
SQInteger emoRuntimeEcho(HSQUIRRELVM v);
SQInteger emoRuntimeLog(HSQUIRRELVM v);
SQInteger emoRuntimeLogInfo(HSQUIRRELVM v);
SQInteger emoRuntimeLogError(HSQUIRRELVM v);
SQInteger emoRuntimeLogWarn(HSQUIRRELVM v);
SQInteger emoRuntimeFinish(HSQUIRRELVM v);
SQInteger emoRuntimeGetOSName(HSQUIRRELVM v);
SQInteger emoRuntimeGetDeviceName(HSQUIRRELVM v);
SQInteger emoRuntimeIsSimulator(HSQUIRRELVM v);
SQInteger emoRuntimeStopwatchStart(HSQUIRRELVM v);
SQInteger emoRuntimeStopwatchStop(HSQUIRRELVM v);
SQInteger emoRuntimeStopwatchElapsed(HSQUIRRELVM v);
SQInteger emoRuntimeSetLogLevel(HSQUIRRELVM v);
SQInteger emoClearImageCache(HSQUIRRELVM v);
#ifndef EMO_WITH_SANDBOX
SQInteger emoRuntimeCompileBuffer(HSQUIRRELVM v);
SQInteger emoRuntimeCompile(HSQUIRRELVM v);
#endif
SQInteger emoRuntimeGetDocumentDir(HSQUIRRELVM v);
SQInteger emoRuntimeIsSandboxEnabled(HSQUIRRELVM v);

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
SQInteger emoRuntimeBuildNumber(HSQUIRRELVM v);
SQInteger emoEnableSimpleLog(HSQUIRRELVM v);
SQInteger emoEnableSimpleLogWithLevel(HSQUIRRELVM v);
SQInteger emoRuntimeNativeRandom(HSQUIRRELVM v);
SQInteger emoRuntimeRandom(HSQUIRRELVM v);
SQInteger emoRuntimeAndroidToast(HSQUIRRELVM v);
SQInteger emoGetDefaultLocale(HSQUIRRELVM v);
SQInteger emoRuntimeAndroidGetOSVersion(HSQUIRRELVM v);
SQInteger emoRuntimeAndroidGetSDKVersion(HSQUIRRELVM v);
SQInteger emoRuntimeAndroidGetNativeOrientation(HSQUIRRELVM v);
#endif
