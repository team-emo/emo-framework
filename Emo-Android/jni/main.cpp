#include <stdio.h>
#include <jni.h>
#include <errno.h>
#include <sys/timeb.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android_native_app_glue.h>

#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdaux.h>

#include <main.h>
#include <sqfunc.h>

/* global pointer to application engine */
struct engine *g_engine;

/*
 * extern common functions
 */
extern void LOGI(const SQChar* msg);
extern void LOGW(const SQChar* msg);
extern void LOGE(const SQChar* msg);

extern SQInteger sq_lexer(SQUserPointer asset);
extern SQInteger emoImportScript(HSQUIRRELVM v);
extern SQInteger emoSetOptions(HSQUIRRELVM v);
extern void emoUpdateOptions(SQInteger value);
extern SQBool loadScriptFromAsset(const char* fname);

static void engine_update_uptime(struct engine* engine) {
	timeb eventTime;
	ftime(&eventTime);

	engine->uptime.time     = eventTime.time - engine->startTime.time;
	engine->uptime.millitm  = eventTime.millitm;
	engine->uptime.timezone = eventTime.timezone;
	engine->uptime.dstflag  = eventTime.dstflag;
}

/**
 * Initialize the framework
 */
void emo_init_engine(struct engine* engine) {

    // initialize startup time
    ftime(&engine->startTime);

    // initialize uptime
    engine_update_uptime(engine);

    engine->sqvm = sq_open(SQUIRREL_VM_INITIAL_STACK_SIZE);
    engine->lastError = EMO_NO_ERROR;

    // disable drawframe callback to improve performance (default)
    engine->enableOnDrawFrame = SQFalse;

    // enable perspective hint to nicest (default)
    engine->enablePerspectiveNicest = SQTrue;

    // init Squirrel VM
    initSQVM(engine->sqvm);

    // initialize squirrel functions
    register_global_func(engine->sqvm, emoImportScript, "emo_import");
    register_global_func(engine->sqvm, emoSetOptions,   "emo_options");

    // load main script
    loadScriptFromAsset(SQUIRREL_MAIN_SCRIPT);

    // force fullscreen
    emoUpdateOptions(OPT_WINDOW_FORCE_FULLSCREEN);

    // call onLoad()
    callSqFunction(engine->sqvm, "onLoad");

}

/*
 * Initialize the display
 */
void emo_init_display(struct engine* engine) {

    /* initialize OpenGL state */

    if (engine->enablePerspectiveNicest) {
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    } else {
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_DITHER);
    glDisable(GL_COLOR_ARRAY);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_VERTEX_ARRAY);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}

/*
 * Draw current frame
 */
void emo_draw_frame(struct engine* engine) {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if (engine->enableOnDrawFrame) {
    	callSqFunction(engine->sqvm, "onDrawFrame");
    }
}

/*
 * Terminate the framework
 */
void emo_dispose_engine(struct engine* engine) {
    callSqFunction(engine->sqvm, "onDispose");
    sq_close(engine->sqvm);
}

/*
 * Process motion event
 */
static int32_t emo_event_motion(struct android_app* app, AInputEvent* event) {
	struct engine* engine = (struct engine*)app->userData;
	size_t pointerCount =  AMotionEvent_getPointerCount(event);

	engine_update_uptime(engine);

	for (size_t i = 0; i < pointerCount; i++) {
		size_t pointerId = AMotionEvent_getPointerId(event, i);
		size_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
		size_t pointerIndex = i;
		
		if (action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_POINTER_UP) {
			pointerIndex = (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			pointerId = AMotionEvent_getPointerId(event, pointerIndex);
		}

		engine->touchEventParamCache[0] = pointerId;
		engine->touchEventParamCache[1] = action;
		engine->touchEventParamCache[2] = AMotionEvent_getX(event, pointerIndex);
		engine->touchEventParamCache[3] = AMotionEvent_getY(event, pointerIndex);
		engine->touchEventParamCache[4] = engine->uptime.time;
		engine->touchEventParamCache[5] = engine->uptime.millitm;
		engine->touchEventParamCache[6] = AInputEvent_getDeviceId(event);
		engine->touchEventParamCache[7] = AInputEvent_getSource(event);
		
		if (callSqFunction_Bool_Floats(engine->sqvm, "onMotionEvent", engine->touchEventParamCache, MOTION_EVENT_PARAMS_SIZE, false)) {
			return 1;
		}
	}
	return 0;
}

/*
 * Process key event
 */
static int32_t emo_event_key(struct android_app* app, AInputEvent* event) {
	struct engine* engine = (struct engine*)app->userData;

	engine_update_uptime(engine);

	engine->keyEventParamCache[0] = AKeyEvent_getAction(event);
	engine->keyEventParamCache[1] = AKeyEvent_getKeyCode(event);
	engine->keyEventParamCache[2] = AKeyEvent_getRepeatCount(event);
	engine->keyEventParamCache[3] = AKeyEvent_getMetaState(event);
	engine->keyEventParamCache[4] = engine->uptime.time;
	engine->keyEventParamCache[5] = engine->uptime.millitm;
	engine->keyEventParamCache[6] = AInputEvent_getDeviceId(event);
	engine->keyEventParamCache[7] = AInputEvent_getSource(event);

	if (callSqFunction_Bool_Floats(engine->sqvm, "onKeyEvent", engine->keyEventParamCache, KEY_EVENT_PARAMS_SIZE, false)) {
		return 1;
	}
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
	callSqFunction(engine->sqvm, "onGainedFocus");
    engine->animating = 1;
}

/*
 * Lost focus
 */
void emo_lost_focus(struct engine* engine) {
	callSqFunction(engine->sqvm, "onLostFocus");
    engine->animating = 0;
}

/**
 * Command from main thread: the system is running low on memory.
 * Try to reduce your memory use.
 */
void emo_low_memory(struct engine* engine) {
	callSqFunction(engine->sqvm, "onLowMemory");
}

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
	/*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    context = eglCreateContext(display, config, NULL, NULL);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;

    emo_init_display(engine);
    
    return 0;
}
/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {
    if (engine->display == NULL) {
        return;
    }
    emo_draw_frame(engine);
    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {

        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}
/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            engine->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)engine->app->savedState) = engine->state;
            engine->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            if (engine->app->window != NULL) {
                engine_init_display(engine);
                engine_draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            emo_gained_focus(engine);
            break;
        case APP_CMD_LOST_FOCUS:
            emo_lost_focus(engine);
            break;
        case APP_CMD_LOW_MEMORY:
            emo_low_memory(engine);
            break;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    struct engine engine;

    app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = emo_handle_input;
    engine.app = state;

    if (state->savedState != NULL) {
        engine.state = *(struct saved_state*)state->savedState;
    }

    g_engine = &engine;

    /* Initialize the framework */
    emo_init_engine(&engine);

    while (1) {
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // Process the user queue.
            if (ident == LOOPER_ID_USER) {

            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                emo_dispose_engine(&engine);
                return;
            }
        }

        if (engine.animating) {
            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
            engine_draw_frame(&engine);
        }
    }
}
