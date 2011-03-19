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
	BOOL enableSQOnDrawFrame;
}
@property int lastError;
@property BOOL enableSQOnDrawFrame;
@property HSQUIRRELVM sqvm;

- (BOOL)loadScriptFromResource:(NSString *)fname;
- (BOOL)callFunctionNoParam:(const SQChar*) name;
- (BOOL)startEngine;
- (BOOL)stopEngine;
@end

