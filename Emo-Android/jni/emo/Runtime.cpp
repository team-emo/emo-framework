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
#include <math.h>

#include <android/log.h>
#include <android/native_activity.h>
#include <../native_app_glue.h>
#include <android/window.h>
#include <android/configuration.h>

#include "Constants.h"
#include "Runtime.h"
#include "Engine.h"
#include "VmFunc.h"
#include "Util.h"

extern emo::Engine* engine;

namespace emo {

Android::Android() {
    androidTable = new Sqrat::Table(engine->sqvm);
    engine->emoSpace->Bind("ANDROID", *androidTable);

    extraPostTable = new Sqrat::Table(engine->sqvm);
    androidTable->Bind("EXTRA_POST", *extraPostTable);

}

void Android::toast(std::string text) {
    int duration = ANDROID_TOAST_SHORT;
    engine->javaGlue->callStringInt_Void("toast", text, duration);
}

std::string Android::getOSVersion() {
    return engine->javaGlue->getOSVersion();
}

int Android::getSDKVersion() {
    return engine->javaGlue->getSDKVersion();
}

int Android::getNativeOrientation() {
    return engine->nativeOrientation;
}

void Android::transit(Sqrat::Object intent, int requestCode = -1) {
    std::string targetClass = intent.GetSlot("className").Cast<std::string>();
    int optionFlag = intent.GetSlot("optionFlag").Cast<int>();
    kvs_t* extras = new kvs_t();
    Sqrat::Array extraArray = intent.GetSlot("extras");
    for (int i = 0, size = extraArray.GetSize(); i < size; i++) {
        std::string key = extraArray[i].GetSlot("key").Cast<std::string>();
        std::string value = extraArray[i].GetSlot("value").Cast<std::string>();
        extras->insert(make_pair(key, value));
    }
    engine->javaGlue->transit(targetClass, extras, optionFlag,requestCode);
}

}

void initRuntimeFunctions() {
    registerClass(engine->sqvm, EMO_RUNTIME_CLASS);
    registerClass(engine->sqvm, EMO_EVENT_CLASS);
    registerClass(engine->sqvm, EMO_STOPWATCH_CLASS);

    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "import",
            emoImportScript);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "setOptions",
            emoSetOptions);

    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "echo", emoRuntimeEcho);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "test", emoRuntimeTest);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "log", emoRuntimeLog);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "info",
            emoRuntimeLogInfo);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "error",
            emoRuntimeLogError);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "warn",
            emoRuntimeLogWarn);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "finish",
            emoRuntimeFinish);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "os",
            emoRuntimeGetOSName);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "device",
            emoRuntimeGetDeviceName);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "isSimulator",
            emoRuntimeIsSimulator);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "gc", emoRuntimeGC);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "clearTextureCache",
            emoClearImageCache);
    registerClassFunc(engine->sqvm, EMO_STOPWATCH_CLASS, "start",
            emoRuntimeStopwatchStart);
    registerClassFunc(engine->sqvm, EMO_STOPWATCH_CLASS, "restart",
                emoRuntimeStopwatchRestart);
    registerClassFunc(engine->sqvm, EMO_STOPWATCH_CLASS, "stop",
            emoRuntimeStopwatchStop);
    registerClassFunc(engine->sqvm, EMO_STOPWATCH_CLASS, "isStopped",
                emoRuntimeStopwatchIsStopped);
    registerClassFunc(engine->sqvm, EMO_STOPWATCH_CLASS, "elapsed",
            emoRuntimeStopwatchElapsed);
    registerClassFunc(engine->sqvm, EMO_STOPWATCH_CLASS, "realElapsed",
            emoRuntimeStopwatchRealElapsed);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "setLogLevel",
            emoRuntimeSetLogLevel);
#ifndef EMO_WITH_SANDBOX
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "compilebuffer",
            emoRuntimeCompileBuffer);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "compile",
            emoRuntimeCompile);
#endif
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "getDocumentDir",
            emoRuntimeGetDocumentDir);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "isSandboxEnabled",
            emoRuntimeIsSandboxEnabled);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "buildNumber",
            emoRuntimeBuildNumber);

    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS, "registerSensors",
            emoRegisterSensors);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS, "enableSensor",
            emoEnableSensor);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS, "disableSensor",
            emoDisableSensor);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS, "enableOnDrawCallback",
            emoEnableOnDrawCallback);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS, "disableOnDrawCallback",
            emoDisableOnDrawCallback);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS, "enableOnUpdateCallback",
            emoEnableOnUpdateCallback);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS, "disableOnUpdateCallback",
            emoDisableOnUpdateCallback);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS, "enableOnFpsCallback",
            emoEnableOnFpsCallback);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS, "disableOnFpsCallback",
            emoDisableOnFpsCallback);

    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "enableSimpleLog",
            emoEnableSimpleLog);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS,
            "enableSimpleLogWithLevel", emoEnableSimpleLogWithLevel);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "nativeRandom",
            emoRuntimeNativeRandom);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "random",
            emoRuntimeRandom);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "getDefaultLocale",
            emoGetDefaultLocale);

}

int32_t app_handle_input(struct android_app* app, AInputEvent* event) {
    return engine->event_handle_input(app, event);
}

void app_handle_cmd(struct android_app* app, int32_t cmd) {
    engine->event_handle_cmd(app, cmd);
}

/* Log INFO */
void LOGI(const char* msg) {
    if (engine->logLevel <= LOG_INFO) {
        ((void) __android_log_print(ANDROID_LOG_INFO, EMO_LOG_TAG, msg));
    }
}

/* Log WARN */
void LOGW(const char* msg) {
    if (engine->logLevel <= LOG_WARN) {
        ((void) __android_log_print(ANDROID_LOG_WARN, EMO_LOG_TAG, msg));
    }
}

/* Log ERROR */
void LOGE(const char* msg) {
    if (engine->logLevel <= LOG_ERROR) {
        ((void) __android_log_print(ANDROID_LOG_ERROR, EMO_LOG_TAG, msg));
    }
}

/*
 * clear all OpenGL errors
 */
void clearGLErrors(const char* msg) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        if (error != GL_NO_ERROR) {
            LOGI(msg);
            char str[128];
            sprintf(str, "err code=0x%x", error);
            LOGI(str);
        }
    }
}

/*
 * print all OpenGL errors
 */
bool printGLErrors(const char* msg) {
    bool result = true;
    for (GLint error = glGetError(); error; error = glGetError()) {
        if (error != GL_NO_ERROR) {
            LOGE(msg);
            char str[128];
            sprintf(str, "err code=0x%x", error);
            LOGE(str);
            result = false;
        }
    }
    return result;
}

void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx,
        GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy,
        GLfloat upz) {
    GLfloat m[16];
    GLfloat x[3], y[3], z[3];
    GLfloat mag;

    /* Make rotation matrix */

    /* Z vector */
    z[0] = eyex - centerx;
    z[1] = eyey - centery;
    z[2] = eyez - centerz;
    mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
    if (mag) { /* mpichler, 19950515 */
        z[0] /= mag;
        z[1] /= mag;
        z[2] /= mag;
    }

    /* Y vector */
    y[0] = upx;
    y[1] = upy;
    y[2] = upz;

    /* X vector = Y cross Z */
    x[0] = y[1] * z[2] - y[2] * z[1];
    x[1] = -y[0] * z[2] + y[2] * z[0];
    x[2] = y[0] * z[1] - y[1] * z[0];

    /* Recompute Y = Z cross X */
    y[0] = z[1] * x[2] - z[2] * x[1];
    y[1] = -z[0] * x[2] + z[2] * x[0];
    y[2] = z[0] * x[1] - z[1] * x[0];

    /* mpichler, 19950515 */
    /* cross product gives area of parallelogram, which is < 1.0 for
     * non-perpendicular unit-length vectors; so normalize x, y here
     */

    mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    if (mag) {
        x[0] /= mag;
        x[1] /= mag;
        x[2] /= mag;
    }

    mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
    if (mag) {
        y[0] /= mag;
        y[1] /= mag;
        y[2] /= mag;
    }

#define M(row,col)  m[col*4+row]
    M(0, 0) = x[0];
    M(0, 1) = x[1];
    M(0, 2) = x[2];
    M(0, 3) = 0.0;
    M(1, 0) = y[0];
    M(1, 1) = y[1];
    M(1, 2) = y[2];
    M(1, 3) = 0.0;
    M(2, 0) = z[0];
    M(2, 1) = z[1];
    M(2, 2) = z[2];
    M(2, 3) = 0.0;
    M(3, 0) = 0.0;
    M(3, 1) = 0.0;
    M(3, 2) = 0.0;
    M(3, 3) = 1.0;
#undef M
    glMultMatrixf(m);

    /* Translate Eye to Origin */
    glTranslatef(-eyex, -eyey, -eyez);

}

void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
    GLfloat f = 1.0f / tanf(fovy * (M_PI / 360.0));
    GLfloat m[16];

    m[0] = f / aspect;
    m[1] = 0.0;
    m[2] = 0.0;
    m[3] = 0.0;

    m[4] = 0.0;
    m[5] = f;
    m[6] = 0.0;
    m[7] = 0.0;

    m[8] = 0.0;
    m[9] = 0.0;
    m[10] = (zFar + zNear) / (zNear - zFar);
    m[11] = -1.0;

    m[12] = 0.0;
    m[13] = 0.0;
    m[14] = 2.0 * zFar * zNear / (zNear - zFar);
    m[15] = 0.0;

    glMultMatrixf(m);
}

/*
 * callback function to read squirrel script from asset
 */
static SQInteger sq_lexer_asset(SQUserPointer asset) {
    SQChar c;
    if (AAsset_read((AAsset*) asset, &c, 1) > 0) {
        return c;
    }
    return 0;
}

/*
 * callback function to read squirrel script
 */
static SQInteger sq_lexer_fp(SQUserPointer fp) {
    int ret;
    SQChar c;
    if ((ret = fread(&c, sizeof(c), 1, (FILE *) fp) > 0)) {
        return c;
    }
    return 0;
}

/*
 * callback function for reading squuirrel bytecodes
 */
static SQInteger sq_lexer_bytecode(SQUserPointer asset, SQUserPointer buf,
        SQInteger size) {
    SQInteger ret;
    if ((ret = AAsset_read((AAsset*) asset, buf, size)) != 0) {
        return ret;
    }
    return -1;
}

/*
 * load text content from asset
 */
std::string loadContentFromAsset(std::string fname) {
    std::string content;
    /*
     * read squirrel script from asset
     */
    AAssetManager* mgr = engine->app->activity->assetManager;
    if (mgr == NULL) {
        engine->setLastError(ERR_SCRIPT_LOAD);
        LOGE("loadScriptFromAsset: failed to load AAssetManager");
        return content;
    }

    AAsset* asset = AAssetManager_open(mgr, fname.c_str(), AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
        engine->setLastError(ERR_SCRIPT_OPEN);
        LOGW("loadScriptFromAsset: failed to open main script file");
        LOGW(fname.c_str());
        return content;
    }

    off_t length = AAsset_getLength(asset);
    char* buffer = new char[length + 1];

    for (int i = 0; i < length; i++) {
        SQChar c;
        if (AAsset_read((AAsset*) asset, &c, 1) > 0) {
            buffer[i] = c;
        }
    }
    buffer[length] = NULL;

    content = buffer;
    delete buffer;

    AAsset_close(asset);

    return content;
}

/*
 * Load squirrel script from asset
 */
bool loadScriptFromAsset(const char* fname) {

    std::string fpath = std::string( SQUIRREL_SCRIPT_DIR );
    fpath.append(fname);

    /*
     * read squirrel script from asset
     */
    AAssetManager* mgr = engine->app->activity->assetManager;
    if (mgr == NULL) {
        engine->setLastError(ERR_SCRIPT_LOAD);
        LOGE("loadScriptFromAsset: failed to load AAssetManager");
        return false;
    }

    AAsset* asset = AAssetManager_open(mgr, fpath.c_str(), AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
        engine->setLastError(ERR_SCRIPT_OPEN);
        LOGW("loadScriptFromAsset: failed to open main script file");
        LOGW(fname);
        return false;
    }

    unsigned short sqtag;
    bool isByteCode = false;
    if (AAsset_read(asset, &sqtag, 2) > 0) {
        if (sqtag == SQ_BYTECODE_STREAM_TAG) {
            isByteCode = true;
        }
        AAsset_seek(asset, 0, SEEK_SET);
    } else {
        AAsset_close(asset);
        return false;
    }

    if (isByteCode
            && SQ_SUCCEEDED(sq_readclosure(engine->sqvm, sq_lexer_bytecode, asset))) {
        sq_pushroottable(engine->sqvm);
        if (SQ_FAILED(sq_call(engine->sqvm, 1, SQFalse, SQTrue))) {
            engine->setLastError(ERR_SCRIPT_CALL_ROOT);
            LOGW("loadScriptFromAsset: failed to sq_call");
            LOGW(fname);
            AAsset_close(asset);
            return false;
        }
    } else if (!isByteCode
            && SQ_SUCCEEDED(sq_compile(engine->sqvm, sq_lexer_asset, asset, fname, SQTrue))) {
        sq_pushroottable(engine->sqvm);
        if (SQ_FAILED(sq_call(engine->sqvm, 1, SQFalse, SQTrue))) {
            engine->setLastError(ERR_SCRIPT_CALL_ROOT);
            LOGW("loadScriptFromAsset: failed to sq_call");
            LOGW(fname);
            AAsset_close(asset);
            return false;
        }
    } else {
        engine->setLastError(ERR_SCRIPT_COMPILE);
        LOGW("loadScriptFromAsset: failed to compile squirrel script");
        LOGW(fname);
        AAsset_close(asset);
        return false;
    }

    AAsset_close(asset);

    return true;
}

/*
 * load squirrel script from given file name
 */
bool loadScript(const char* fname) {
    FILE* fp = fopen(fname, "r");
    if (fp == NULL) {
        engine->setLastError(ERR_SCRIPT_OPEN);
        LOGW("loadScript: failed to open main script file");
        LOGW(fname);
        return false;
    }
    if (SQ_SUCCEEDED(sq_compile(engine->sqvm, sq_lexer_fp, fp, fname, SQTrue))) {
        sq_pushroottable(engine->sqvm);
        if (SQ_FAILED(sq_call(engine->sqvm, 1, SQFalse, SQTrue))) {
            engine->setLastError(ERR_SCRIPT_CALL_ROOT);
            LOGW("loadScript: failed to sq_call");
            LOGW(fname);
            fclose(fp);
            return false;
        }
    } else {
        engine->setLastError(ERR_SCRIPT_COMPILE);
        LOGW("loadScript: failed to compile squirrel script");
        LOGW(fname);
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

/*
 * load squirrel script from user document
 */
bool loadScriptFromUserDocument(const char* fname) {
    return loadScript(engine->javaGlue->getDataFilePath(fname).c_str());
}

/*
 * returns if the given string ends with the other string
 */
bool endsWith(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0
                == fullString.compare(fullString.length() - ending.length(),
                        ending.length(), ending));
    } else {
        return false;
    }
}

/*
 * prints xml parse error
 */
namespace rapidxml {
void parse_error_handler(const char *what, void *where_void) {
    LOGE(what);
}
}

/*
 * returns build number of the library
 */
SQInteger emoRuntimeBuildNumber(HSQUIRRELVM v) {
    sq_pushinteger(v, EMO_BUILD_NUMBER);
    return 1;
}

/*
 * Calls test function
 */
SQInteger emoRuntimeTest(HSQUIRRELVM v) {
    test();
    return 0;
}

/*
 * Runtime logging
 *
 * @param log level
 * @param log message
 */
SQInteger emoRuntimeLog(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    SQInteger level;
    const SQChar *message;

    if (nargs < 3)
        return 0;

    if (sq_gettype(v, 2) == OT_INTEGER && sq_gettype(v, 3) == OT_STRING) {
        sq_getinteger(v, 2, &level);
        sq_tostring(v, 3);
        sq_getstring(v, -1, &message);
        sq_poptop(v);

        switch (level) {
        case LOG_INFO:
            LOGI((char*) message);
            break;
        case LOG_ERROR:
            LOGE((char*) message);
            break;
        case LOG_WARN:
            LOGW((char*) message);
            break;
        }
    }
    return 0;
}

/*
 * Set Runtime log level
 *
 * @param log level
 * @param EMO_NO_ERROR if succeeds
 */
SQInteger emoRuntimeSetLogLevel(HSQUIRRELVM v) {

    if (sq_gettype(v, 2) == OT_INTEGER) {
        SQInteger level;
        sq_getinteger(v, 2, &level);

        switch (level) {
        case LOG_INFO:
        case LOG_WARN:
        case LOG_ERROR:
            engine->logLevel = level;
            break;
        default:
            sq_pushinteger(v, ERR_INVALID_PARAM);
            return 1;
        }

    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * Runtime log info
 *
 * @param log message
 */
SQInteger emoRuntimeLogInfo(HSQUIRRELVM v) {
    const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for (SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);

            LOGI((char*) str);
        }
    }
    return 0;
}

/*
 * Runtime log error
 * 
 * @param log message
 */
SQInteger emoRuntimeLogError(HSQUIRRELVM v) {
    const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for (SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);

            LOGE((char*) str);
        }
    }
    return 0;
}

/*
 * Runtime log warn
 *
 * @param log message
 */
SQInteger emoRuntimeLogWarn(HSQUIRRELVM v) {
    const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for (SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);

            LOGW((char*) str);
        }
    }
    return 0;
}

/*
 * Runtime echo
 *
 * @param message
 * @return given message
 */
SQInteger emoRuntimeEcho(HSQUIRRELVM v) {
    const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for (SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);
            sq_poptop(v);
        } else {
            str = NULL;
        }
    }

    if (str != NULL) {
        sq_pushstring(v, str, -1);
    }

    return 1;
}

/*
 * Shutdown the runtime
 */
SQInteger emoRuntimeFinish(HSQUIRRELVM v) {
    engine->finishing = true;
    return 0;
}

/*
 * Returns OS name
 */
SQInteger emoRuntimeGetOSName(HSQUIRRELVM v) {
    sq_pushstring(v, (SQChar*) OS_ANDROID, -1);
    return 1;
}

/*
 * Returns device name
 */
SQInteger emoRuntimeGetDeviceName(HSQUIRRELVM v) {
    sq_pushstring(v, (SQChar*) engine->javaGlue->getDeviceName().c_str(), -1);
    return 1;
}

/*
 * Returns whether the device is simulator or not
 */
SQInteger emoRuntimeIsSimulator(HSQUIRRELVM v) {
    sq_pushbool(v, engine->javaGlue->isSimulator());
    return 1;
}

/*
 * start stopwatch
 */
SQInteger emoRuntimeStopwatchStart(HSQUIRRELVM v) {
    engine->stopwatchStart();
    return 0;
}

/*
 * restart stopwatch
 */
SQInteger emoRuntimeStopwatchRestart(HSQUIRRELVM v) {
    engine->stopwatchRestart();
    return 0;
}

/*
 * get elapsed time from stop watch
 */
SQInteger emoRuntimeStopwatchElapsed(HSQUIRRELVM v) {
    sq_pushinteger(v, engine->stopwatchElapsed());
    return 1;
}

/*
 * get real elapsed time from stop watch
 */
SQInteger emoRuntimeStopwatchRealElapsed(HSQUIRRELVM v) {
    sq_pushinteger(v, engine->stopwatchRealElapsed());
    return 1;
}

/*
 * stop stopwatch
 */
SQInteger emoRuntimeStopwatchStop(HSQUIRRELVM v) {
    engine->stopwatchStop();
    return 0;
}

/*
 * return whether the stopwatch has been stopped or not
 */
SQInteger emoRuntimeStopwatchIsStopped(HSQUIRRELVM v) {
    sq_pushbool(v, engine->stopwatchIsStopped());
    return 1;
}

/*
 * Import function called from squirrel script
 */
SQInteger emoImportScript(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for (SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_STRING) {
            const SQChar *fname;
            sq_tostring(v, n);
            sq_getstring(v, -1, &fname);
            sq_poptop(v);

            loadScriptFromAsset((char*) fname);
        }
    }
    return 0;
}

#ifndef EMO_WITH_SANDBOX
/*
 * compile script from path
 *
 * @param script file name
 */
SQInteger emoRuntimeCompile(HSQUIRRELVM v) {

    if (sq_gettype(v, 2) == OT_STRING) {
        const SQChar *fname;
        sq_tostring(v, 2);
        sq_getstring(v, -1, &fname);
        sq_poptop(v);

        // check if the file type exists
        if (sq_gettype(v, 3) == OT_INTEGER) {
            SQInteger fileType = TYPE_ASSET;
            sq_getinteger(v, 3, &fileType);

            if (fileType == TYPE_ASSET) {
                // load script from resource
                loadScriptFromAsset((char*) fname);
            } else if (fileType == TYPE_DOCUMENT) {
                // load script from user document directory
                loadScriptFromUserDocument(fname);
            } else {
                // load script from path
                loadScript(fname);
            }
        } else {
            // load script from path
            loadScript(fname);
        }

    }
    return 0;
}

/*
 * compile script from given string
 *
 * @param squirrel program as string
 */
SQInteger emoRuntimeCompileBuffer(HSQUIRRELVM v) {
    const SQChar* script;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &script);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    sq_pushinteger(v, sqCompileBuffer(v, script, EMO_RUNTIME_CLASS));
    return 1;
}
#endif

/*
 * Returns the document directory
 */
SQInteger emoRuntimeGetDocumentDir(HSQUIRRELVM v) {
    sq_pushstring(v, engine->javaGlue->getDataFilePath("").c_str(), -1);
    return 1;
}

/*
 * Check if the sandbox is enabled
 */
SQInteger emoRuntimeIsSandboxEnabled(HSQUIRRELVM v) {
#ifdef EMO_WITH_SANDBOX
    sq_pushbool(v, true);
#else
    sq_pushbool(v, false);
#endif    
    return 1;
}

/*
 * set options function called from script
 * 
 * @param options to be enabled
 */
SQInteger emoSetOptions(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for (SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_INTEGER) {
            SQInteger value;
            sq_getinteger(v, n, &value);

            engine->updateOptions(value);
        }
    }
    return 0;
}

/*
 * register sensors function called from script
 *
 * @param sensors to be registered
 */
SQInteger emoRegisterSensors(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for (SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_INTEGER) {
            SQInteger value;
            sq_getinteger(v, n, &value);

            engine->createSensors(value);
        }
    }
    return 0;
}

/*
 * enable sensor
 *
 * @param sensor type
 * @param sensor interval
 */
SQInteger emoEnableSensor(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    if (nargs < 3) {
        LOGE("Invalid call: emoEnableSensors(sensorType, interval)");
        return 0;
    }

    SQInteger sensorType;
    SQInteger interval;

    sq_getinteger(v, 2, &sensorType);
    sq_getinteger(v, 3, &interval);

    engine->enableSensor(sensorType, interval);

    return 0;
}

/*
 * disable sensor
 *
 * @param sensor type
 */
SQInteger emoDisableSensor(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    if (nargs < 2) {
        LOGE("Invalid call: emoDisableSensors(sensorType)");
        return 0;
    }

    SQInteger sensorType;
    sq_getinteger(v, 2, &sensorType);

    engine->disableSensor(sensorType);

    return 0;
}

/*
 * enable onDraw callback
 *
 * @param onDraw interval (msec)
 */
SQInteger emoEnableOnDrawCallback(HSQUIRRELVM v) {
    engine->enableOnDrawListener(true);

    SQInteger nargs = sq_gettop(v);
    if (nargs <= 2 && sq_gettype(v, 2) != OT_NULL) {
        SQInteger interval;
        sq_getinteger(v, 2, &interval);

        engine->setOnDrawListenerInterval(interval);
    }

    return 0;
}

/*
 * disable onDraw callback
 */
SQInteger emoDisableOnDrawCallback(HSQUIRRELVM v) {
    engine->enableOnDrawListener(false);
    return 0;
}

/*
 * enable onUpdate callback
 */
SQInteger emoEnableOnUpdateCallback(HSQUIRRELVM v) {
    engine->enableOnUpdateListener(true);
    return 0;
}

/*
 * disable onUpdate callback
 */
SQInteger emoDisableOnUpdateCallback(HSQUIRRELVM v) {
    engine->enableOnUpdateListener(false);
    return 0;
}

/*
 * enable onFps callback
 * 
 * @param onUpdate interval
 */
SQInteger emoEnableOnFpsCallback(HSQUIRRELVM v) {
    engine->enableOnFpsListener(true);

    SQInteger nargs = sq_gettop(v);

    if (nargs <= 2 && sq_gettype(v, 2) != OT_NULL) {
        SQInteger interval;
        sq_getinteger(v, 2, &interval);

        engine->setOnFpsListenerInterval(interval);
    }

    return 0;
}

/*
 * disable onFps callback
 */
SQInteger emoDisableOnFpsCallback(HSQUIRRELVM v) {
    engine->enableOnFpsListener(false);
    return 0;
}

/*
 * invoke garbage collection
 */
SQInteger emoRuntimeGC(HSQUIRRELVM v) {
    sq_pushinteger(v, sq_collectgarbage(v));
    return 1;
}

/*
 * clear texture cache
 */
SQInteger emoClearImageCache(HSQUIRRELVM v) {
    engine->clearCachedImage();
    return 0;
}

/*
 * Use simple log without any tag and level
 *
 * This takes no effect on Android
 */
SQInteger emoEnableSimpleLog(HSQUIRRELVM v) {
    sq_pushinteger(v, ERR_NOT_SUPPORTED);
    return 1;
}

/*
 * Use simple log without tag
 *
 * This takes no effect on Android
 */
SQInteger emoEnableSimpleLogWithLevel(HSQUIRRELVM v) {
    sq_pushinteger(v, ERR_NOT_SUPPORTED);
    return 1;
}

/*
 * Returns a integer value with a positive sign,
 * greater than or equal to 0.0 and less than max.
 */
SQInteger emoRuntimeNativeRandom(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);

    SQInteger max = RAND_MAX;
    if (nargs <= 2 && sq_gettype(v, 2) == OT_INTEGER) {
        sq_getinteger(v, 2, &max);
    }

    sq_pushinteger(v, engine->javaGlue->callInt_Int("random", max));
    return 1;
}

/*
 * Returns a integer value with a positive sign,
 * greater than or equal to 0.0 and less than max.
 */
SQInteger emoRuntimeRandom(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);

    SQInteger max = RAND_MAX;
    if (nargs <= 2 && sq_gettype(v, 2) == OT_INTEGER) {
        sq_getinteger(v, 2, &max);
    }

    srand(time(NULL));

    sq_pushinteger(v, rand() % max);
    return 1;
}

/*
 * Returns default locale
 */
SQInteger emoGetDefaultLocale(HSQUIRRELVM v) {
    sq_pushstring(v,
            engine->javaGlue->callVoid_String("getDefaultLocale").c_str(), -1);
    return 1;
}
