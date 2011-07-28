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
#import "squirrel.h"
#import "Constants.h"
#import "EmoAudio.h"
#import "EmoDrawable.h"
#import "EmoStage.h"
#import "EmoRuntime.h"
#import "EmoDatabase.h"

NSString* char2ns(const char* str);
NSString* data2ns(NSData* data);

@interface EmoEngine : NSObject<UIAccelerometerDelegate> {
	HSQUIRRELVM sqvm;
	NSInteger lastError;
	BOOL isFrameInitialized;
	BOOL isRunning;
	BOOL sortOrderDirty;
	NSDate* startTime;
	NSTimeInterval lastOnDrawInterval;
	NSTimeInterval lastOnDrawDrawablesInterval;
	
	UIAccelerometer* accelerometerSensor;
	float accelerometerEventParamCache[ACCELEROMETER_EVENT_PARAMS_SIZE];
	
	BOOL enablePerspectiveNicest;
	BOOL enableOnDrawFrame;
	BOOL accelerometerSensorRegistered;
	BOOL enableOnUpdate;
	
	BOOL enableOnFps;
	NSInteger frameCount;
	NSInteger onFpsInterval;
	NSInteger onFpsIntervalDelta;
	
	NSInteger onDrawFrameInterval;
	NSInteger onDrawDrawablesInterval;
	
	EmoAudioManager* audioManager;
	EmoDatabase* database;
	
	EmoStage* stage;
	
	NSMutableDictionary* drawables;
	NSMutableDictionary* netTasks;
	NSArray* drawablesToDraw;
	NSMutableDictionary* imageCache;
	
	BOOL stopwatchStarted;
	NSTimeInterval stopwatchStartTime;
	NSTimeInterval stopwatchElapsedTime;
	
	NSInteger currentOrientation;
	
	NSInteger logLevel;
}
@property (readonly) HSQUIRRELVM sqvm;
@property (readwrite) int  lastError;
@property (readonly) BOOL isFrameInitialized;
@property (readonly) BOOL isRunning;
@property (readwrite) BOOL enablePerspectiveNicest;
@property (readwrite) BOOL enableOnDrawFrame;
@property (readonly) int onDrawFrameInterval;
@property (readonly) int onDrawDrawablesInterval;
@property (readonly) EmoAudioManager* audioManager;
@property (readonly) EmoStage* stage;
@property (readwrite) BOOL sortOrderDirty;
@property (readonly) EmoDatabase* database;
@property (readwrite) NSInteger currentOrientation;
@property (readwrite) NSInteger logLevel;

- (int)loadScriptFromResource:(const char*)fname vm:(HSQUIRRELVM) v;
- (int)loadScript:(NSString *)path vm:(HSQUIRRELVM) v;
- (void)registerAccelerometerSensor:(BOOL)enable;
- (void)enableSensor:(BOOL)enable withType:(NSInteger)sensorType withInterval:(int)updateInterval;
- (void)disableSensor:(NSInteger)sensorType;
- (BOOL)initDrawFrame;
- (BOOL)onLoad;
- (BOOL)onGainedFocus;
- (BOOL)onDrawFrame;
- (BOOL)onLostFocus;
- (BOOL)onDispose;
- (BOOL)onLowMemory;
- (BOOL)onMotionEvent:(float *)param;
- (BOOL)onKeyEvent:(float *)param;
- (BOOL)startEngine:(GLint)width withHeight:(GLint)height;
- (BOOL)stopEngine;
- (void)updateOnDrawFrameInterval:(NSInteger)interval;
- (void)updateOnDrawDrawablesInterval:(NSInteger)interval;

- (void)addDrawable:(EmoDrawable*)drawable withKey:(const char*)key;
- (EmoDrawable*)getDrawable:(const char*)key;
- (void)clearDrawables;
- (BOOL)removeDrawable:(const char*)key;

-(EmoNetTask*)createNetTask:(NSString*)taskName;
-(void)removeNetTask:(NSString*)taskName;

-(void)stopwatchStart;
-(void)stopwatchStop;
-(NSInteger)stopwatchElapsed;
-(void)enableOnUpdateListener:(BOOL)enable;

-(void)enableOnFpsListener:(BOOL)enable;
-(void)setOnFpsListenerInterval:(NSInteger)value;

-(BOOL)hasCachedImage:(NSString*)key;
-(EmoImage*)getCachedImage:(NSString*)key;
-(void)addCachedImage:(NSString*)key value:(EmoImage*)image;
-(void)removeCachedImage:(NSString*)key;
-(void)clearImageCache;

-(NSString*)getDeviceName;
-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;

-(NSTimeInterval)uptime;
@end

