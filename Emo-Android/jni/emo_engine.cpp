#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android_native_app_glue.h>

#include <squirrel.h>
#include <common.h>
#include <main.h>
#include <sqfunc.h>

/*
 * extern common functions
 */
extern void LOGI(const SQChar* msg);
extern void LOGW(const SQChar* msg);
extern void LOGE(const SQChar* msg);

extern SQInteger emoImportScript(HSQUIRRELVM v);
extern SQInteger emoSetOptions(HSQUIRRELVM v);
extern SQInteger emoRegisterSensors(HSQUIRRELVM v);
extern SQInteger emoEnableSensor(HSQUIRRELVM v);
extern SQInteger emoDisableSensor(HSQUIRRELVM v);

extern void      emoUpdateOptions(SQInteger value);

extern SQBool loadScriptFromAsset(const char* fname);

/*
 * register class and functions for script
 */
static void initScriptFunctions(struct engine* engine) {
    register_class(engine->sqvm, SQUIRREL_RUNTIME_CLASS);
    register_class(engine->sqvm, SQUIRREL_EVENT_CLASS);

    register_class_func(engine->sqvm, SQUIRREL_RUNTIME_CLASS, "import",          emoImportScript);
    register_class_func(engine->sqvm, SQUIRREL_RUNTIME_CLASS, "setOptions",      emoSetOptions);
    register_class_func(engine->sqvm, SQUIRREL_EVENT_CLASS,   "registerSensors", emoRegisterSensors);
    register_class_func(engine->sqvm, SQUIRREL_EVENT_CLASS,   "enableSensor",    emoEnableSensor);
    register_class_func(engine->sqvm, SQUIRREL_EVENT_CLASS,   "disableSensor",   emoDisableSensor);
}

void engine_update_uptime(struct engine* engine) {
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

    // register class and functions for script
    initScriptFunctions(engine);

    // load runtime and main script
    loadScriptFromAsset(SQUIRREL_RUNTIME_SCRIPT);
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
int32_t emo_event_motion(struct android_app* app, AInputEvent* event) {
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
int32_t emo_event_key(struct android_app* app, AInputEvent* event) {
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

/*
 * handle sensor event TODO
 */
void emo_event_sensors(struct engine* engine, ASensorEvent* event) {
   LOGI("engine_handle_sensors: not implemented yet.");
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

