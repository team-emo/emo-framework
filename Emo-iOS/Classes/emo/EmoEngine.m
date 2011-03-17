#import "common.h"
#import "EmoEngine.h"
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
@synthesize enableSQOnDrawFrame;

- (BOOL)startEngine {
	
	enableSQOnDrawFrame = false;
	lastError = EMO_NO_ERROR;
	
	// initialize squirrel vm
	if (!initSQGlue()) {
		lastError = ERR_VM_OPEN;
		return FALSE;
	}

	return TRUE;
}

- (BOOL)stopEngine {
	if (!stopSQGlue()) {
		lastError = ERR_VM_CLOSE;
		return FALSE;
	}
	return TRUE;
}

- (BOOL)loadScriptFromResource:(NSString *)fname {
	NSString* path = [[NSBundle mainBundle] pathForResource:fname ofType:nil];
	NSString* nscontent = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error: nil];
	
	if (nscontent == nil) {
		lastError = ERR_SCRIPT_OPEN;
		return FALSE;
	}
	
	const char* script = [nscontent UTF8String];
	const char* sourcename  = [path UTF8String];

	int compileResult = sqGlue_compile(script, sourcename);
	if (compileResult != EMO_NO_ERROR) {
		lastError = compileResult;
		return FALSE;
	}
	
	return TRUE;
}
-(void) dealloc{
	[super dealloc];
}
@end