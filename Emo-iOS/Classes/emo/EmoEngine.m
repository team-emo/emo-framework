#import "common.h"
#import "EmoEngine.h"
#import "sqfunc.h"

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
@synthesize sqvm;

- (BOOL)startEngine {
	
	enableSQOnDrawFrame = false;
	lastError = EMO_NO_ERROR;
	
	sqvm = sq_open(SQUIRREL_VM_INITIAL_STACK_SIZE);
	
	initSQVM(sqvm);

	return TRUE;
}

- (BOOL)stopEngine {
	sq_close(sqvm);
	return TRUE;
}

-(BOOL)loadScriptFromResource:(NSString *)fname {
	NSString* path = [[NSBundle mainBundle] pathForResource:fname ofType:nil];
	NSString* nscontent = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error: nil];
	
	if (nscontent == nil) {
		lastError = ERR_SCRIPT_OPEN;
		return FALSE;
	}
	
	const char* script = [nscontent UTF8String];
	const char* sourcename  = [path UTF8String];
	
	lastError = sqCompileBuffer(sqvm, script, sourcename);
	
	return lastError == EMO_NO_ERROR;
}

-(BOOL)onLoad {
	return callSqFunctionNoParam(sqvm, "onLoad");
}
-(BOOL)onGainedFocus {
	return callSqFunctionNoParam(sqvm, "onGainedFocus");
}
-(BOOL)onLostFocus {
	return callSqFunctionNoParam(sqvm, "onLostFocus");
}
-(BOOL)onDispose {
	return callSqFunctionNoParam(sqvm, "onDispose");
}

-(void) dealloc{
	[super dealloc];
}
@end