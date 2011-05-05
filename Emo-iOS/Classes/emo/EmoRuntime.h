#import "squirrel.h"

@interface EmoNetTask : NSObject
{
	NSURLConnection* connection;
	NSMutableData* content;

	NSString* name;
	NSInteger timeout;
	NSString* method;
}
@property (readwrite, copy) NSString* name;
@property (readwrite, copy) NSString* method;
@property (readwrite) NSInteger timeout;

-(void)asyncHttpRequest:(NSString*)url withParam:(NSMutableDictionary*)params;
@end

void initRuntimeFunctions();

void registerEmoClass(HSQUIRRELVM v, const char *cname);
void registerEmoClassFunc(HSQUIRRELVM v, const char *cname, const char *fname, SQFUNCTION func);
SQInteger emoRuntimeGetOSName(HSQUIRRELVM v);
SQInteger emoRuntimeEcho(HSQUIRRELVM v);
SQInteger emoImportScript(HSQUIRRELVM v);
SQInteger emoEnableOnDrawCallback(HSQUIRRELVM v);
SQInteger emoDisableOnDrawCallback(HSQUIRRELVM v);
void emoUpdateOptions(SQInteger value);
void emoCreateSensors(SQInteger value);
SQInteger emoSetOptions(HSQUIRRELVM v);
SQInteger emoRegisterSensors(HSQUIRRELVM v);
SQInteger emoEnableSensor(HSQUIRRELVM v);
SQInteger emoDisableSensor(HSQUIRRELVM v);
SQInteger emoRuntimeLog(HSQUIRRELVM v);
SQInteger emoRuntimeLogInfo(HSQUIRRELVM v);
SQInteger emoRuntimeLogError(HSQUIRRELVM v);
SQInteger emoRuntimeLogWarn(HSQUIRRELVM v);
SQInteger emoRuntimeFinish(HSQUIRRELVM v);

SQInteger emoAsyncHttpRequest(HSQUIRRELVM v);
SQInteger emoGetLastCallbackErrorMessage(HSQUIRRELVM v);
SQInteger emoGetLastCallbackErrorType(HSQUIRRELVM v);

SQInteger emoRuntimeStopwatchStart(HSQUIRRELVM v);
SQInteger emoRuntimeStopwatchElapsed(HSQUIRRELVM v);
SQInteger emoRuntimeStopwatchStop(HSQUIRRELVM v);
SQInteger emoEnableOnUpdateCallback(HSQUIRRELVM v);
SQInteger emoDisableOnUpdateCallback(HSQUIRRELVM v);
