#import "EmoFunc.h"
#import "sqfunc.h"
#import "common.h"

/*
 * Logging
 */
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
 * Register new class for emo framework
 */
void registerEmoClass(HSQUIRRELVM v, const char *cname) {
    register_class_with_namespace(v, EMO_NAMESPACE, cname);
}

/*
 * Register class method for emo framework
 */
void registerEmoClassFunc(HSQUIRRELVM v, const char *cname, const char *fname, SQFUNCTION func) {
	register_class_func_with_namespace(
			   v, EMO_NAMESPACE, cname, fname, func);
}