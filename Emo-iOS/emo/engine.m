#import "common.h"
#import "engine.h"

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

@implementation Engine
@synthesize lastError;

- (BOOL)loadScriptFromResource:(NSString *)fname {
	NSString* path = [[NSBundle mainBundle] pathForResource:fname ofType:nil];
	NSString* nscontent = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error: nil];
	
	if (nscontent == nil) {
		lastError = ERR_SCRIPT_OPEN;
		return FALSE;
	}
	
	const char* contents = [nscontent UTF8String];
	
	LOGI(contents);
	
	return TRUE;
}
-(void) dealloc{
	[super dealloc];
}
@end