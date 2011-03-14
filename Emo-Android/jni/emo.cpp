#include <stdio.h>

#include <android/asset_manager.h>
#include <GLES/gl.h>

#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdaux.h>

#include <common.h>
#include <emo.h>

/*
 * Squirrel basic functions
 */
static void sq_printfunc(HSQUIRRELVM v,const SQChar *s,...) {
	static SQChar text[2048];
	va_list args;
    va_start(args, s);
    scvsprintf(text, s, args);
    va_end(args);

    LOGI(text);
}
static void sq_errorfunc(HSQUIRRELVM v,const SQChar *s,...) {
	static SQChar text[2048];
	va_list args;
    va_start(args, s);
    scvsprintf(text, s, args);
    va_end(args);

    LOGW(text);
}
static SQInteger sqlexer(SQUserPointer asset) {
	SQChar c;
		if(AAsset_read((AAsset*)asset, &c, 1) > 0) {
			return c;
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

    /* read squirrel script */
    // use asset manager to open asset by filename
    AAssetManager* mgr = engine->app->activity->assetManager;
    if (mgr == NULL) {
    	LOGE("Failed to load AAssetManager");
    	return;
    }

    AAsset* asset = AAssetManager_open(mgr, SQUIRREL_MAIN_SCRIPT, AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
    	LOGE("Failed to open Emo main script file");
    	return;
    }

    if(SQ_SUCCEEDED(sq_compile(engine->sqvm, sqlexer, asset, SQUIRREL_MAIN_SCRIPT, SQTrue))) {
        sq_pushroottable(engine->sqvm);
        if (SQ_FAILED(sq_call(engine->sqvm, 1, SQFalse, SQTrue))) {
            LOGE("failed to sq_call");
        }
    } else {
        LOGE("Failed to load squirrel script");
    }

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
}


/*
 * Terminate the framework
 */
void emo_term_display(struct engine* engine) {

}

/**
 * Process the input event.
 */
int32_t emo_handle_input(struct android_app* app, AInputEvent* event) {
    struct engine* engine = (struct engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        engine->state.x = AMotionEvent_getX(event, 0);
        engine->state.y = AMotionEvent_getY(event, 0);
        return 1;
    }
    return 0;
}

/*
 * Gained focus
 */
void emo_gained_focus(struct engine* engine) {
    engine->animating = 1;
}

/*
 * Lost focus
 */
void emo_lost_focus(struct engine* engine) {
    engine->animating = 0;
}

