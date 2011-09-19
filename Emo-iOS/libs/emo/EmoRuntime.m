// Copyright (c) 2011 emo-framework project
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the project nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
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
	content    = nil;
	statusCode = -1;
	statusMessage = [NSString string];
}
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
    statusCode = [((NSHTTPURLResponse *)response) statusCode];
	statusMessage = [NSHTTPURLResponse localizedStringForStatusCode:statusCode];
	content = [[NSMutableData alloc] initWithData:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
	[content appendData:data];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection {
	NSString* response = data2ns(content);
	
	if (statusCode < 400) {
		callSqFunction_Bool_Strings(engine.sqvm, 
			EMO_NAMESPACE, EMO_FUNC_ONCALLBACK, [name UTF8String],
									[response UTF8String], "", "", SQFalse);
	} else {
		callSqFunction_Bool_Strings(engine.sqvm, 
			EMO_NAMESPACE, EMO_FUNC_ONCALLBACK, HTTP_ERROR, [name UTF8String],
					[[NSString stringWithFormat:@"%d", statusCode] UTF8String],
					[statusMessage UTF8String], SQFalse);
	}
	[response release];
	[engine removeNetTask:name];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
    callSqFunction_Bool_Strings(engine.sqvm, EMO_NAMESPACE, EMO_FUNC_ONCALLBACK, 
					HTTP_ERROR, [name UTF8String],
					[[NSString stringWithFormat: @"%d" ,[error code]] UTF8String],
					[[error localizedDescription] UTF8String], SQFalse);
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
	registerClass(engine.sqvm, EMO_RUNTIME_CLASS);
	registerClass(engine.sqvm, EMO_EVENT_CLASS);
    registerClass(engine.sqvm, EMO_NET_CLASS);
    registerClass(engine.sqvm, EMO_STOPWATCH_CLASS);
	
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "import",          emoImportScript);
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "setOptions",      emoSetOptions);
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "echo",            emoRuntimeEcho);
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "log",             emoRuntimeLog);
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "info",            emoRuntimeLogInfo);
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "error",           emoRuntimeLogError);
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "warn",            emoRuntimeLogWarn);
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "finish",          emoRuntimeFinish);
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "os",              emoRuntimeGetOSName);
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "device",          emoRuntimeGetDeviceName);
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "isSimulator",     emoRuntimeIsSimulator);
    registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "gc",              emoRuntimeGC);
    registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "clearTextureCache", emoClearImageCache);
    registerClassFunc(engine.sqvm, EMO_STOPWATCH_CLASS, "start",         emoRuntimeStopwatchStart);
    registerClassFunc(engine.sqvm, EMO_STOPWATCH_CLASS, "stop",          emoRuntimeStopwatchStop);
    registerClassFunc(engine.sqvm, EMO_STOPWATCH_CLASS, "elapsed",       emoRuntimeStopwatchElapsed);
    registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS,   "setLogLevel",   emoRuntimeSetLogLevel);
#ifndef EMO_WITH_SANDBOX
    registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS,   "compilebuffer", emoRuntimeCompileBuffer);
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS,   "compile",       emoRuntimeCompile);
#endif
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS,   "isSandboxEnabled", emoRuntimeIsSandboxEnabled);
	registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS,   "getDocumentDir",emoRuntimeGetDocumentDir);
	registerClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "registerSensors", emoRegisterSensors);
	registerClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "enableSensor",    emoEnableSensor);
	registerClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "disableSensor",   emoDisableSensor);
	registerClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "enableOnDrawCallback",  emoEnableOnDrawCallback);
	registerClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "disableOnDrawCallback", emoDisableOnDrawCallback);
    registerClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "enableOnUpdateCallback",  emoEnableOnUpdateCallback);
    registerClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "disableOnUpdateCallback", emoDisableOnUpdateCallback);
    registerClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "enableOnFpsCallback",  emoEnableOnFpsCallback);
    registerClassFunc(engine.sqvm, EMO_EVENT_CLASS,   "disableOnFpsCallback", emoDisableOnFpsCallback);

    registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "buildNumber",   emoRuntimeBuildNumber);	
    registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "nativeEcho",   emoRuntimeEcho);	
    registerClassFunc(engine.sqvm, EMO_NET_CLASS,     "request",      emoAsyncHttpRequest);

    registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "enableSimpleLog",          emoEnableSimpleLog);
    registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "enableSimpleLogWithLevel", emoEnableSimpleLogWithLevel);
    registerClassFunc(engine.sqvm, EMO_RUNTIME_CLASS, "random",          emoRuntimeRandom);
}

/*
 * returns build number of the library
 */
SQInteger emoRuntimeBuildNumber(HSQUIRRELVM v) {
    sq_pushinteger(v, EMO_BUILD_NUMBER);
    return 1;
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
 * import script from resource
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

#ifndef EMO_WITH_SANDBOX
/*
 * compile script from path
 */
SQInteger emoRuntimeCompile(HSQUIRRELVM v) {
    
    if (sq_gettype(v, 2) == OT_STRING) {
        const SQChar *fname;
        sq_tostring(v, 2);
        sq_getstring(v, -1, &fname);
        sq_poptop(v);

        // check if the file type exists
        if (sq_gettype(v, 3) == OT_INTEGER) {
            SQInteger fileType = TYPE_ASSET;
            sq_getinteger(v, 3, &fileType);
            
            if (fileType == TYPE_ASSET) {
                // load script from resource
                [engine loadScriptFromResource:(const char*)fname vm:v];
            } else if (fileType == TYPE_DOCUMENT) {
                // load script from user document directory
                NSString* docDir = [NSSearchPathForDirectoriesInDomains(
                                NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
                [engine loadScript:[NSString stringWithFormat:@"%@/%s", docDir, fname] vm:v];                
            } else {
                // load script from path
                [engine loadScript:[NSString stringWithFormat:@"%s", fname] vm:v];                
            }
        } else {
            // load script from path
            [engine loadScript:[NSString stringWithFormat:@"%s", fname] vm:v];                
        }
        
    }
	return 0;
}

SQInteger emoRuntimeCompileBuffer(HSQUIRRELVM v) {
    const SQChar* script;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &script);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
    
    sq_pushinteger(v, sqCompileBuffer(v, script, EMO_RUNTIME_CLASS));
    
    return 1;
}
#endif

/*
 * Returns the document directory
 */
SQInteger emoRuntimeGetDocumentDir(HSQUIRRELVM v) {
    NSString* dir = [NSSearchPathForDirectoriesInDomains(
                                                         NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    sq_pushstring(v, [[NSString stringWithFormat:@"%@/", dir] UTF8String], -1);
    
    return 1;
}

/*
 * Check if the sandbox is enabled
 */
SQInteger emoRuntimeIsSandboxEnabled(HSQUIRRELVM v) {
#ifdef EMO_WITH_SANDBOX
    sq_pushbool(v, true);
#else
    sq_pushbool(v, false);
#endif    
    return 1;
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
		case OPT_ORIENTATION_LANDSCAPE_LEFT:
			engine.currentOrientation = OPT_ORIENTATION_LANDSCAPE_LEFT;
			[engine.stage invertSize];
			break;
		case OPT_ORIENTATION_LANDSCAPE_RIGHT:
			engine.currentOrientation = OPT_ORIENTATION_LANDSCAPE_RIGHT;
			[engine.stage invertSize];
			break;
		case OPT_ORIENTATION_LANDSCAPE:
			engine.currentOrientation = OPT_ORIENTATION_LANDSCAPE;
			[engine.stage invertSize];
			break;
		case OPT_ORIENTATION_PORTRAIT:
			engine.currentOrientation = OPT_ORIENTATION_PORTRAIT;
			break;
    }
}

SQInteger emoEnableOnDrawCallback(HSQUIRRELVM v) {
	engine.enableOnDrawFrame = TRUE;
	
	SQInteger nargs = sq_gettop(v);
	
    if (nargs <= 2 && sq_gettype(v, 2) != OT_NULL) {
        SQInteger interval;
        sq_getinteger(v, 2, &interval);
		
        [engine updateOnDrawFrameInterval:interval];
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
 * Set Runtime log level
 */
SQInteger emoRuntimeSetLogLevel(HSQUIRRELVM v) {
	
	if (sq_gettype(v, 2) == OT_INTEGER) {
		SQInteger level;
		sq_getinteger(v, 2, &level);
		
		switch(level) {
			case LOG_INFO:
			case LOG_WARN:
			case LOG_ERROR:
				engine.logLevel = level;
				break;
			default:
				sq_pushinteger(v, ERR_INVALID_PARAM);
				return 1;
		}
		
	} else {
		sq_pushinteger(v, ERR_INVALID_PARAM);
	    return 1;
	}
	
	sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
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
 * Returns device name
 */
SQInteger emoRuntimeGetDeviceName(HSQUIRRELVM v) {
    sq_pushstring(v, [[engine getDeviceName] UTF8String], -1);
    return 1;
}

/*
 * Returns whether the device is simulator or not
 */
SQInteger emoRuntimeIsSimulator(HSQUIRRELVM v) {
	NSString* device = [engine getDeviceName];
	BOOL value = ([device isEqualToString:@"i386"]) || ([device isEqualToString:@"x86_64"]); 
	sq_pushbool(v, value);
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
	
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
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

SQInteger emoEnableOnFpsCallback(HSQUIRRELVM v) {
    [engine enableOnFpsListener:TRUE];
	
    SQInteger nargs = sq_gettop(v);
	
    if (nargs <= 2 && sq_gettype(v, 2) != OT_NULL) {
        SQInteger interval;
        sq_getinteger(v, 2, &interval);
		
        [engine setOnFpsListenerInterval:interval];
    }
	
    return 0;
}

SQInteger emoDisableOnFpsCallback(HSQUIRRELVM v) {
    [engine enableOnFpsListener:FALSE];
    return 0;
}

SQInteger emoClearImageCache(HSQUIRRELVM v) {
    [engine clearImageCache];
    return 0;
}

SQInteger emoRuntimeGC(HSQUIRRELVM v) {
	sq_pushinteger(v, sq_collectgarbage(v));
	return 1;
}

/*
 * Use simple log without any tag and level
 */
SQInteger emoEnableSimpleLog(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	
    if (nargs <= 2 && sq_gettype(v, 2) == OT_BOOL) {
        SQBool enable;
        sq_getbool(v, 2, &enable);
		
        engine.enableSimpleLog = enable;
    } else {
        // called with no parameter
        engine.enableSimpleLog = TRUE;
    }
	
	sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * Use simple log without tag
 */
SQInteger emoEnableSimpleLogWithLevel(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	
    if (nargs <= 2 && sq_gettype(v, 2) == OT_BOOL) {
        SQBool enable;
        sq_getbool(v, 2, &enable);
		
        engine.enableSimpleLogWithLevel = enable;
    } else {
        // called with no parameter
        engine.enableSimpleLogWithLevel = TRUE;
    }
	
	sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * Returns a integer value with a positive sign,
 * greater than or equal to 0.0 and less than max.
 */
SQInteger emoRuntimeRandom(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    
    SQInteger max = RAND_MAX;
    if (nargs <= 2 && sq_gettype(v, 2) == OT_INTEGER) {
        sq_getinteger(v, 2, &max);
    }   
    
    sq_pushinteger(v, arc4random() % max);
    return 1;
}
