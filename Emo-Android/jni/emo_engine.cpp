#include <stdio.h>
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
extern SQInteger emoRuntimeEcho(HSQUIRRELVM v);
extern SQInteger emoRuntimeLog(HSQUIRRELVM v);
extern SQInteger emoRuntimeLogInfo(HSQUIRRELVM v);
extern SQInteger emoRuntimeLogError(HSQUIRRELVM v);
extern SQInteger emoRuntimeLogWarn(HSQUIRRELVM v);

extern SQInteger emoRegisterSensors(HSQUIRRELVM v);
extern SQInteger emoEnableSensor(HSQUIRRELVM v);
extern SQInteger emoDisableSensor(HSQUIRRELVM v);
extern SQInteger emoEnableOnDrawCallback(HSQUIRRELVM v);
extern SQInteger emoDisableOnDrawCallback(HSQUIRRELVM v);

extern SQInteger emoDrawableCreate(HSQUIRRELVM v);
extern SQInteger emoDrawableMove(HSQUIRRELVM v);
extern SQInteger emoDrawableScale(HSQUIRRELVM v);
extern SQInteger emoDrawableRotate(HSQUIRRELVM v);

extern void      emoUpdateOptions(SQInteger value);

extern SQBool loadScriptFromAsset(const char* fname);

/*
 * Register new class for emo framework
 */
void registerEmoClass(HSQUIRRELVM v, const char *cname) {
    register_class_with_namespace(v, EMO_NAMESPACE, cname);
}

/*
 * Register class method for emo framework
 */
void registerEmoClassFunc(HSQUIRRELVM v, const char *cname, const char *fname, SQFUNCTION func) {
	register_class_func_with_namespace(
			v, EMO_NAMESPACE, cname, fname, func);
}

/*
 * register class and functions for script
 */
static void initScriptFunctions(struct engine* engine) {

	register_table(engine->sqvm, EMO_NAMESPACE);
	registerEmoClass(engine->sqvm, EMO_RUNTIME_CLASS);
	registerEmoClass(engine->sqvm, EMO_EVENT_CLASS);
	registerEmoClass(engine->sqvm, EMO_DRAWABLE_CLASS);

	registerEmoClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "import",          emoImportScript);
	registerEmoClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "setOptions",      emoSetOptions);
	registerEmoClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "echo",            emoRuntimeEcho);
	registerEmoClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "log",             emoRuntimeLog);
	registerEmoClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "info",            emoRuntimeLogInfo);
	registerEmoClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "error",           emoRuntimeLogError);
	registerEmoClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "warn",            emoRuntimeLogWarn);

	registerEmoClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "registerSensors", emoRegisterSensors);
	registerEmoClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "enableSensor",    emoEnableSensor);
	registerEmoClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "disableSensor",   emoDisableSensor);
	registerEmoClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "enableOnDrawCallback",  emoEnableOnDrawCallback);
	registerEmoClassFunc(engine->sqvm, EMO_EVENT_CLASS,   "disableOnDrawCallback", emoDisableOnDrawCallback);

	registerEmoClassFunc(engine->sqvm, EMO_DRAWABLE_CLASS, "constructor",    emoDrawableCreate);
	registerEmoClassFunc(engine->sqvm, EMO_DRAWABLE_CLASS, "move",           emoDrawableMove);
	registerEmoClassFunc(engine->sqvm, EMO_DRAWABLE_CLASS, "scale",          emoDrawableScale);
	registerEmoClassFunc(engine->sqvm, EMO_DRAWABLE_CLASS, "rotate",         emoDrawableRotate);
}

void engine_update_uptime(struct engine* engine) {
    timeb eventTime;
    ftime(&eventTime);

    engine->uptime.time     = eventTime.time - engine->startTime.time;
    engine->uptime.millitm  = eventTime.millitm;
}

int32_t engine_getLastOnDrawDelta(struct engine* engine) {
    int32_t deltaSec  = engine->uptime.time - engine->lastOnDrawInterval.time;
    int32_t deltaMsec = engine->uptime.millitm - engine->lastOnDrawInterval.millitm;

    return (deltaSec * 1000) + deltaMsec;
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
    engine->enableOnDrawFrame   = false;
    engine->onDrawFrameInterval = 0;
    engine->lastOnDrawInterval  = engine->uptime;

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
    engine_update_uptime(engine);

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
    engine_update_uptime(engine);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if (engine->enableOnDrawFrame && engine_getLastOnDrawDelta(engine) > engine->onDrawFrameInterval) {
        engine->lastOnDrawInterval  = engine->uptime;
        callSqFunction(engine->sqvm, "onDrawFrame");
    }
}

/*
 * Terminate the framework
 */
void emo_dispose_engine(struct engine* engine) {
    engine_update_uptime(engine);
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
		
		if (callSqFunction_Bool_Floats(engine->sqvm, EMO_FUNC_MOTIONEVENT, engine->touchEventParamCache, MOTION_EVENT_PARAMS_SIZE, false)) {
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

	if (callSqFunction_Bool_Floats(engine->sqvm, EMO_FUNC_KEYEVENT, engine->keyEventParamCache, KEY_EVENT_PARAMS_SIZE, false)) {
		return 1;
	}
    return 0;
}

/*
 * handle sensor event
 */
int32_t emo_event_sensors(struct engine* engine, ASensorEvent* event) {
    engine_update_uptime(engine);
    switch(event->sensor) {
    case ASENSOR_TYPE_ACCELEROMETER:
        engine->accelerometerEventParamCache[0] = event->sensor;
        engine->accelerometerEventParamCache[1] = event->acceleration.x;
        engine->accelerometerEventParamCache[2] = event->acceleration.y;
        engine->accelerometerEventParamCache[3] = event->acceleration.z;
        if (callSqFunction_Bool_Floats(engine->sqvm, "onSensorEvent", engine->accelerometerEventParamCache, ACCELEROMETER_EVENT_PARAMS_SIZE, false)) {
            return 1;
        }
        break;
    }
    return 0;
}

/*
 * Gained focus
 */
void emo_gained_focus(struct engine* engine) {
    engine_update_uptime(engine);
    callSqFunction(engine->sqvm, "onGainedFocus");
    engine->animating = 1;
}

/*
 * Lost focus
 */
void emo_lost_focus(struct engine* engine) {
    engine_update_uptime(engine);
    callSqFunction(engine->sqvm, "onLostFocus");
    engine->animating = 0;
}

/**
 * Command from main thread: the system is running low on memory.
 * Try to reduce your memory use.
 */
void emo_low_memory(struct engine* engine) {
    engine_update_uptime(engine);
    callSqFunction(engine->sqvm, "onLowMemory");
}

