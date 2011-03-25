#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#import "common.h"
#import "EmoEngine.h"
#import "sqfunc.h"

static BOOL enablePerspectiveNicest = TRUE;
static BOOL enableOnDrawFrame = FALSE;
static BOOL shouldImmediateUpdateStatus = FALSE;
static BOOL accelerometerSensorRegistered = FALSE;
static BOOL accelerometerShouldAlive = FALSE;
static int onDrawFrameInterval = 0;

/*
 * Logging
 */
void LOGI(const char* msg) {
	NSLog(@"%s INFO %s", EMO_LOG_TAG, msg);
}

void LOGE(const char* msg) {
	NSLog(@"%s ERROR %s", EMO_LOG_TAG, msg);
}

void LOGW(const char* msg) {
	NSLog(@"%s WARN %s", EMO_LOG_TAG, msg);
}

void NSLOGI(NSString* msg) {
	LOGI([msg UTF8String]);	
}

void NSLOGE(NSString* msg) {
	LOGE([msg UTF8String]);	
}

void NSLOGW(NSString* msg) {
	LOGW([msg UTF8String]);	
}

/*
 * import function called from squirrel script
 */
SQInteger emoImportScript(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
    	if (sq_gettype(v, n) == OT_STRING) {
    		const SQChar *fname;
            sq_tostring(v, n);
            sq_getstring(v, -1, &fname);
            sq_poptop(v);
			
    		[EmoEngine loadScriptFromResource:(const char*)fname vm:v];
    	}
    }
	return 0;
}

/*
 * update options
 */
static void emoUpdateOptions(SQInteger value) {
    switch(value) {
		case OPT_ENABLE_PERSPECTIVE_NICEST:
			enablePerspectiveNicest = TRUE;
			break;
		case OPT_ENABLE_PERSPECTIVE_FASTEST:
			enablePerspectiveNicest = FALSE;
			break;
		case OPT_WINDOW_KEEP_SCREEN_ON:
			[UIApplication sharedApplication].idleTimerDisabled = YES;
			break;
    }
}

/*
 * delcare to use sensors
 */
static void emoCreateSensors(SQInteger value) {
	switch(value) {
		case SENSOR_TYPE_ACCELEROMETER:
			[EmoEngine registerAccelerometerSensor:TRUE];
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
	
	[EmoEngine enableSensor:TRUE withType:sensorType withInterval:interval];
	
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
	
	[EmoEngine disableSensor:sensorType];
	
	return 0;
}

SQInteger emoEnableOnDrawCallback(HSQUIRRELVM v) {
	enableOnDrawFrame = TRUE;

	SQInteger nargs = sq_gettop(v);
	
    if (nargs <= 2 && sq_gettype(v, 2) == OT_INTEGER) {
        SQInteger interval;
        sq_getinteger(v, 2, &interval);
		
        onDrawFrameInterval = interval;
    }
	
	return 0;
}

SQInteger emoDisableOnDrawCallback(HSQUIRRELVM v) {
	enableOnDrawFrame = FALSE;
	return 0;
}

@interface EmoEngine (PrivateMethods)
- (void)initEngine;
- (void)updateEngineStatus;
- (NSTimeInterval)getLastOnDrawDelta;
@end

@implementation EmoEngine
@synthesize sqvm;
@synthesize lastError;
@synthesize isFrameInitialized;
@synthesize isRunning;

/*
 * register classes and functions for script
 */
- (void)initScriptFunctions {
    register_class(sqvm, SQUIRREL_RUNTIME_CLASS);
    register_class(sqvm, SQUIRREL_EVENT_CLASS);
	
    register_class_func(sqvm, SQUIRREL_RUNTIME_CLASS, "import", emoImportScript);
    register_class_func(sqvm, SQUIRREL_RUNTIME_CLASS, "setOptions", emoSetOptions);
    register_class_func(sqvm, SQUIRREL_EVENT_CLASS,   "registerSensors", emoRegisterSensors);
    register_class_func(sqvm, SQUIRREL_EVENT_CLASS,   "enableSensor",    emoEnableSensor);
    register_class_func(sqvm, SQUIRREL_EVENT_CLASS,   "disableSensor",   emoDisableSensor);
    register_class_func(sqvm, SQUIRREL_EVENT_CLASS,   "enableOnDrawCallback",  emoEnableOnDrawCallback);
    register_class_func(sqvm, SQUIRREL_EVENT_CLASS,   "disableOnDrawCallback", emoDisableOnDrawCallback);
	
}

/*
 * start the engine
 */
- (BOOL)startEngine {
	
	isFrameInitialized = FALSE;
	lastError = EMO_NO_ERROR;
	isRunning = TRUE;
	
	// engine startup time
	startTime = [[NSDate date] retain];
	lastOnDrawInterval = [self uptime];
	
	sqvm = sq_open(SQUIRREL_VM_INITIAL_STACK_SIZE);
	
	initSQVM(sqvm);
	
	[self initScriptFunctions];

	return TRUE;
}

/*
 * stop the engine
 */
- (BOOL)stopEngine {
	
	// disable "keep screen on"
	[UIApplication sharedApplication].idleTimerDisabled = NO;
	
	sq_close(sqvm);
	sqvm = nil;
	isRunning = FALSE;
	lastOnDrawInterval = 0;
	
	[startTime release];
	
	return TRUE;
}

/*
 * initialize OpenGL state 
 */
- (BOOL)initDrawFrame {

	if (!isRunning) {
		NSLOGE(@"The framework is not running: initDrawFrame");
		return FALSE;
	}
	if (isFrameInitialized) return FALSE;
	
    if (enablePerspectiveNicest) {
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
	
	isFrameInitialized = TRUE;
	
	return TRUE;
}

/*
 * load script from resource
 */
+(int)loadScriptFromResource:(const char*)chfname vm:(HSQUIRRELVM) v {
	NSString* fname = [[NSString alloc] initWithUTF8String:chfname];
	NSString* path = [[NSBundle mainBundle] pathForResource:fname ofType:nil];
	NSString* nscontent = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error: nil];
	
	[fname release];
	
	if (nscontent == nil) {
		return ERR_SCRIPT_OPEN;
	}
	
	const char* script = [nscontent UTF8String];
	const char* sourcename  = [path UTF8String];
	
	return sqCompileBuffer(v, script, sourcename);
}

/*
 * called when the app is loaded
 */ 
-(BOOL)onLoad {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onLoad");
		return FALSE;
	}
	BOOL sqResult = callSqFunction(sqvm, "onLoad");

	[self updateEngineStatus];

	return sqResult;
}

/*
 * called when the app gained focus
 */
-(BOOL)onGainedFocus {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onGainedFocus");
		return FALSE;
	}
	BOOL sqResult = callSqFunction(sqvm, "onGainedFocus");

	[self updateEngineStatus];

	return sqResult;
}

/*
 * called when the app requests drawing
 */
-(BOOL)onDrawFrame {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onDrawFrame");
		return FALSE;
	}
	
	if (shouldImmediateUpdateStatus) {
		[self updateEngineStatus];
		shouldImmediateUpdateStatus = FALSE;
	}
	
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
	
	if (enableOnDrawFrame && [self getLastOnDrawDelta] > onDrawFrameInterval) {
		lastOnDrawInterval = [self uptime];
		return callSqFunction(sqvm, "onDrawFrame");
	}
	
	return FALSE;
}

/*
 * returns last ondraw delta (msec)
 */
-(NSTimeInterval)getLastOnDrawDelta {
	return ([self uptime] - lastOnDrawInterval) * 1000;
}

/*
 * called when the app lost focus
 */
-(BOOL)onLostFocus {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onLostFocus");
		return FALSE;
	}
	BOOL sqResult = callSqFunction(sqvm, "onLostFocus");

	[self updateEngineStatus];

	return sqResult;
}

/*
 * called when the app will be disposed
 */
-(BOOL)onDispose {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onDispose");
		return FALSE;
	}
	BOOL sqResult = callSqFunction(sqvm, "onDispose");

	[self updateEngineStatus];

	return sqResult;
}

/*
 * called when the memory is low
 */
-(BOOL)onLowMemory {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onLowMemory");
		return FALSE;
	}
	BOOL sqResult = callSqFunction(sqvm, "onLowMemory");
	
	[self updateEngineStatus];
	
	return sqResult;
}

/*
 * called when the touch event occurs
 */
-(BOOL)onMotionEvent:(float *)param {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onMotionEvent");
		return FALSE;
	}
	return callSqFunction_Bool_Floats(sqvm, "onMotionEvent", param, MOTION_EVENT_PARAMS_SIZE, FALSE);
}

/*
 * called when the key event occurs
 */
-(BOOL)onKeyEvent:(float *)param {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onKeyEvent");
		return FALSE;
	}
	return callSqFunction_Bool_Floats(sqvm, "onKeyEvent", param, KEY_EVENT_PARAMS_SIZE, FALSE);	
}

/*
 * accelerometer event
 */
-(void)accelerometer:(UIAccelerometer *)accelerometer
	   didAccelerate:(UIAcceleration *)acceleration {
	accelerometerEventParamCache[0] = SENSOR_TYPE_ACCELEROMETER;
	accelerometerEventParamCache[1] = acceleration.x;
	accelerometerEventParamCache[2] = acceleration.y;
	accelerometerEventParamCache[3] = acceleration.z;
	
	callSqFunction_Bool_Floats(sqvm, "onSensorEvent",
			accelerometerEventParamCache, ACCELEROMETER_EVENT_PARAMS_SIZE, FALSE);	
}

/*
 * returns uptime of the engine
 */
-(NSTimeInterval)uptime {
	if (!isRunning) {
		return 0.0f;
	}
	return [[NSDate date] timeIntervalSinceDate:startTime];
}

/*
 * update status of the engine
 * this method enables/disables sensors
 */
-(void)updateEngineStatus {
	// enable/disable accelerometerSensor
	if (accelerometerShouldAlive && accelerometerSensor == nil) {
		accelerometerSensor = [UIAccelerometer sharedAccelerometer];
		accelerometerSensor.delegate = self;
    } else if (!accelerometerShouldAlive && accelerometerSensor != nil) {
		accelerometerSensor.delegate = nil;
		accelerometerSensor = nil;
	}
}

/*
 * register accelerometer sensor
 */
+(void)registerAccelerometerSensor:(BOOL)enable {
	accelerometerSensorRegistered = enable;
}

/*
 * enable sensor with updateInterval
 */
+ (void)enableSensor:(BOOL)enable withType:(NSInteger)sensorType withInterval:(int)updateInterval {
	if (sensorType == SENSOR_TYPE_ACCELEROMETER) {
		if (enable && accelerometerSensorRegistered) {
			accelerometerShouldAlive = TRUE;
			[UIAccelerometer sharedAccelerometer].updateInterval = updateInterval / 1000.0f;
		} else {
			accelerometerShouldAlive = FALSE;
		}
	}
}

/*
 * disable sensor
 */
+ (void)disableSensor:(NSInteger)sensorType {
	[self enableSensor:FALSE withType:sensorType withInterval:0.1f];
}

/*
 * update engine status immediately
 */
+ (void)updateStatusImmediately {
	shouldImmediateUpdateStatus = TRUE;
}
@end