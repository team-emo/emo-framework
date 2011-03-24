#import "squirrel.h"

void LOGI(const char* msg);
void LOGE(const char* msg);
void LOGW(const char* msg);

void NSLOGI(NSString* msg);
void NSLOGE(NSString* msg);
void NSLOGW(NSString* msg);

@interface EmoEngine : NSObject<UIAccelerometerDelegate> {
	HSQUIRRELVM sqvm;
	int lastError;
	BOOL isFrameInitialized;
	BOOL isRunning;
	NSDate* startTime;
	UIAccelerometer* accelerometerSensor;
}
@property (readonly) HSQUIRRELVM sqvm;
@property (readonly) int  lastError;
@property (readonly) BOOL isFrameInitialized;
@property (readonly) BOOL isRunning;

+ (int)loadScriptFromResource:(const char*)fname vm:(HSQUIRRELVM) v;
+ (void)registerAccelerometerSensor:(BOOL)enable;
+ (void)enableSensor:(BOOL)enable withType:(NSInteger)sensorType withInterval:(NSTimeInterval)updateInterval;
+ (void)disableSensor:(NSInteger)sensorType;
+ (void)updateStatusImmediately;
- (BOOL)initDrawFrame;
- (BOOL)onLoad;
- (BOOL)onGainedFocus;
- (BOOL)onDrawFrame;
- (BOOL)onLostFocus;
- (BOOL)onDispose;
- (BOOL)onLowMemory;
- (BOOL)onMotionEvent:(float *)param;
- (BOOL)onKeyEvent:(float *)param;
- (BOOL)startEngine;
- (BOOL)stopEngine;
- (NSTimeInterval)uptime;
@end

