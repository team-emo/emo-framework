#include <stdio.h>

#include <android/asset_manager.h>
#include <GLES/gl.h>

#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdaux.h>

#include <common.h>
#include <emo.h>
#include <sqfunc.h>

/**
 * Initialize the framework
 */
void emo_init_display(struct engine* engine) {

    /* init Squirrel VM */
    sqstd_seterrorhandlers(engine->sqvm);
    sq_setprintfunc(engine->sqvm, sq_printfunc, sq_errorfunc);

    /* read squirrel script */
    // use asset manager to open asset by filename
    AAssetManager* mgr = engine->app->activity->assetManager;
    if (mgr == NULL) {
    	engine->lastError = ERR_SCRIPT_LOAD;
    	LOGE("Failed to load AAssetManager");
    	return;
    }

    AAsset* asset = AAssetManager_open(mgr, SQUIRREL_MAIN_SCRIPT, AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
    	engine->lastError = ERR_SCRIPT_OPEN;
    	LOGE("Failed to open main script file");
    	return;
    }

    if(SQ_SUCCEEDED(sq_compile(engine->sqvm, sq_lexer, asset, SQUIRREL_MAIN_SCRIPT, SQTrue))) {
        sq_pushroottable(engine->sqvm);
        if (SQ_FAILED(sq_call(engine->sqvm, 1, SQFalse, SQTrue))) {
        	engine->lastError = ERR_SCRIPT_CALL_ROOT;
            LOGE("failed to sq_call");
            return;
        }
    } else {
    	engine->lastError = ERR_SCRIPT_COMPILE;
        LOGE("Failed to compile squirrel script");
        return;
    }

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
    struct engine* engine = (struct engine*)app->userData;
	return 0;
}

/*
 * Process key event
 */
static int32_t emo_event_key(struct android_app* app, AInputEvent* event) {
    struct engine* engine = (struct engine*)app->userData;
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
