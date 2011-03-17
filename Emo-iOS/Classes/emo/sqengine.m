#import "common.h"
#import "sqengine.h"
#import "sqglue.h"

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

@implementation EmoEngine
@synthesize lastError;

- (id)init {
    self = [super init];
    if (self != nil) {
		
		// initialize squirrel vm
		initSQGlue();
		
    }
    return self;
}

- (BOOL)loadScriptFromResource:(NSString *)fname {
	NSString* path = [[NSBundle mainBundle] pathForResource:fname ofType:nil];
	NSString* nscontent = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error: nil];
	
	if (nscontent == nil) {
		lastError = ERR_SCRIPT_OPEN;
		return FALSE;
	}
	
	const char* script = [nscontent UTF8String];

	if (sqGlue_compile(script)) {
		lastError = EMO_NO_ERROR;
	} else {
		lastError = ERR_SCRIPT_COMPILE;
	}
	
	return TRUE;
}
-(void) dealloc{
	[super dealloc];
}
@end