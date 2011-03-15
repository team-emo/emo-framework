#include <stdio.h>

#include <android/asset_manager.h>
#include <GLES/gl.h>
#include <android_native_app_glue.h>

#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdaux.h>

#include <common.h>
#include <emo.h>
#include <sqfunc.h>

/* global pointer to application engine */
extern struct engine *g_engine;

/*
 * mport function called from squirrel script
 */
SQInteger emo_import_script(HSQUIRRELVM v) {
    AAssetManager* mgr = g_engine->app->activity->assetManager;

    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
    	if (sq_gettype(v, n) == OT_STRING) {
    		const SQChar *fname;
            sq_tostring(v, n);
            sq_getstring(v, -1, &fname);
            sq_poptop(v);

    		loadScriptFromAsset(g_engine, mgr, fname);
    	}
    }
	return 0;
}

/*
 * option function called from squirrel script
 */
SQInteger emo_set_options(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
    	if (sq_gettype(v, n) == OT_STRING) {
    		const SQChar *fname;
            sq_tostring(v, n);
            sq_getstring(v, -1, &fname);
            sq_poptop(v);

    	}
    }
	return 0;
}

/**
 * Initialize the framework
 */
void emo_init_display(struct engine* engine) {

    /* init Squirrel VM */
    sqstd_seterrorhandlers(engine->sqvm);
    sq_setprintfunc(engine->sqvm, sq_printfunc, sq_errorfunc);

    register_global_func(engine->sqvm, emo_import_script, "emoImport");

    /*
     * read squirrel script from asset
     */
    AAssetManager* mgr = engine->app->activity->assetManager;
    if (mgr == NULL) {
    	engine->lastError = ERR_SCRIPT_LOAD;
    	LOGE("Failed to load AAssetManager");
    	return;
    }

    /* load main script */
    loadScriptFromAsset(engine, mgr, SQUIRREL_MAIN_SCRIPT);

    /* call onLoad() */
    callSqFunctionNoParam(engine->sqvm, "onLoad");

    /* init OpenGL state */
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

/*
 * Draw current frame
 */
void emo_draw_frame(struct engine* engine) {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if (engine->enableSQOnDrawFrame) {
    	callSqFunctionNoParam(engine->sqvm, "onDrawFrame");
    }
}

/*
 * Terminate the framework
 */
void emo_term_display(struct engine* engine) {
	callSqFunctionNoParam(engine->sqvm, "onDispose");
}

/*
 * Process motion event
 */
static int32_t emo_event_motion(struct android_app* app, AInputEvent* event) {
	return 0;
}

/*
 * Process key event
 */
static int32_t emo_event_key(struct android_app* app, AInputEvent* event) {
    return 0;
}

/**
 * Process the input event.
 */
int32_t emo_handle_input(struct android_app* app, AInputEvent* event) {
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        return emo_event_motion(app, event);
    } else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        return emo_event_key(app, event);
    }
    return 0;
}

/*
 * Gained focus
 */
void emo_gained_focus(struct engine* engine) {
	callSqFunctionNoParam(engine->sqvm, "onGainedFocus");
    engine->animating = 1;
}

/*
 * Lost focus
 */
void emo_lost_focus(struct engine* engine) {
	callSqFunctionNoParam(engine->sqvm, "onLostFocus");
    engine->animating = 0;
}
