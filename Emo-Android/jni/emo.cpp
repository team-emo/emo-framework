#include <stdio.h>
#include <stdarg.h>

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
/**
 * Initialize the framework
 */
void emo_init_display(struct engine* engine) {

    /* init Squirrel VM */
    sqstd_seterrorhandlers(engine->sqvm);
    sq_setprintfunc(engine->sqvm, sq_printfunc, sq_errorfunc);

    /* read squirrel script */
    SQChar sqbuffer[1024] = _SC("local str = \"hello, squirrel!\";print(str);");
    if(SQ_SUCCEEDED(sq_compilebuffer(engine->sqvm, sqbuffer, strlen(sqbuffer), _SC("main.nut"), false))) {
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

