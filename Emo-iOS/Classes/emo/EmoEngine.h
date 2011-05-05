#import "squirrel.h"
#import "Constants.h"
#import "EmoAudio.h"
#import "EmoDrawable.h"
#import "EmoStage.h"
#import "EmoRuntime.h"

NSString* char2ns(const SQChar* str);
NSString* data2ns(NSData* data);

@interface EmoEngine : NSObject<UIAccelerometerDelegate> {
	HSQUIRRELVM sqvm;
	NSInteger lastError;
	NSString* lastCallbackErrorMessage;
	NSString* lastCallbackErrorType;
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
	
	NSInteger onDrawFrameInterval;
	NSInteger onDrawDrawablesInterval;
	
	EmoAudioManager* audioManager;
	
	EmoStage* stage;
	
	NSMutableDictionary* drawables;
	NSMutableDictionary* netTasks;
	NSArray* drawablesToDraw;
	
	BOOL stopwatchStarted;
	NSTimeInterval stopwatchStartTime;
	NSTimeInterval stopwatchElapsedTime;
}
@property (readonly) HSQUIRRELVM sqvm;
@property (readwrite) int  lastError;
@property (readwrite, copy) NSString* lastCallbackErrorMessage;
@property (readwrite, copy) NSString* lastCallbackErrorType;
@property (readonly) BOOL isFrameInitialized;
@property (readonly) BOOL isRunning;
@property (readwrite) BOOL enablePerspectiveNicest;
@property (readwrite) BOOL enableOnDrawFrame;
@property (readwrite) int onDrawFrameInterval;
@property (readwrite) int onDrawDrawablesInterval;
@property (readonly) EmoAudioManager* audioManager;
@property (readonly) EmoStage* stage;
@property (readwrite) BOOL sortOrderDirty;

- (int)loadScriptFromResource:(const char*)fname vm:(HSQUIRRELVM) v;
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

- (NSTimeInterval)uptime;
@end

