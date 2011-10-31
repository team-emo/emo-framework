// Copyright (c) 2011 emo-framework project
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the project nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
#include "sys/sysctl.h"

#import "OpenGLES/ES2/glext.h"

#import "Constants.h"
#import "VmFunc.h"
#import "EmoRuntime.h"
#import "EmoEngine_glue.h"
#import "EmoDrawable_glue.h"
#import "EmoAudio_glue.h"
#import "EmoDatabase_glue.h"
#import "EmoEngine.h"
#import "EmoDrawable.h"
#import "EmoPhysics.h"

NSString* char2ns(const char* str) {
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
-(void)stopOffscreenDrawable:(EmoDrawable*)drawable;
@end

@implementation EmoEngine
@synthesize sqvm;
@synthesize lastError;
@synthesize isFrameInitialized;
@synthesize isRunning;
@synthesize sortOrderDirty;
@synthesize enablePerspectiveNicest;
@synthesize enableOnDrawFrame;
@synthesize onDrawFrameInterval;
@synthesize onDrawDrawablesInterval;
@synthesize audioManager;
@synthesize stage;
@synthesize database;
@synthesize currentOrientation;
@synthesize logLevel;
@synthesize enableSimpleLog, enableSimpleLogWithLevel;
@synthesize useOffscreen, stopOffscreenRequested;

- (id)init {
    self = [super init];
    if (self != nil) {
		currentOrientation = OPT_ORIENTATION_UNSPECIFIED;
		logLevel = LOG_INFO;
        enableSimpleLog = FALSE;
        enableSimpleLogWithLevel = FALSE;
        
        sqvm = sq_open(SQUIRREL_VM_INITIAL_STACK_SIZE);
    }
    return self;
}

/*
 * register classes and functions for script
 */
- (void)initScriptFunctions {
	register_table(sqvm, EMO_NAMESPACE);
	
	initRuntimeFunctions();
	initDrawableFunctions();
	initAudioFunctions();
	initPhysicsFunctions();
	initDatabaseFunctions();
}

/*
 * start the engine
 */
- (BOOL)startEngine:(GLint)width withHeight:(GLint)height  {
	
	isFrameInitialized = FALSE;
	lastError = EMO_NO_ERROR;
	isRunning = TRUE;
	sortOrderDirty = TRUE;
	stopwatchStarted = FALSE;
	
	enablePerspectiveNicest = TRUE;
	enableOnDrawFrame = FALSE;
	accelerometerSensorRegistered = FALSE;
	enableOnUpdate = FALSE;
	
	audioManager = [[EmoAudioManager alloc]init];
	drawables    = [[NSMutableDictionary alloc]init];
	stage        = [[EmoStage alloc]init];
	netTasks     = [[NSMutableDictionary alloc]init];
	database     = [[EmoDatabase alloc]init];
	imageCache   = [[NSMutableDictionary alloc]init];
	
    [stage setBufferSize:width height:height];
	[stage setSize:width height:height];
	
	// engine startup time
	startTime = [[NSDate date] retain];
	
	onDrawFrameInterval = 0;
	onDrawDrawablesInterval = 0;
	lastOnDrawInterval = [self uptime];
	lastOnDrawDrawablesInterval = [self uptime];
	
	frameCount         = 0;
	onFpsInterval      = 0;
	onFpsIntervalDelta = 0;
	enableOnFps        = FALSE;
    
    useOffscreen = FALSE;
    offscreenFramebuffer = 0;
    stopOffscreenRequested = FALSE;
	
	drawablesToDraw  = [NSArray alloc];
	
    if (sqvm == nil) {
        sqvm = sq_open(SQUIRREL_VM_INITIAL_STACK_SIZE);
    }
	
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
	
    if (useOffscreen) [self disableOffscreen];
    
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
	[imageCache release];
	
	[netTasks release];
	netTasks = nil;
	
	[database release];
	database = nil;
	
	return TRUE;
}

/*
 * enable offscreen rendering
 */
- (void)enableOffscreen {
    if (!useOffscreen && offscreenFramebuffer == 0) {
        glGenFramebuffers(1, &offscreenFramebuffer);
    }
    useOffscreen = TRUE;
    stopOffscreenRequested = FALSE;
}

/*
 * disable offscreen rendering
 */
- (void)disableOffscreen {
    if (useOffscreen && offscreenFramebuffer != 0) {
        glDeleteFramebuffers(1, &offscreenFramebuffer);
        offscreenFramebuffer = 0;
    }
    useOffscreen = FALSE;
    stopOffscreenRequested = FALSE;
}

/*
 * bind offscreen framebuffer
 */
- (void)bindOffscreenFramebuffer {
    if (offscreenFramebuffer > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, offscreenFramebuffer);
    }
}

/*
 * unbind offscreen framebuffer
 */
- (void)unbindOffscreenFramebuffer {
    if (offscreenFramebuffer > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
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
	
    glDisable(GL_CULL_FACE);
	
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

	// load stage
	[stage loadBuffer];
	
	isFrameInitialized = TRUE;
	
	lastOnDrawInterval = [self uptime];
	lastOnDrawDrawablesInterval = [self uptime];
	
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
		LOGE("Script resource is not found:");
		LOGE(chfname);
		return ERR_SCRIPT_OPEN;
	}
    
    return [self loadScript:path vm:v];
}

/*
 * callback function to read squirrel script
 */
static SQInteger sq_lexer_bytecode(SQUserPointer file, SQUserPointer buf, SQInteger size) {
    NSData* data = [(NSFileHandle*)file readDataOfLength:size];
    NSUInteger len = [data length];
    if (len > 0) {
        [data getBytes:buf length:len];
        return len;
    } else {
        return -1;
    }
}

/*
 * load script file (full path)
 */
-(int)loadScript:(NSString *)path vm:(HSQUIRRELVM) v {
	NSFileManager* manager = [NSFileManager defaultManager];
	if (![manager fileExistsAtPath:path]) {
		LOGE("Script file is not found:");
		NSLOGE(path);
		return ERR_SCRIPT_OPEN;
    }

    NSFileHandle* file = [NSFileHandle fileHandleForReadingAtPath: path];
	if (file == nil) {
		LOGE("Script file is not found:");
		NSLOGE(path);
		return ERR_SCRIPT_OPEN;
	}
    
    unsigned short magic_number;
    [[file readDataOfLength: 2] getBytes:&magic_number length:2];
    [file seekToFileOffset:0];
    
    // bytecode
    if (magic_number == SQ_BYTECODE_STREAM_TAG) {
        if (SQ_SUCCEEDED(sq_readclosure(v, sq_lexer_bytecode, file))) {
            
            [file closeFile];
            
            sq_pushroottable(v);
            if (SQ_FAILED(sq_call(v, 1, SQFalse, SQTrue))) {
                LOGE("Failed to call root closure:");
                NSLOGE(path);
                return ERR_SCRIPT_CALL_ROOT;
            } else {
                return EMO_NO_ERROR;
            }
        } else {
            
            [file closeFile];
            
            LOGE("Failed to read bytecode content:");
            NSLOGE(path);
            return ERR_SCRIPT_CALL_ROOT;
        }
    }  
    [file closeFile];
    
	NSString* nscontent = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error: nil];
	if (nscontent == nil) {
		LOGE("Script content is not found:");
		NSLOGE(path);
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
-(BOOL)onDrawFrame:(GLuint)framebuffer {
	if (!isRunning) {
		return FALSE;
	}
    
	if (sortOrderDirty) {
		[drawablesToDraw release];
		drawablesToDraw = [[[drawables allValues] sortedArrayUsingSelector:@selector(compareZ:)] retain];
		sortOrderDirty = FALSE;
	}
	
	if (enableOnUpdate) {
		NSTimeInterval _delta = [self getLastOnDrawDrawablesDelta];
		callSqFunction_Bool_Float(sqvm, EMO_NAMESPACE, EMO_FUNC_ON_UPDATE, _delta, SQFalse);
	}
	
	NSTimeInterval delta = [self getLastOnDrawDelta];
	if (enableOnDrawFrame && delta > onDrawFrameInterval) {
		lastOnDrawInterval = [self uptime];
		callSqFunction_Bool_Float(sqvm, EMO_NAMESPACE, EMO_FUNC_ONDRAW_FRAME, delta, SQFalse);
	}

	// check if the engine have to continue to draw
	delta = [self getLastOnDrawDrawablesDelta];
	if (delta < onDrawDrawablesInterval) {
		return FALSE;
	}
	
	if (enableOnFps) {
		frameCount++;
		onFpsIntervalDelta += delta;
		if (onFpsIntervalDelta >= onFpsInterval) {
			float fps = 1000.0 / (onFpsIntervalDelta / (float)frameCount);
			callSqFunction_Bool_Float(sqvm, EMO_NAMESPACE, EMO_FUNC_ON_FPS, fps, SQFalse);
			onFpsIntervalDelta = 0;
			frameCount         = 0;
		}
	}
	
	lastOnDrawDrawablesInterval = [self uptime];
	if (!useOffscreen) [stage onDrawFrame:delta];
	for (int i = 0; i < [drawablesToDraw count]; i++) {
        // if offscreen is enabled, the first drawable is
        // the snapshot drawable that should be drawn last.
		EmoDrawable* drawable = [drawablesToDraw objectAtIndex:i];
        if (useOffscreen && i == 0 && !drawable.isScreenEntity) {
            [self bindOffscreenFramebuffer];
            [stage onDrawFrame:delta];
            continue;
        }
		if (drawable.loaded && drawable.independent && [drawable isVisible]) {
			[drawable onDrawFrame:delta withStage:stage];
		}
	}
    
    // render the offscreen result
    if (useOffscreen && [drawablesToDraw count] > 0) {
        [self unbindOffscreenFramebuffer];
        // restore the default framebuffer
		EmoDrawable* drawable = [drawablesToDraw objectAtIndex:0];
		if (drawable.loaded && !drawable.isScreenEntity) {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			[drawable onDrawFrame:delta withStage:stage];
            if (stopOffscreenRequested) {
                stopOffscreenRequested = FALSE;
                [self stopOffscreenDrawable:drawable];
                callSqFunction_Bool_Float(sqvm, EMO_NAMESPACE, EMO_FUNC_ONSTOP_OFFSCREEN, delta, SQFalse);
            }
		}
    }
	
	return FALSE;
}

/*
 * stop offscreen
 */
-(void)stopOffscreenDrawable:(EmoDrawable*)drawable {
    drawable.width  = stage.width;
    drawable.height = stage.height;
    
    // fix rotation and scale center
    [drawable setRotate:1 withValue:drawable.width  * 0.5f];
    [drawable setRotate:2 withValue:drawable.height * 0.5f];
    [drawable setScale:2  withValue:drawable.width  * 0.5f];
    [drawable setScale:3  withValue:drawable.height * 0.5f];
    
    [self disableOffscreen];
    stage.dirty = TRUE;
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
 * only accelerometer is supported for iOS.
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

	EmoDrawable* drawable = [drawables objectForKey:_key];
	[drawable doUnload:TRUE];
	[drawables removeObjectForKey:_key];

	sortOrderDirty = TRUE;
	return TRUE;
	
}
-(void)clearDrawables {
	
	for (NSString* key in drawables) {
		[[drawables objectForKey:key] doUnload:TRUE];
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
	
    [net release];
    
	return net;
}

-(void)removeNetTask:(NSString*)taskName {
	[netTasks removeObjectForKey:taskName];
}

-(void)stopwatchStart {
	stopwatchStartTime = [self uptime];
	stopwatchStarted = TRUE;
}

-(void)stopwatchStop {
	stopwatchElapsedTime = [self uptime] - stopwatchStartTime;
	stopwatchStarted = FALSE;
}

-(NSInteger)stopwatchElapsed {
	if (stopwatchStarted) {
		stopwatchElapsedTime = [self uptime] - stopwatchStartTime;
	}
	return (NSInteger)(stopwatchElapsedTime * 1000);
}

-(void)enableOnUpdateListener:(BOOL)enable {
	enableOnUpdate = enable;
}

-(void)enableOnFpsListener:(BOOL)enable {
	frameCount  = 0;
	enableOnFps = enable;
}

-(void)setOnFpsListenerInterval:(NSInteger)value {
	onFpsInterval = value;
}

-(BOOL)hasCachedImage:(NSString*)key {
	return [imageCache objectForKey:key] != nil;
}

-(EmoImage*)getCachedImage:(NSString*)key {
	return [imageCache objectForKey:key];
}

-(void)addCachedImage:(NSString*)key value:(EmoImage*)image {
	[imageCache setObject:image forKey:key];
}

-(void)removeCachedImage:(NSString*)key {
	[imageCache removeObjectForKey:key];
}

-(void)clearImageCache {
	for (NSString* key in imageCache) {
		[[imageCache objectForKey:key] freeData];
	}
	
	[imageCache removeAllObjects];
}

- (NSString*)getDeviceName {
	size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = malloc(size);
	sysctlbyname("hw.machine", machine, &size, NULL, 0);
	NSString *platform = [NSString stringWithCString:machine encoding: NSUTF8StringEncoding];
	free(machine);
	return platform;
}

- (void)updateOnDrawFrameInterval:(NSInteger)interval {
    onDrawFrameInterval = interval;
    lastOnDrawInterval = [self uptime];
}

- (void)updateOnDrawDrawablesInterval:(NSInteger)interval {
    onDrawDrawablesInterval = interval;
    lastOnDrawDrawablesInterval = [self uptime];
}


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	
	if (currentOrientation == OPT_ORIENTATION_UNSPECIFIED) {
		return NO;
	}
	if (currentOrientation == OPT_ORIENTATION_LANDSCAPE &&
			interfaceOrientation == UIInterfaceOrientationLandscapeLeft) {
		return YES;
	}
	if (currentOrientation == OPT_ORIENTATION_LANDSCAPE_LEFT &&
        interfaceOrientation == UIInterfaceOrientationLandscapeLeft) {
		return YES;
	}
	if (currentOrientation == OPT_ORIENTATION_LANDSCAPE_RIGHT &&
        interfaceOrientation == UIInterfaceOrientationLandscapeRight) {
		return YES;
	}
	if (currentOrientation == OPT_ORIENTATION_PORTRAIT &&
		interfaceOrientation == UIInterfaceOrientationPortrait) {
		return YES;
	}
	
	return NO;
}
@end
