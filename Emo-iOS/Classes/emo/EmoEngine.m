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

/*
 * Import function called from squirrel script
 * Returns number of parameters that are succeed to import.
 */
SQInteger emoImportScript(HSQUIRRELVM v) {
	SQInteger succeedCount = 0;
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
    	if (sq_gettype(v, n) == OT_STRING) {
    		const SQChar *fname;
            sq_tostring(v, n);
            sq_getstring(v, -1, &fname);
            sq_poptop(v);
			
    		if ([EmoEngine loadScriptFromResource:(const char*)fname vm:v] == EMO_NO_ERROR) {
				succeedCount++;
			}
    	}
    }
	return succeedCount;
}

@interface EmoEngine (PrivateMethods)
- (void)initEngine;
@end

@implementation EmoEngine
@synthesize lastError;
@synthesize enableSQOnDrawFrame;
@synthesize sqvm;

- (void)initEngine {
	register_global_func(sqvm, emoImportScript, "emoImport");	
}
- (BOOL)startEngine {
	
	enableSQOnDrawFrame = false;
	lastError = EMO_NO_ERROR;
	
	sqvm = sq_open(SQUIRREL_VM_INITIAL_STACK_SIZE);
	
	initSQVM(sqvm);
	[self initEngine];

	return TRUE;
}

- (BOOL)stopEngine {
	sq_close(sqvm);
	return TRUE;
}

+(int)loadScriptFromResource:(const char*)chfname vm:(HSQUIRRELVM) v {
	NSString* fname = [[NSString alloc] initWithUTF8String:chfname];
	NSString* path = [[NSBundle mainBundle] pathForResource:fname ofType:nil];
	NSString* nscontent = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error: nil];
	
	[fname release];
	
	if (nscontent == nil) {
		return ERR_SCRIPT_OPEN;
	}
	
	const char* script = [nscontent UTF8String];
	const char* sourcename  = [path UTF8String];
	
	return sqCompileBuffer(v, script, sourcename);
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
@end