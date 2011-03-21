#import "squirrel.h"

void LOGI(const char* msg);
void LOGE(const char* msg);
void LOGW(const char* msg);

void NSLOGI(NSString* msg);
void NSLOGE(NSString* msg);
void NSLOGW(NSString* msg);

@interface EmoEngine : NSObject {
	HSQUIRRELVM sqvm;
	int lastError;
	BOOL isFrameInitialized;
}
@property (readonly) int  lastError;
@property (readonly) BOOL isFrameInitialized;
@property (readonly) HSQUIRRELVM sqvm;

+ (int)loadScriptFromResource:(const char*)fname vm:(HSQUIRRELVM) v;
- (BOOL)initDrawFrame;
- (BOOL)onLoad;
- (BOOL)onGainedFocus;
- (BOOL)onDrawFrame;
- (BOOL)onLostFocus;
- (BOOL)onDispose;
- (BOOL)onLowMemory;
- (BOOL)startEngine;
- (BOOL)stopEngine;
@end

