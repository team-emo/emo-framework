#import "Constants.h"
#import "VmFunc.h"
#import "EmoRuntime.h"
#import "EmoEngine_glue.h"
#import "EmoDrawable_glue.h"
#import "EmoAudio_glue.h"
#import "EmoEngine.h"
#import "EmoDrawable.h"


NSString* char2ns(const SQChar* str) {
	return [NSString stringWithCString:(char*)str 
							  encoding:NSUTF8StringEncoding];
}

NSString* data2ns(NSData* data) {
	return [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
}

@interface EmoEngine (PrivateMethods)
- (void)initEngine;
- (void)updateEngineStatus;
- (NSTimeInterval)getLastOnDrawDelta;
- (NSTimeInterval)getLastOnDrawDrawablesDelta;
@end

@implementation EmoEngine
@synthesize sqvm;
@synthesize lastError;
@synthesize lastCallbackErrorMessage;
@synthesize lastCallbackErrorType;
@synthesize isFrameInitialized;
@synthesize isRunning;
@synthesize sortOrderDirty;
@synthesize enablePerspectiveNicest;
@synthesize enableOnDrawFrame;
@synthesize onDrawFrameInterval;
@synthesize onDrawDrawablesInterval;
@synthesize audioManager;
@synthesize stage;

/*
 * register classes and functions for script
 */
- (void)initScriptFunctions {
	register_table(sqvm, EMO_NAMESPACE);
	
	initRuntimeFunctions();
	initDrawableFunctions();
	initAudioFunctions();
}

/*
 * start the engine
 */
- (BOOL)startEngine:(GLint)width withHeight:(GLint)height  {
	
	isFrameInitialized = FALSE;
	lastError = EMO_NO_ERROR;
	isRunning = TRUE;
	sortOrderDirty = TRUE;
	
	enablePerspectiveNicest = TRUE;
	enableOnDrawFrame = FALSE;
	accelerometerSensorRegistered = FALSE;
	
	audioManager = [[EmoAudioManager alloc]init];
	drawables    = [[NSMutableDictionary alloc]init];
	stage        = [[EmoStage alloc]init];
	netTasks     = [[NSMutableDictionary alloc]init];
	
	[stage setSize:width height:height];
	
	// engine startup time
	startTime = [[NSDate date] retain];
	lastOnDrawInterval = [self uptime];
	lastOnDrawDrawablesInterval = [self uptime];
	
	onDrawFrameInterval = 0;
	onDrawDrawablesInterval = 0;
	
	drawablesToDraw  = [NSArray alloc];
	
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
	lastOnDrawDrawablesInterval = 0;
	
	[audioManager closeEngine];
	[audioManager release];
	audioManager = nil;

	[self clearDrawables];
	[drawables release];
	drawables = nil;
	
	[stage unloadBuffer];
	[stage release];
	stage = nil;
	
	[startTime release];
	[drawablesToDraw release];
	
	[netTasks release];
	netTasks = nil;
	
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
	
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
	
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

	// load stage
	[stage loadBuffer];
	
	isFrameInitialized = TRUE;
	
	return TRUE;
}

/*
 * load script from resource
 */
-(int)loadScriptFromResource:(const char*)chfname vm:(HSQUIRRELVM) v {
	NSString* fname = [[NSString alloc] initWithUTF8String:chfname];
	NSString* path = [[NSBundle mainBundle] pathForResource:fname ofType:nil];
	[fname release];
	if (path == nil) {
		LOGE("Script resource does not found:");
		LOGE(chfname);
		return ERR_SCRIPT_OPEN;
	}
	
	NSString* nscontent = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error: nil];
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
	return callSqFunction(sqvm, EMO_NAMESPACE, EMO_FUNC_ONLOAD);
}

/*
 * called when the app gained focus
 */
-(BOOL)onGainedFocus {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onGainedFocus");
		return FALSE;
	}
	return callSqFunction(sqvm, EMO_NAMESPACE, EMO_FUNC_ONGAINED_FOUCS);
}

/*
 * called when the app requests drawing
 */
-(BOOL)onDrawFrame {
	if (!isRunning) {
		return FALSE;
	}
	
	if (sortOrderDirty) {
		[drawablesToDraw release];
		drawablesToDraw = [[[drawables allValues] sortedArrayUsingSelector:@selector(compareZ:)] retain];
		sortOrderDirty = FALSE;
	}
	
	NSTimeInterval delta = [self getLastOnDrawDelta];
	if (enableOnDrawFrame && delta > onDrawFrameInterval) {
		lastOnDrawInterval = [self uptime];
		return callSqFunction_Bool_Float(sqvm, EMO_NAMESPACE, EMO_FUNC_ONDRAW_FRAME, delta, SQFalse);
	}

	// check if the engine have to continue to draw
	delta = [self getLastOnDrawDrawablesDelta];
	if (delta < onDrawDrawablesInterval) {
		return FALSE;
	}

	lastOnDrawDrawablesInterval = [self uptime];
	[stage onDrawFrame:delta];
	for (int i = 0; i < [drawablesToDraw count]; i++) {
		EmoDrawable* drawable = [drawablesToDraw objectAtIndex:i];
		if (drawable.loaded && drawable.independent) {
			[drawable onDrawFrame:delta withStage:stage];
		}
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
 * returns last ondraw drawables delta (msec)
 */
-(NSTimeInterval)getLastOnDrawDrawablesDelta {
	return ([self uptime] - lastOnDrawDrawablesInterval) * 1000;
}

/*
 * called when the app lost focus
 */
-(BOOL)onLostFocus {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onLostFocus");
		return FALSE;
	}
	return callSqFunction(sqvm, EMO_NAMESPACE, EMO_FUNC_ONLOST_FOCUS);
}

/*
 * called when the app will be disposed
 */
-(BOOL)onDispose {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onDispose");
		return FALSE;
	}
	return callSqFunction(sqvm, EMO_NAMESPACE, EMO_FUNC_ONDISPOSE);
}

/*
 * called when the memory is low
 */
-(BOOL)onLowMemory {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onLowMemory");
		return FALSE;
	}
	return callSqFunction(sqvm, EMO_NAMESPACE, EMO_FUNC_ONLOW_MEMORY);
}

/*
 * called when the touch event occurs
 */
-(BOOL)onMotionEvent:(float *)param {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onMotionEvent");
		return FALSE;
	}
	return callSqFunction_Bool_Floats(sqvm, EMO_NAMESPACE,
				EMO_FUNC_MOTIONEVENT, param, MOTION_EVENT_PARAMS_SIZE, FALSE);
}

/*
 * called when the key event occurs
 */
-(BOOL)onKeyEvent:(float *)param {
	if (!isRunning) {
		NSLOGE(@"The framework is not running: onKeyEvent");
		return FALSE;
	}
	return callSqFunction_Bool_Floats(sqvm, EMO_NAMESPACE,
				EMO_FUNC_KEYEVENT, param, KEY_EVENT_PARAMS_SIZE, FALSE);	
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
	
	callSqFunction_Bool_Floats(sqvm, EMO_NAMESPACE, EMO_FUNC_SENSOREVENT,
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
 * register accelerometer sensor
 */
-(void)registerAccelerometerSensor:(BOOL)enable {
	accelerometerSensorRegistered = enable;
}

/*
 * enable sensor with updateInterval
 */
- (void)enableSensor:(BOOL)enable withType:(NSInteger)sensorType withInterval:(int)updateInterval {
	if (sensorType == SENSOR_TYPE_ACCELEROMETER) {
		if (enable && accelerometerSensorRegistered) {
			accelerometerSensor = [UIAccelerometer sharedAccelerometer];
			accelerometerSensor.delegate = self;
			[UIAccelerometer sharedAccelerometer].updateInterval = updateInterval / 1000.0f;
		} else {
			accelerometerSensor.delegate = nil;
			accelerometerSensor = nil;
		}
	}
}

/*
 * disable sensor
 */
- (void)disableSensor:(NSInteger)sensorType {
	[self enableSensor:FALSE withType:sensorType withInterval:0.1f];
}

- (EmoDrawable*)getDrawable:(const char*)key {
	return [drawables objectForKey:char2ns(key)];
}

-(BOOL)removeDrawable:(const char*)key {
	NSString* _key = char2ns(key);

	[_key retain];
	EmoDrawable* drawable = [drawables objectForKey:_key];
	[drawable doUnload];
	[drawable release];
	[drawables removeObjectForKey:_key];
	[_key release];
	
	sortOrderDirty = TRUE;
	return TRUE;
	
}
-(void)clearDrawables {
	
	for (NSString* key in drawables) {
		[[drawables objectForKey:key] doUnload];
		[[drawables objectForKey:key] release];
	}
	
	[drawables removeAllObjects];
	
	sortOrderDirty = TRUE;
}

-(void)addDrawable:(EmoDrawable*)drawable withKey:(const char*)key {
	[drawables setObject:drawable forKey: char2ns(key)];
	sortOrderDirty = TRUE;
}

-(EmoNetTask*)createNetTask:(NSString*)taskName {
	EmoNetTask* net = [[EmoNetTask alloc] init];
	net.name = taskName;
	
	[netTasks setObject:net forKey:taskName];
	
	return net;
}

-(void)removeNetTask:(NSString*)taskName {
	EmoNetTask* netTask = [netTasks objectForKey:taskName];
	[netTask release];
	[netTasks removeObjectForKey:taskName];
}

@end