#include <GLES/gl.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include <squirrel.h>
#include <common.h>
#include <main.h>

extern struct engine *g_engine;

/* Log Info */
void LOGI(const SQChar* msg) {
	((void)__android_log_print(ANDROID_LOG_INFO,  EMO_LOG_TAG, msg));
}

/* Log Warning */
void LOGW(const SQChar* msg) {
	((void)__android_log_print(ANDROID_LOG_WARN,  EMO_LOG_TAG, msg));
}

/* Log Error */
void LOGE(const SQChar* msg) {
	((void)__android_log_print(ANDROID_LOG_ERROR, EMO_LOG_TAG, msg));
}

/*
 * Read script callback
 */
SQInteger sq_lexer(SQUserPointer asset) {
	SQChar c;
		if(AAsset_read((AAsset*)asset, &c, 1) > 0) {
			return c;
		}
	return 0;
}

/*
 * Load squirrel script from asset
 */
SQBool loadScriptFromAsset(const char* fname) {
    /*
     * read squirrel script from asset
     */
    AAssetManager* mgr = g_engine->app->activity->assetManager;
    if (mgr == NULL) {
    	g_engine->lastError = ERR_SCRIPT_LOAD;
    	LOGE("Failed to load AAssetManager");
    	return SQFalse;
    }

    AAsset* asset = AAssetManager_open(mgr, fname, AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
    	g_engine->lastError = ERR_SCRIPT_OPEN;
    	LOGW("Failed to open main script file");
        LOGW(fname);
    	return SQFalse;
    }

    if(SQ_SUCCEEDED(sq_compile(g_engine->sqvm, sq_lexer, asset, fname, SQTrue))) {
        sq_pushroottable(g_engine->sqvm);
        if (SQ_FAILED(sq_call(g_engine->sqvm, 1, SQFalse, SQTrue))) {
        	g_engine->lastError = ERR_SCRIPT_CALL_ROOT;
            LOGW("failed to sq_call");
            LOGW(fname);
            return SQFalse;
        }
    } else {
    	g_engine->lastError = ERR_SCRIPT_COMPILE;
        LOGW("Failed to compile squirrel script");
        LOGW(fname);
        return SQFalse;
    }

    AAsset_close(asset);

    return SQTrue;
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

    		loadScriptFromAsset(fname);
    	}
    }
	return 0;
}

static void emoUpdateOptions(SQInteger value) {
    switch(value) {
    case OPT_ENABLE_PERSPECTIVE_NICEST:
        g_engine->enablePerspectiveNicest = true;
        break;
    case OPT_ENABLE_PERSPECTIVE_FASTEST:
        g_engine->enablePerspectiveNicest = false;
        break;
    case OPT_ENABLE_ONDRAW_CALLBACK:
        g_engine->enableOnDrawFrame = true;
        break;
    }
}

/*
 * option function called from squirrel script
 */
SQInteger emoSetOptions(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_INTEGER) {
            SQInteger value;
            sq_getinteger(v, n, &value);

            emoUpdateOptions(value);
        }
    }
    return 0;
}

