#import "EmoRuntime.h"
#import "EmoEngine_glue.h"
#import "VmFunc.h"
#import "Constants.h"
#import "EmoEngine.h"

extern EmoEngine* engine;

@implementation EmoNetTask
@synthesize name, method;
@synthesize timeout;

-(void)asyncHttpRequest:(NSString*)baseUrl withParam:(NSMutableDictionary*)params {
	
	NSMutableString* paramsAsStr = [NSMutableString string];
	
	int count = 0;
	for (NSString* key in params) {
		count++;
		[paramsAsStr appendString:key];
		[paramsAsStr appendString:@"="];
		[paramsAsStr appendString:[params objectForKey:key]];
		if (count < [params count]) {
			[paramsAsStr appendString:@"&"];
		}
	}

	NSMutableString* url = [NSMutableString stringWithString:baseUrl];
	if ([method isEqualToString:@"GET"] && [params count] > 0) {
		[url appendString:@"?"];
		[url appendString:paramsAsStr];
	}
	
	NSMutableURLRequest* request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
	if ([method isEqualToString:@"POST"]) {
		NSData* requestData = [paramsAsStr dataUsingEncoding:NSUTF8StringEncoding];
		[request setHTTPMethod: @"POST"];
		[request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"content-type"];
		[request setHTTPBody: requestData];
	}
   	connection = [[NSURLConnection alloc] initWithRequest:request delegate:self];
}
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
	content = [[NSMutableData alloc] initWithData:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
	[content appendData:data];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection {
	NSString* response = data2ns(content);
    callSqFunction_Bool_TwoStrings(engine.sqvm, 
			EMO_NAMESPACE, EMO_FUNC_ONCALLBACK, 
				[name UTF8String], [response UTF8String], SQFalse);
	[engine removeNetTask:name];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
	engine.lastCallbackErrorMessage = [error localizedDescription];
	engine.lastCallbackErrorType = @"NSURLConnection";
    callSqFunction_Bool_TwoStrings(engine.sqvm, 
								   EMO_NAMESPACE, EMO_FUNC_ONCALLBACK, 
								   "ERROR", [name UTF8String], SQFalse);
	[engine removeNetTask:name];
}
-(void)dealloc {
	[connection release];
	[content release];
	[name release];
	[method release];
	
	[super dealloc];
}
@end

void initRuntimeFunctions() {
	registerEmoClass(engine.sqvm, EMO_RUNTIME_CLASS);
	registerEmoClass(engine.sqvm, EMO_EVENT_CLASS);
    registerEmoClass(engine.sqvm, EMO_NET_CLASS);
    registerEmoClass(engine.sqvm, EMO_STOPWATCH_CLASS);
	
	registerEmoClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "import",          emoImportScript);
	registerEmoClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "setOptions",      emoSetOptions);
	registerEmoClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "echo",            emoRuntimeEcho);
	registerEmoClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "log",             emoRuntimeLog);
	registerEmoClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "info",            emoRuntimeLogInfo);
	registerEmoClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "error",           emoRuntimeLogError);
	registerEmoClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "warn",            emoRuntimeLogWarn);
	registerEmoClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "finish",          emoRuntimeFinish);
	registerEmoClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "os",              emoRuntimeGetOSName);
    registerEmoClassFunc(engine.sqvm, EMO_STOPWATCH_CLASS, "start",         emoRuntimeStopwatchStart);
    registerEmoClassFunc(engine.sqvm, EMO_STOPWATCH_CLASS, "stop",          emoRuntimeStopwatchStop);
    registerEmoClassFunc(engine.sqvm, EMO_STOPWATCH_CLASS, "elapsed",       emoRuntimeStopwatchElapsed);
	
	registerEmoClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "registerSensors", emoRegisterSensors);
	registerEmoClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "enableSensor",    emoEnableSensor);
	registerEmoClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "disableSensor",   emoDisableSensor);
	registerEmoClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "enableOnDrawCallback",  emoEnableOnDrawCallback);
	registerEmoClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "disableOnDrawCallback", emoDisableOnDrawCallback);
    registerEmoClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "enableOnUpdateCallback",  emoEnableOnUpdateCallback);
    registerEmoClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "disableOnUpdateCallback", emoDisableOnUpdateCallback);
	
	registerEmoClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "getLastErrorMessage",   emoGetLastCallbackErrorMessage);
	registerEmoClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "getLastErrorType",      emoGetLastCallbackErrorType);

    registerEmoClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "nativeEcho",   emoRuntimeEcho);	
    registerEmoClassFunc(engine.sqvm, EMO_NET_CLASS,     "request",      emoAsyncHttpRequest);
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

/*
 * Echo function for api testing
 */
SQInteger emoRuntimeEcho(HSQUIRRELVM v) {
	const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
    	if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);
            sq_poptop(v);
    	}
    }
	
	if (strlen(str) > 0) {
		sq_pushstring(v, str, -1);
	}
	
	return 1;
}

/*
 * import function called from squirrel script
 */
SQInteger emoImportScript(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
    	if (sq_gettype(v, n) == OT_STRING) {
    		const SQChar *fname;
            sq_tostring(v, n);
            sq_getstring(v, -1, &fname);
            sq_poptop(v);
			
    		[engine loadScriptFromResource:(const char*)fname vm:v];
    	}
    }
	return 0;
}

/*
 * update options
 */
void emoUpdateOptions(SQInteger value) {
    switch(value) {
		case OPT_ENABLE_PERSPECTIVE_NICEST:
			engine.enablePerspectiveNicest = TRUE;
			break;
		case OPT_ENABLE_PERSPECTIVE_FASTEST:
			engine.enablePerspectiveNicest = FALSE;
			break;
		case OPT_WINDOW_KEEP_SCREEN_ON:
			[UIApplication sharedApplication].idleTimerDisabled = YES;
			break;
    }
}

SQInteger emoEnableOnDrawCallback(HSQUIRRELVM v) {
	engine.enableOnDrawFrame = TRUE;
	
	SQInteger nargs = sq_gettop(v);
	
    if (nargs <= 2 && sq_gettype(v, 2) == OT_INTEGER) {
        SQInteger interval;
        sq_getinteger(v, 2, &interval);
		
        engine.onDrawFrameInterval = interval;
    }
	
	return 0;
}

SQInteger emoDisableOnDrawCallback(HSQUIRRELVM v) {
	engine.enableOnDrawFrame = FALSE;
	return 0;
}

/*
 * delcare to use sensors
 */
void emoCreateSensors(SQInteger value) {
	switch(value) {
		case SENSOR_TYPE_ACCELEROMETER:
			[engine registerAccelerometerSensor:TRUE];
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

/*
 * register sensors function called from script
 */
SQInteger emoRegisterSensors(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_INTEGER) {
            SQInteger value;
            sq_getinteger(v, n, &value);
			
            emoCreateSensors(value);
        }
    }
    return 0;
}

/*
 * enable sensor
 */
SQInteger emoEnableSensor(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    if (nargs < 3) {
        LOGE("Invalid call: emoEnableSensors(sensorType, interval)");
        return 0;
    }
	
    SQInteger sensorType;
    SQInteger interval;
	
    sq_getinteger(v, 2, &sensorType);
    sq_getinteger(v, 3, &interval);
	
	[engine enableSensor:TRUE withType:sensorType withInterval:interval];
	
	return 0;
}

/*
 * disable sensor
 */
SQInteger emoDisableSensor(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    if (nargs < 2) {
        LOGE("Invalid call: emoDisableSensors(sensorType)");
        return 0;
    }
	
    SQInteger sensorType;
    sq_getinteger(v, 2, &sensorType);
	
	[engine disableSensor:sensorType];
	
	return 0;
}

/*
 * Runtime logging
 */
SQInteger emoRuntimeLog(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    SQInteger level;
    const SQChar *message;
	
    if (nargs < 3) return 0;
	
    if (sq_gettype(v, 2) == OT_INTEGER && sq_gettype(v, 3) == OT_STRING) {
        sq_getinteger(v, 2, &level);
        sq_tostring(v, 3);
        sq_getstring(v, -1, &message);
        sq_poptop(v);
		
        switch(level) {
			case LOG_INFO:
				LOGI((char*)message);
				break;
			case LOG_ERROR:
				LOGE((char*)message);
				break;
			case LOG_WARN:
				LOGW((char*)message);
				break;
        }
    }
	return 0;
}

/*
 * Runtime log info
 */
SQInteger emoRuntimeLogInfo(HSQUIRRELVM v) {
    const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);
			
            LOGI((char*)str);
        }
    }
    return 0;
}

/*
 * Runtime log error
 */
SQInteger emoRuntimeLogError(HSQUIRRELVM v) {
    const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);
			
            LOGE((char*)str);
        }
    }
    return 0;
}

/*
 * Runtime log warn
 */
SQInteger emoRuntimeLogWarn(HSQUIRRELVM v) {
    const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
        if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);
			
            LOGW((char*)str);
        }
    }
    return 0;
}

/*
 * shutdown the application
 * (this won't work for iOS because the HOME button should be
 * the only way to exit your app.)
 */
SQInteger emoRuntimeFinish(HSQUIRRELVM v) {
	return 0;
}

/*
 * Returns OS name
 */
SQInteger emoRuntimeGetOSName(HSQUIRRELVM v) {
    sq_pushstring(v, OS_IOS, -1);
    return 1;
}

/*
 * Returns last callback error message
 */
SQInteger emoGetLastCallbackErrorMessage(HSQUIRRELVM v) {
    sq_pushstring(v, [engine.lastCallbackErrorMessage UTF8String], -1);
    return 1;
}

/*
 * Returns last callback error type (class name or exception name)
 */
SQInteger emoGetLastCallbackErrorType(HSQUIRRELVM v) {
    sq_pushstring(v, [engine.lastCallbackErrorType UTF8String], -1);
    return 1;
}

/*
 * Asynchronous http request
 */
SQInteger emoAsyncHttpRequest(HSQUIRRELVM v) {
    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
		
    const SQChar* url;
    const SQChar* method;
    SQInteger timeout = 5000;
	
    if (nargs >= 3 && sq_gettype(v, 3) == OT_STRING) {
        sq_tostring(v, 3);
        sq_getstring(v, -1, &url);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (nargs >= 4 && sq_gettype(v, 4) == OT_STRING) {
        sq_tostring(v, 4);
        sq_getstring(v, -1, &method);
        sq_poptop(v);
    } else {
        method = "GET";
    }
	
    if (nargs >= 5 && sq_gettype(v, 5) == OT_INTEGER) {
        sq_getinteger(v, 5, &timeout);
    }
	
	NSMutableDictionary* params = [[NSMutableDictionary alloc]init];
	NSString* param_key;
    for(SQInteger n = 6; n <= nargs; n++) {
		
        const SQChar* param;
        sq_tostring(v, n);
        sq_getstring(v, -1, &param);
        sq_poptop(v);
		
		NSString* paramStr = char2ns(param);
		
        if (n % 2 == 0) {
			param_key = paramStr;
        } else if ([param_key length] > 0) {
            [params setObject:paramStr forKey:param_key];
        }
    }
	
	if (url != NULL) {
		EmoNetTask* net = [engine createNetTask:char2ns(name)];
		net.method = char2ns(method);
		net.timeout = timeout;
		
        [net asyncHttpRequest: char2ns(url) withParam:params];
	}

	[params release];
	
    sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}

/*
 * start stopwatch
 */
SQInteger emoRuntimeStopwatchStart(HSQUIRRELVM v) {
    [engine stopwatchStart];
    return 0;
}

/*
 * get elapsed time from stop watch
 */
SQInteger emoRuntimeStopwatchElapsed(HSQUIRRELVM v) {
    sq_pushinteger(v, [engine stopwatchElapsed]);
    return 1;
}

/*
 * stop stopwatch
 */
SQInteger emoRuntimeStopwatchStop(HSQUIRRELVM v) {
    [engine stopwatchStop];
    return 0;
}

SQInteger emoEnableOnUpdateCallback(HSQUIRRELVM v) {
    [engine enableOnUpdateListener:true];
    return 0;
}
SQInteger emoDisableOnUpdateCallback(HSQUIRRELVM v) {
    [engine enableOnUpdateListener:false];
    return 0;
}

