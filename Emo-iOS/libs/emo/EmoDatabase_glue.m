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
#import "EmoDatabase_glue.h"
#import "Constants.h"
#import "EmoEngine.h"
#import "VmFunc.h"

extern EmoEngine* engine;

void initDatabaseFunctions() {
    registerClass(engine.sqvm, EMO_DATABASE_CLASS);
    registerClass(engine.sqvm, EMO_PREFERENCE_CLASS);
	
    registerClassFunc(engine.sqvm, EMO_DATABASE_CLASS, "openOrCreate", emoDatabaseOpenOrCreate);
    registerClassFunc(engine.sqvm, EMO_DATABASE_CLASS, "open",         emoDatabaseOpen);
    registerClassFunc(engine.sqvm, EMO_DATABASE_CLASS, "close",        emoDatabaseClose);
    registerClassFunc(engine.sqvm, EMO_DATABASE_CLASS, "getPath",      emoDatabaseGetPath);
    registerClassFunc(engine.sqvm, EMO_DATABASE_CLASS, "getLastError", emoDatabaseGetLastError);
    registerClassFunc(engine.sqvm, EMO_DATABASE_CLASS, "getLastErrorMessage", emoDatabaseGetLastErrorMessage);
    registerClassFunc(engine.sqvm, EMO_DATABASE_CLASS, "deleteDatabase", emoDatabaseDeleteDatabase);
	
    registerClassFunc(engine.sqvm, EMO_PREFERENCE_CLASS, "open",         emoDatabaseOpenPreference);
    registerClassFunc(engine.sqvm, EMO_PREFERENCE_CLASS, "openOrCreate", emoDatabaseOpenOrCreatePreference);
    registerClassFunc(engine.sqvm, EMO_PREFERENCE_CLASS, "get",          emoDatabaseGetPreference);
    registerClassFunc(engine.sqvm, EMO_PREFERENCE_CLASS, "set",          emoDatabaseSetPreference);
    registerClassFunc(engine.sqvm, EMO_PREFERENCE_CLASS, "del",          emoDatabaseDeletePreference);
    registerClassFunc(engine.sqvm, EMO_PREFERENCE_CLASS, "keys",         emoDatabaseGetPreferenceKeys);
    registerClassFunc(engine.sqvm, EMO_PREFERENCE_CLASS, "close",        emoDatabaseClose);
}

SQInteger emoDatabaseOpenOrCreate(HSQUIRRELVM v) {
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
	
    SQInteger mode = FILE_MODE_PRIVATE;
    if (nargs >= 3 && sq_gettype(v, 3) == OT_INTEGER) {
        sq_getinteger(v, 3, &mode);
    }
	
	NSString* nsname = [[NSString alloc]initWithUTF8String:name];
    if (![engine.database openOrCreate:nsname mode:mode]) {
        sq_pushinteger(v, ERR_DATABASE_OPEN);
		[nsname release];
        return 1;
    }
	[nsname release];
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDatabaseOpen(HSQUIRRELVM v) {
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
	
	NSString* nsname = [[NSString alloc]initWithUTF8String:name];
    if (![engine.database open:nsname]) {
        sq_pushinteger(v, ERR_DATABASE_OPEN);
		[nsname release];
        return 1;
    }
	[nsname release];
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDatabaseClose(HSQUIRRELVM v) {
    if (![engine.database close]) {
        sq_pushinteger(v, ERR_DATABASE_CLOSE);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDatabaseGetPath(HSQUIRRELVM v) {
    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);
        sq_poptop(v);
    } else {
        return 0;
    }
	NSString* nsname = [[NSString alloc]initWithUTF8String:name];
    const char* path = [[engine.database getPath:nsname] UTF8String];
	[nsname release];
	
    sq_pushstring(v, path, strlen(path));
	
    return 1;
}

SQInteger emoDatabaseGetLastError(HSQUIRRELVM v) {
    sq_pushinteger(v, [engine.database getLastError]);
    return 1;
}

SQInteger emoDatabaseGetLastErrorMessage(HSQUIRRELVM v) {
    const char* str = [[engine.database getLastErrorMessage] UTF8String];
    sq_pushstring(v, str, strlen(str));
    return 1;
}

SQInteger emoDatabaseOpenPreference(HSQUIRRELVM v) {
    if (![engine.database openPreference]) {
        sq_pushinteger(v, ERR_DATABASE_OPEN);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDatabaseOpenOrCreatePreference(HSQUIRRELVM v) {
    if (![engine.database openOrCreatePreference]) {
        sq_pushinteger(v, ERR_DATABASE_OPEN);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDatabaseGetPreference(HSQUIRRELVM v) {
    const SQChar* key;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &key);
        sq_poptop(v);
    } else {
        return 0;
    }
	
	NSString* nskey = [[NSString alloc]initWithUTF8String:key];
    const char* str = [[engine.database getPreference:nskey] UTF8String];
	[nskey release];
	
    sq_pushstring(v, str, strlen(str));
    return 1;
}

SQInteger emoDatabaseSetPreference(HSQUIRRELVM v) {
    const SQChar* key;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) != OT_NULL) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &key);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    const SQChar* value;
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        sq_tostring(v, 3);
        sq_getstring(v, -1, &value);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
	NSString* nskey   = [[NSString alloc]initWithUTF8String:key];
	NSString* nsvalue = [[NSString alloc]initWithUTF8String:value];
    if (![engine.database setPreference:nskey value:nsvalue]) {
        sq_pushinteger(v, ERR_DATABASE);
		[nskey release];
		[nsvalue release];
        return 1;
    }
	[nskey release];
	[nsvalue release];

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDatabaseDeleteDatabase(HSQUIRRELVM v) {
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
	NSString* nsname = [[NSString alloc]initWithUTF8String:name];
    if (![engine.database deleteDatabase:nsname]) {
        sq_pushinteger(v, ERR_DATABASE);
		[nsname release];
        return 1;
    }
	[nsname release];
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
	
}

SQInteger emoDatabaseDeletePreference(HSQUIRRELVM v) {
    const SQChar* key;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &key);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	NSString* nskey   = [[NSString alloc]initWithUTF8String:key];
    if (![engine.database deletePreference:nskey]) {
        sq_pushinteger(v, ERR_DATABASE);
		[nskey release];
        return 1;
    }
	[nskey release];
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDatabaseGetPreferenceKeys(HSQUIRRELVM v) {
	
    NSArray* keys = [engine.database getPreferenceKeys];
	
    sq_newarray(v, 0);
	
    for (int i = 0; i < [keys count]; i++) {
		const char* key = [[keys objectAtIndex:i] UTF8String];
        sq_pushstring(v, key, -1);
        sq_arrayappend(v, -2);
    }
	
    sq_push(v, -1);
	
    return 1;
}

