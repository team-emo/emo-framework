#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#import "common.h"
#import "EmoEngine.h"
#import "sqfunc.h"

static BOOL enablePerspectiveNicest = TRUE;
static BOOL enableOnDrawFrame = FALSE;

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

static void emoUpdateOptions(SQInteger value) {
    switch(value) {
		case OPT_ENABLE_PERSPECTIVE_NICEST:
			enablePerspectiveNicest = TRUE;
			break;
		case OPT_ENABLE_PERSPECTIVE_FASTEST:
			enablePerspectiveNicest = FALSE;
			break;
		case OPT_ENABLE_ONDRAW_CALLBACK:
			enableOnDrawFrame = TRUE;
			break;
    }
}

/*
 * option function called from squirrel script
 */
SQInteger emoSetOptions(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_INTEGER) {
            SQInteger value;
            sq_getinteger(v, n, &value);
			
            emoUpdateOptions(value);
        }
    }
	return 0;
}

@interface EmoEngine (PrivateMethods)
- (void)initEngine;
@end

@implementation EmoEngine
@synthesize lastError;
@synthesize isFrameInitialized;
@synthesize sqvm;

- (void)initEngine {
	register_global_func(sqvm, emoImportScript, "emo_import");	
	register_global_func(sqvm, emoSetOptions,   "emo_options");	
}
- (BOOL)startEngine {
	
	isFrameInitialized = FALSE;
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

/*
 * initialize OpenGL state 
 */
- (BOOL)initDrawFrame {

	if (isFrameInitialized) return FALSE;
	
    if (enablePerspectiveNicest) {
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    } else {
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    }
	
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_DITHER);
    glDisable(GL_COLOR_ARRAY);
	
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
    glEnable(GL_VERTEX_ARRAY);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
	
	isFrameInitialized = TRUE;
	
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
-(BOOL)onDrawFrame {
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
	
	if (enableOnDrawFrame) {
		return callSqFunctionNoParam(sqvm, "onDrawFrame");
	}
	return FALSE;
}
-(BOOL)onLostFocus {
	return callSqFunctionNoParam(sqvm, "onLostFocus");
}
-(BOOL)onDispose {
	return callSqFunctionNoParam(sqvm, "onDispose");
}
@end