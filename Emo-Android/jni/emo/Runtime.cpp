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

#include <EGL/egl.h>
#include <GLES/gl.h>

#include "Constants.h"
#include "Runtime.h"
#include "Engine.h"
#include "VmFunc.h"

extern emo::Engine* engine;

void initRuntimeFunctions() {
    registerClass(engine->sqvm, EMO_RUNTIME_CLASS);
    registerClass(engine->sqvm, EMO_EVENT_CLASS);
    registerClass(engine->sqvm, EMO_STOPWATCH_CLASS);

    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "import",          emoImportScript);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "setOptions",      emoSetOptions);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "echo",            emoRuntimeEcho);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "log",             emoRuntimeLog);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "info",            emoRuntimeLogInfo);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "error",           emoRuntimeLogError);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "warn",            emoRuntimeLogWarn);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "finish",          emoRuntimeFinish);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "os",              emoRuntimeGetOSName);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "device",          emoRuntimeGetDeviceName);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "isSimulator",     emoRuntimeIsSimulator);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "gc",              emoRuntimeGC);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "clearTextureCache", emoClearImageCache);
    registerClassFunc(engine->sqvm, EMO_STOPWATCH_CLASS, "start",         emoRuntimeStopwatchStart);
    registerClassFunc(engine->sqvm, EMO_STOPWATCH_CLASS, "stop",          emoRuntimeStopwatchStop);
    registerClassFunc(engine->sqvm, EMO_STOPWATCH_CLASS, "elapsed",       emoRuntimeStopwatchElapsed);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "setLogLevel",     emoRuntimeSetLogLevel);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "compilebuffer",   emoRuntimeCompileBuffer);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "compile",         emoRuntimeCompile);
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "getDocumentDir",  emoRuntimeGetDocumentDir);

    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "registerSensors", emoRegisterSensors);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "enableSensor",    emoEnableSensor);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "disableSensor",   emoDisableSensor);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "enableOnDrawCallback",  emoEnableOnDrawCallback);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "disableOnDrawCallback", emoDisableOnDrawCallback);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "enableOnUpdateCallback",  emoEnableOnUpdateCallback);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "disableOnUpdateCallback", emoDisableOnUpdateCallback);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "enableOnFpsCallback",     emoEnableOnFpsCallback);
    registerClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "disableOnFpsCallback",    emoDisableOnFpsCallback);
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
		((void)__android_log_print(ANDROID_LOG_INFO,  EMO_LOG_TAG, msg));
	}
}

/* Log WARN */
void LOGW(const char* msg) {
	if (engine->logLevel <= LOG_WARN) {
		((void)__android_log_print(ANDROID_LOG_WARN,  EMO_LOG_TAG, msg));
	}
}

/* Log ERROR */
void LOGE(const char* msg) {
	if (engine->logLevel <= LOG_ERROR) {
		((void)__android_log_print(ANDROID_LOG_ERROR, EMO_LOG_TAG, msg));
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

/*
 * callback function to read squirrel script from asset
 */
static SQInteger sq_lexer_asset(SQUserPointer asset) {
	SQChar c;
		if(AAsset_read((AAsset*)asset, &c, 1) > 0) {
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
    if((ret = fread(&c, sizeof(c), 1, (FILE *)fp) > 0)) {
        return c;
    }
    return 0;
}

/*
 * Load squirrel script from asset
 */
bool loadScriptFromAsset(const char* fname) {
    /*
     * read squirrel script from asset
     */
    AAssetManager* mgr = engine->app->activity->assetManager;
    if (mgr == NULL) {
    	engine->setLastError(ERR_SCRIPT_LOAD);
    	LOGE("loadScriptFromAsset: failed to load AAssetManager");
    	return false;
    }

    AAsset* asset = AAssetManager_open(mgr, fname, AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
    	engine->setLastError(ERR_SCRIPT_OPEN);
    	LOGW("loadScriptFromAsset: failed to open main script file");
        LOGW(fname);
    	return false;
    }

    if(SQ_SUCCEEDED(sq_compile(engine->sqvm, sq_lexer_asset, asset, fname, SQTrue))) {
        sq_pushroottable(engine->sqvm);
        if (SQ_FAILED(sq_call(engine->sqvm, 1, SQFalse, SQTrue))) {
        	engine->setLastError(ERR_SCRIPT_CALL_ROOT);
            LOGW("loadScriptFromAsset: failed to sq_call");
            LOGW(fname);
            return false;
        }
    } else {
    	engine->setLastError(ERR_SCRIPT_COMPILE);
        LOGW("loadScriptFromAsset: failed to compile squirrel script");
        LOGW(fname);
        return false;
    }

    AAsset_close(asset);

    return true;
}

bool loadScript(const char* fname) {
    FILE* fp = fopen(fname, "r");
    if (fp == NULL) {
    	engine->setLastError(ERR_SCRIPT_OPEN);
    	LOGW("loadScript: failed to open main script file");
        LOGW(fname);
    	return false;
    }
    if(SQ_SUCCEEDED(sq_compile(engine->sqvm, sq_lexer_fp, fp, fname, SQTrue))) {
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

bool loadScriptFromUserDocument(const char* fname) {
    return loadScript(engine->javaGlue->getDataFilePath(fname).c_str());
}

/*
 * Runtime logging
 */
SQInteger emoRuntimeLog(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    SQInteger level;
    const SQChar *message;

    if (nargs < 3) return 0;

    if (sq_gettype(v, 2) == OT_INTEGER && sq_gettype(v, 3) == OT_STRING) {
        sq_getinteger(v, 2, &level);
        sq_tostring(v, 3);
        sq_getstring(v, -1, &message);
        sq_poptop(v);

        switch(level) {
        case LOG_INFO:
            LOGI((char*)message);
            break;
        case LOG_ERROR:
            LOGE((char*)message);
            break;
        case LOG_WARN:
            LOGW((char*)message);
            break;
        }
    }
	return 0;
}

/*
 * Set Runtime log level
 */
SQInteger emoRuntimeSetLogLevel(HSQUIRRELVM v) {

	if (sq_gettype(v, 2) == OT_INTEGER) {
		SQInteger level;
		sq_getinteger(v, 2, &level);

		switch(level) {
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
 */
SQInteger emoRuntimeLogInfo(HSQUIRRELVM v) {
    const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);

            LOGI((char*)str);
        }
    }
    return 0;
}

/*
 * Runtime log error
 */
SQInteger emoRuntimeLogError(HSQUIRRELVM v) {
    const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);

            LOGE((char*)str);
        }
    }
    return 0;
}

/*
 * Runtime log warn
 */
SQInteger emoRuntimeLogWarn(HSQUIRRELVM v) {
    const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);

            LOGW((char*)str);
        }
    }
    return 0;
}


/*
 * Runtime echo
 */
SQInteger emoRuntimeEcho(HSQUIRRELVM v) {
	const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
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
    sq_pushstring(v, (SQChar*)OS_ANDROID, -1);
    return 1;
}

/*
 * Returns device name
 */
SQInteger emoRuntimeGetDeviceName(HSQUIRRELVM v) {
    sq_pushstring(v, (SQChar*)engine->javaGlue->getDeviceName().c_str(), -1);
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
 * get elapsed time from stop watch
 */
SQInteger emoRuntimeStopwatchElapsed(HSQUIRRELVM v) {
    sq_pushinteger(v, engine->stopwatchElapsed());
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
 * Import function called from squirrel script
 */
SQInteger emoImportScript(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
    	if (sq_gettype(v, n) == OT_STRING) {
    		const SQChar *fname;
            sq_tostring(v, n);
            sq_getstring(v, -1, &fname);
            sq_poptop(v);

    		loadScriptFromAsset((char*)fname);
    	}
    }
	return 0;
}

/*
 * compile script from path
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
                loadScriptFromAsset((char*)fname);
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
 * Returns the document directory
 */
SQInteger emoRuntimeGetDocumentDir(HSQUIRRELVM v) {
    sq_pushstring(v, engine->javaGlue->getDataFilePath("").c_str(), -1);
    return 1;
}

/*
 * set options function called from script
 */
SQInteger emoSetOptions(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
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
 */
SQInteger emoRegisterSensors(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
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

SQInteger emoDisableOnDrawCallback(HSQUIRRELVM v) {
    engine->enableOnDrawListener(false);
    return 0;
}

SQInteger emoEnableOnUpdateCallback(HSQUIRRELVM v) {
    engine->enableOnUpdateListener(true);
    return 0;
}
SQInteger emoDisableOnUpdateCallback(HSQUIRRELVM v) {
    engine->enableOnUpdateListener(false);
    return 0;
}

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

SQInteger emoDisableOnFpsCallback(HSQUIRRELVM v) {
    engine->enableOnFpsListener(false);
    return 0;
}

SQInteger emoRuntimeGC(HSQUIRRELVM v) {
    sq_pushinteger(v, sq_collectgarbage(v));
    return 1;
}

SQInteger emoClearImageCache(HSQUIRRELVM v) {
    engine->clearCachedImage();
    return 0;
}

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
