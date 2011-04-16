#include <math.h>

#include <android/log.h>
#include <android/native_activity.h>
#include <android_native_app_glue.h>
#include <android/window.h>
#include <android/configuration.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <squirrel.h>
#include <common.h>
#include <main.h>
#include <sqfunc.h>
#include <emo_engine.h>
#include <emo_engine_func.h>
#include <png.h>

extern engine *g_engine;

/* Log INFO */
void LOGI(const SQChar* msg) {
	((void)__android_log_print(ANDROID_LOG_INFO,  EMO_LOG_TAG, msg));
}

/* Log WARN */
void LOGW(const SQChar* msg) {
	((void)__android_log_print(ANDROID_LOG_WARN,  EMO_LOG_TAG, msg));
}

/* Log ERROR */
void LOGE(const SQChar* msg) {
	((void)__android_log_print(ANDROID_LOG_ERROR, EMO_LOG_TAG, msg));
}

/*
 * callback function to read squirrel script
 */
static SQInteger sq_lexer(SQUserPointer asset) {
	SQChar c;
		if(AAsset_read((AAsset*)asset, &c, 1) > 0) {
			return c;
		}
	return 0;
}

/*
 * callback function to read png image
 */
static void png_asset_read(png_structp png_ptr, png_bytep data, png_uint_32 length) {
    AAsset* asset = (AAsset*)png_get_io_ptr(png_ptr);
    AAsset_read(asset, data, length);
}

bool loadPngSizeFromAsset(const char *fname, int *width, int *height) {
    AAssetManager* mgr = g_engine->app->activity->assetManager;
    if (mgr == NULL) {
    	g_engine->lastError = ERR_ASSET_LOAD;
    	LOGE("loadPngFromAsset: failed to load AAssetManager");
    	return false;
    }

    AAsset* asset = AAssetManager_open(mgr, fname, AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
    	g_engine->lastError = ERR_ASSET_OPEN;
    	LOGW("loadPngFromAsset: failed to open asset");
        LOGW(fname);
    	return false;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (info_ptr == NULL) {
        AAsset_close(asset);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        AAsset_close(asset);
        return false;
    }

    png_set_read_fn(png_ptr, (void *)asset, png_asset_read);

    unsigned int sig_read = 0;
    png_set_sig_bytes(png_ptr, sig_read);

    png_read_info(png_ptr, info_ptr);

    *width  = info_ptr->width;
    *height = info_ptr->height;

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    AAsset_close(asset);

    return true;
}

/* 
 * load png image from asset
 */
bool loadPngFromAsset(const char *fname, ImageInfo* imageInfo) {
    AAssetManager* mgr = g_engine->app->activity->assetManager;
    if (mgr == NULL) {
    	g_engine->lastError = ERR_ASSET_LOAD;
    	LOGE("loadPngFromAsset: failed to load AAssetManager");
    	return false;
    }

    AAsset* asset = AAssetManager_open(mgr, fname, AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
    	g_engine->lastError = ERR_ASSET_OPEN;
    	LOGW("loadPngFromAsset: failed to open asset");
        LOGW(fname);
    	return false;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (info_ptr == NULL) {
        AAsset_close(asset);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        AAsset_close(asset);
        return false;
    }

    png_set_read_fn(png_ptr, (void *)asset, png_asset_read);

    unsigned int sig_read = 0;
    png_set_sig_bytes(png_ptr, sig_read);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, NULL);

    imageInfo->textureId = -1;
    imageInfo->filename = fname;
    imageInfo->width  = info_ptr->width;
    imageInfo->height = info_ptr->height;

    switch (info_ptr->color_type) {
        case PNG_COLOR_TYPE_RGBA:
            imageInfo->hasAlpha = true;
            break;
        case PNG_COLOR_TYPE_RGB:
            imageInfo->hasAlpha = false;
            break;
        default:
            LOGE("loadPngFromAsset: unsupported color type");
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            AAsset_close(asset);
            return false;
    }
    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    imageInfo->data = (unsigned char*) malloc(row_bytes * imageInfo->height);

    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

    for (int i = 0; i < imageInfo->height; i++) {
        memcpy(imageInfo->data+(row_bytes * (imageInfo->height-1-i)), row_pointers[i], row_bytes);
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    AAsset_close(asset);

    return true;
}

/*
 * Load squirrel script from asset
 */
bool loadScriptFromAsset(const char* fname) {
    /*
     * read squirrel script from asset
     */
    AAssetManager* mgr = g_engine->app->activity->assetManager;
    if (mgr == NULL) {
    	g_engine->lastError = ERR_SCRIPT_LOAD;
    	LOGE("loadScriptFromAsset: failed to load AAssetManager");
    	return false;
    }

    AAsset* asset = AAssetManager_open(mgr, fname, AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
    	g_engine->lastError = ERR_SCRIPT_OPEN;
    	LOGW("loadScriptFromAsset: failed to open main script file");
        LOGW(fname);
    	return false;
    }

    if(SQ_SUCCEEDED(sq_compile(g_engine->sqvm, sq_lexer, asset, fname, SQTrue))) {
        sq_pushroottable(g_engine->sqvm);
        if (SQ_FAILED(sq_call(g_engine->sqvm, 1, SQFalse, SQTrue))) {
        	g_engine->lastError = ERR_SCRIPT_CALL_ROOT;
            LOGW("loadScriptFromAsset: failed to sq_call");
            LOGW(fname);
            return false;
        }
    } else {
    	g_engine->lastError = ERR_SCRIPT_COMPILE;
        LOGW("loadScriptFromAsset: failed to compile squirrel script");
        LOGW(fname);
        return false;
    }

    AAsset_close(asset);

    return true;
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
    g_engine->animating = 0;
    ANativeActivity_finish(g_engine->app->activity);
    return 0;
}

/*
 * Returns OS name
 */
SQInteger emoRuntimeGetOSName(HSQUIRRELVM v) {
    sq_pushstring(v, OS_ANDROID, -1);
    return 1;
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

void emoUpdateOptions(SQInteger value) {
    switch(value) {
    case OPT_ENABLE_PERSPECTIVE_NICEST:
        g_engine->enablePerspectiveNicest = true;
        break;
    case OPT_ENABLE_PERSPECTIVE_FASTEST:
        g_engine->enablePerspectiveNicest = false;
        break;
    case OPT_WINDOW_FORCE_FULLSCREEN:
        ANativeActivity_setWindowFlags(g_engine->app->activity, AWINDOW_FLAG_FULLSCREEN, AWINDOW_FLAG_FORCE_NOT_FULLSCREEN);
        break;
    case OPT_WINDOW_KEEP_SCREEN_ON:
        ANativeActivity_setWindowFlags(g_engine->app->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);
        break;
    case OPT_ENABLE_BACK_KEY:
        g_engine->enableBackKey = true;
    case OPT_DISABLE_BACK_KEY:
        g_engine->enableBackKey = false;
        break;
    case OPT_ORIENTATION_PORTRAIT:
        AConfiguration_setOrientation(g_engine->app->config, ACONFIGURATION_ORIENTATION_PORT);
        break;
    case OPT_ORIENTATION_LANDSCAPE:
        AConfiguration_setOrientation(g_engine->app->config, ACONFIGURATION_ORIENTATION_LAND);
        break;
    }
}

void emoCreateSensors(SQInteger value) {

    if (g_engine->sensorManager == NULL) {
        // Get a reference to the sensor manager
        g_engine->sensorManager = ASensorManager_getInstance();

        // Creates a new sensor event queue
        g_engine->sensorEventQueue = ASensorManager_createEventQueue(
            g_engine->sensorManager, g_engine->app->looper, LOOPER_ID_USER, NULL, NULL);
    }

    switch(value) {
    case SENSOR_TYPE_ACCELEROMETER:
        g_engine->accelerometerSensor = ASensorManager_getDefaultSensor(
            g_engine->sensorManager, ASENSOR_TYPE_ACCELEROMETER);
        break;
    case SENSOR_TYPE_MAGNETIC_FIELD:
        g_engine->magneticSensor = ASensorManager_getDefaultSensor(
            g_engine->sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
        break;
    case SENSOR_TYPE_GYROSCOPE:
        g_engine->gyroscopeSensor = ASensorManager_getDefaultSensor(
            g_engine->sensorManager, ASENSOR_TYPE_GYROSCOPE);
        break;
    case SENSOR_TYPE_LIGHT:
        g_engine->lightSensor = ASensorManager_getDefaultSensor(
            g_engine->sensorManager, ASENSOR_TYPE_LIGHT);
        break;
    case SENSOR_TYPE_PROXIMITY:
        g_engine->proximitySensor = ASensorManager_getDefaultSensor(
            g_engine->sensorManager, ASENSOR_TYPE_PROXIMITY);
        break;
    }

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

            emoUpdateOptions(value);
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

            emoCreateSensors(value);
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

    switch(sensorType) {
    case SENSOR_TYPE_ACCELEROMETER:
        if (g_engine->accelerometerSensor != NULL) {
            ASensorEventQueue_enableSensor(g_engine->sensorEventQueue,
                g_engine->accelerometerSensor);
            ASensorEventQueue_setEventRate(g_engine->sensorEventQueue,
                g_engine->accelerometerSensor, interval);
        }
        break;
    case SENSOR_TYPE_MAGNETIC_FIELD:
        if (g_engine->magneticSensor != NULL) {
            ASensorEventQueue_enableSensor(g_engine->sensorEventQueue,
                g_engine->magneticSensor);
            ASensorEventQueue_setEventRate(g_engine->sensorEventQueue,
                g_engine->magneticSensor, interval);
        }
        break;
    case SENSOR_TYPE_GYROSCOPE:
        if (g_engine->gyroscopeSensor != NULL) {
            ASensorEventQueue_enableSensor(g_engine->sensorEventQueue,
                g_engine->gyroscopeSensor);
            ASensorEventQueue_setEventRate(g_engine->sensorEventQueue,
                g_engine->gyroscopeSensor, interval);
        }
        break;
    case SENSOR_TYPE_LIGHT:
        if (g_engine->lightSensor != NULL) {
            ASensorEventQueue_enableSensor(g_engine->sensorEventQueue,
                g_engine->lightSensor);
            ASensorEventQueue_setEventRate(g_engine->sensorEventQueue,
                g_engine->lightSensor, interval);
        }
        break;
    case SENSOR_TYPE_PROXIMITY:
        if (g_engine->proximitySensor != NULL) {
            ASensorEventQueue_enableSensor(g_engine->sensorEventQueue,
                g_engine->proximitySensor);
            ASensorEventQueue_setEventRate(g_engine->sensorEventQueue,
                g_engine->proximitySensor, interval);
        }
        break;
    }

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

    switch(sensorType) {
    case SENSOR_TYPE_ACCELEROMETER:
        if (g_engine->accelerometerSensor != NULL) {
            ASensorEventQueue_disableSensor(g_engine->sensorEventQueue,
                g_engine->accelerometerSensor);
        }
        break;
    case SENSOR_TYPE_MAGNETIC_FIELD:
        if (g_engine->magneticSensor != NULL) {
            ASensorEventQueue_disableSensor(g_engine->sensorEventQueue,
                g_engine->magneticSensor);
        }
        break;
    case SENSOR_TYPE_GYROSCOPE:
        if (g_engine->gyroscopeSensor != NULL) {
            ASensorEventQueue_disableSensor(g_engine->sensorEventQueue,
                g_engine->gyroscopeSensor);
        }
        break;
    case SENSOR_TYPE_LIGHT:
        if (g_engine->lightSensor != NULL) {
            ASensorEventQueue_disableSensor(g_engine->sensorEventQueue,
                g_engine->lightSensor);
        }
        break;
    case SENSOR_TYPE_PROXIMITY:
        if (g_engine->proximitySensor != NULL) {
            ASensorEventQueue_disableSensor(g_engine->sensorEventQueue,
                g_engine->proximitySensor);
        }
        break;
    }

    return 0;

}

SQInteger emoEnableOnDrawCallback(HSQUIRRELVM v) {
    g_engine->enableOnDrawFrame = SQTrue;

    SQInteger nargs = sq_gettop(v);

    if (nargs <= 2 && sq_gettype(v, 2) == OT_INTEGER) {
        SQInteger interval;
        sq_getinteger(v, 2, &interval);

        g_engine->onDrawFrameInterval = interval;
    }

    return 0;
}

SQInteger emoDisableOnDrawCallback(HSQUIRRELVM v) {
    g_engine->enableOnDrawFrame = SQFalse;
    return 0;
}
