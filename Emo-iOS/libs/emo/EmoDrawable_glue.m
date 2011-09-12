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
#import "EmoDrawable_glue.h"
#import "EmoEngine_glue.h"
#import "Constants.h"
#import "EmoEngine.h"
#import "EmoDrawable.h"
#import "EmoMapDrawable.h"
#import "VmFunc.h"
#import "Util.h"

extern EmoEngine* engine;

void initDrawableFunctions() {
	registerClass(engine.sqvm, EMO_STAGE_CLASS);
	
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "createSprite",   emoDrawableCreateSprite);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "createLine",     emoDrawableCreateLine);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "createSpriteSheet", emoDrawableCreateSpriteSheet);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "loadSprite",     emoDrawableLoad);

	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "createSnapshot",   emoDrawableCreateSnapshot);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "createMapSprite",     emoDrawableCreateMapSprite);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "loadMapSprite",    emoDrawableLoadMapSprite);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "addTileRow",       emoDrawableAddTileRow);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "clearTiles",       emoDrawableClearTiles);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "setTileAt",        emoDrawableSetTileAt);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getTileAt",        emoDrawableGetTileAt);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getTileIndexAtCoord",    emoDrawableGetTileIndexAtCoord);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getTilePositionAtCoord", emoDrawableGetTilePositionAtCoord);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "useMeshMapSprite", emoDrawableUseMeshMapSprite);

	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getX",           emoDrawableGetX);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getY",           emoDrawableGetY);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getZ",           emoDrawableGetZ);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getWidth",       emoDrawableGetWidth);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getHeight",      emoDrawableGetHeight);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getScaleX",      emoDrawableGetScaleX);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getScaleY",      emoDrawableGetScaleY);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getAngle",       emoDrawableGetAngle);
	
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "setX",           emoDrawableSetX);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "setY",           emoDrawableSetY);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "setZ",           emoDrawableSetZ);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "setWidth",       emoDrawableSetWidth);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "setHeight",      emoDrawableSetHeight);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "setSize",        emoDrawableSetSize);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "bgcolor",        emoSetStageColor);
	
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "move",           emoDrawableMove);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "scale",          emoDrawableScale);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "rotate",         emoDrawableRotate);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "color",          emoDrawableColor);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "remove",         emoDrawableRemove);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "interval",       emoSetOnDrawInterval);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "viewport",       emoSetViewport);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "ortho",          emoSetStageSize);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "windowWidth",    emoGetWindowWidth);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "windowHeight",   emoGetWindowHeight);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "show",           emoDrawableShow);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "hide",           emoDrawableHide);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "red",            emoDrawableColorRed);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "green",          emoDrawableColorGreen);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "blue",           emoDrawableColorBlue);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "alpha",          emoDrawableColorAlpha);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "pauseAt",        emoDrawablePauseAt);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "pause",          emoDrawablePause);
	registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "stop",           emoDrawableStop);	
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "animate",        emoDrawableAnimate);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "isAnimationFinished", emoDrawableIsAnimationFinished);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getFrameIndex",  emoDrawableGetFrameIndex);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "getFrameCount",  emoDrawableGetFrameCount);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "setLine",        emoDrawableSetLinePosition);
    
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "loadSnapshot",   emoDrawableLoadSnapshot);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "stopSnapshot",   emoDrawableDisableSnapshot);
    registerClassFunc(engine.sqvm, EMO_STAGE_CLASS,    "removeSnapshot",   emoDrawableRemoveSnapshot);
}

/*
 * create drawable instance(single sprite)
 */
SQInteger emoDrawableCreateSprite(HSQUIRRELVM v) {
	EmoDrawable* drawable = [[EmoDrawable alloc] init];
	
    [drawable initDrawable];
	
    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);
		
        if (strlen(name) > 0) {
			drawable.name = [NSString stringWithCString:name encoding:NSUTF8StringEncoding];
        }
    } else {
		drawable.name = nil;
	}
	
    int width  = 0;
    int height = 0;
	
    if (drawable.name != nil && (!loadPngSizeFromAsset(drawable.name, 
				&width, &height) || width <= 0 || height <= 0)) {
        [drawable release];
        return 0;
    }
	
	drawable.width  = width;
	drawable.height = height;
	drawable.frameWidth  = width;
	drawable.frameHeight = height;
	
    [drawable createTextureBuffer];
	
    char key[DRAWABLE_KEY_LENGTH];
	[drawable updateKey:key];
    [engine addDrawable:drawable withKey:key];
	
    sq_pushstring(v, key, strlen(key));
	
	[drawable release];
	
    return 1;
}

SQInteger emoDrawableCreateSnapshot(HSQUIRRELVM v) {
	EmoSnapshotDrawable* drawable = [[EmoSnapshotDrawable alloc] init];
	
    [drawable initDrawable];
    [drawable createTextureBuffer];
	
    char key[DRAWABLE_KEY_LENGTH];
	[drawable updateKey:key];
    [engine addDrawable:drawable withKey:key];
	
    sq_pushstring(v, key, strlen(key));
	
	[drawable release];
	
    return 1;
}

SQInteger emoDrawableCreateLine(HSQUIRRELVM v) {
	SQInteger nargs = sq_gettop(v);
	if (nargs < 5) {
		return 0;
	}
	
	EmoLineDrawable* drawable = [[EmoLineDrawable alloc] init];
	
    [drawable initDrawable];
	
	SQFloat x1, y1, x2, y2;
	sq_getfloat(v, 2, &x1);
	sq_getfloat(v, 3, &y1);
	sq_getfloat(v, 4, &x2);
	sq_getfloat(v, 5, &y2);
	
	drawable.x = x1;
	drawable.y = y1;
	drawable.x2 = x2;
	drawable.y2 = y2;
	
	drawable.width  = 1;
	drawable.height = abs(y1 - y2);
	
    [drawable createTextureBuffer];
	
    char key[DRAWABLE_KEY_LENGTH];
	[drawable updateKey:key];
    [engine addDrawable:drawable withKey:key];
	
    sq_pushstring(v, key, strlen(key));
	
	[drawable release];
	
    return 1;
}

/*
 * create drawable instance (sprite sheet)
 */
SQInteger emoDrawableCreateSpriteSheet(HSQUIRRELVM v) {
	EmoDrawable* drawable = [[EmoDrawable alloc] init];
	
    [drawable initDrawable];
	
    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);
		
        if (strlen(name) > 0) {
			drawable.name = [NSString stringWithCString:name encoding:NSUTF8StringEncoding];
        }
    } else {
		drawable.name = nil;
	}
	
    SQInteger frameIndex = 0;
    SQInteger frameWidth = 0;
	SQInteger frameHeight = 0;
    SQInteger border = 0;
    SQInteger margin = 0;
    if (nargs >= 5 && sq_gettype(v, 3) != OT_NULL && sq_gettype(v, 4) != OT_NULL && sq_gettype(v, 5) != OT_NULL) {
        sq_getinteger(v, 3, &frameIndex);
        sq_getinteger(v, 4, &frameWidth);
        sq_getinteger(v, 5, &frameHeight);
    }
	
    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
        sq_getinteger(v, 6, &border);
    }
	
    if (nargs >= 7 && sq_gettype(v, 7) != OT_NULL) {
        sq_getinteger(v, 7, &margin);
    }
	
    int width  = 0;
    int height = 0;
    
    if ((drawable.name != nil && !loadPngSizeFromAsset(drawable.name, &width, &height)) || 
		(width <= 0 || height <= 0 || frameWidth <= 0 || frameHeight <= 0)) {
        [drawable release];
        return 0;
    }
	
    drawable.hasSheet = true;
    drawable.frame_index = frameIndex;
    drawable.width  = frameWidth;
    drawable.height = frameHeight;
	drawable.frameWidth  = frameWidth;
	drawable.frameHeight = frameHeight;
    drawable.border = border;
	
    if (margin == 0 && border != 0) {
        drawable.margin = border;
    } else {
        drawable.margin = margin;
    }
	
    drawable.frameCount = (int)floor(width / (float)(frameWidth  + border)) 
	* floor(height /(float)(frameHeight + border));
    if (drawable.frameCount <= 0) drawable.frameCount = 1;
	
    [drawable createTextureBuffer];
	
    char key[DRAWABLE_KEY_LENGTH];
	[drawable updateKey:key];
    [engine addDrawable:drawable withKey:key];
	
    sq_pushstring(v, key, strlen(key));
	
	[drawable release];
	
    return 1;
}

/*
 * load drawable
 */
SQInteger emoDrawableLoad(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
	if (drawable.name != nil) {
		EmoImage* imageInfo = nil;
		
		if ([engine hasCachedImage:drawable.name]) {
			imageInfo = [engine getCachedImage:drawable.name];
			
			drawable.texture = imageInfo;
			drawable.hasTexture = TRUE;
			
			imageInfo.referenceCount++;
		} else {
			imageInfo = [[EmoImage alloc]init];
			if (loadPngFromResource(drawable.name, imageInfo)) {
		
				// calculate the size of power of two
				imageInfo.glWidth  = nextPowerOfTwo(imageInfo.width);
				imageInfo.glHeight = nextPowerOfTwo(imageInfo.height);
				imageInfo.loaded = FALSE;
		
				drawable.texture = imageInfo;
				drawable.hasTexture = TRUE;
			
				imageInfo.referenceCount++;
		
				// assign OpenGL texture id
				[imageInfo genTextures];
				
				[engine addCachedImage:drawable.name value:imageInfo];
			} else {
				[imageInfo release];
				sq_pushinteger(v, ERR_ASSET_LOAD);
				return 1;
			}
			if (imageInfo != nil && !drawable.hasSheet) {
				if (drawable.width <= 0) {
					drawable.width = imageInfo.width;
				}
				if (drawable.height <= 0) {
					drawable.height = imageInfo.height;
				}
			}
			[imageInfo release];
		}
	}
	
    // drawable x
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        drawable.x = x;
    }
	
    // drawable y
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat y;
        sq_getfloat(v, 4, &y);
        drawable.y = y;
    }
	
    // drawable width
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        SQInteger width;
        sq_getinteger(v, 5, &width);
        drawable.width = width;
    }
	
    // drawable height
    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
        SQInteger height;
        sq_getinteger(v, 6, &height);
        drawable.height = height;
    }
	
	if (drawable.width == 0)  drawable.width  = 1;
	if (drawable.height == 0) drawable.height = 1;
	
    if ([drawable bindVertex]) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_CREATE_VERTEX);
    }
	
    return 1;
}

/*
 * load snapshot drawable
 */
SQInteger emoDrawableLoadSnapshot(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    // drawable x
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        drawable.x = x;
    }
	
    // drawable y
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat y;
        sq_getfloat(v, 4, &y);
        drawable.y = y;
    }
	
	drawable.width  = engine.stage.bufferWidth;
	drawable.height = engine.stage.bufferHeight;
	
    [engine enableOffscreen];
    [engine bindOffscreenFramebuffer];
    
    if ([drawable bindVertex]) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_CREATE_VERTEX);
    }
	
    return 1;
}

SQInteger emoDrawableCreateMapSprite(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, 0);
        return 1;
    }
	
    // create parent sprite
    EmoMapDrawable* parent = [[EmoMapDrawable alloc] init];
	[parent setChild:drawable];
    [parent initDrawable];
	
    drawable.independent = FALSE;
	
    [parent setFrameCount:1];
    [parent createTextureBuffer];
	
    char key[DRAWABLE_KEY_LENGTH];
	[parent updateKey:key];
    [engine addDrawable:parent withKey:key];
	
    sq_pushstring(v, key, strlen(key));
	
	[parent release];
	
    return 1;
}

SQInteger emoDrawableLoadMapSprite(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoMapDrawable* parent = (EmoMapDrawable*)[engine getDrawable:id];
	
    if (parent == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    EmoDrawable* drawable = [parent getChild];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    // load drawable texture
	if (drawable.name != nil) {
		EmoImage* imageInfo = nil;
		
		if ([engine hasCachedImage:drawable.name]) {
			imageInfo = [engine getCachedImage:drawable.name];
			
			drawable.texture = imageInfo;
			drawable.hasTexture = TRUE;
			
			imageInfo.referenceCount++;
		} else {
			imageInfo = [[EmoImage alloc]init];
			if (loadPngFromResource(drawable.name, imageInfo)) {
				
				// calculate the size of power of two
				imageInfo.glWidth  = nextPowerOfTwo(imageInfo.width);
				imageInfo.glHeight = nextPowerOfTwo(imageInfo.height);
				imageInfo.loaded = FALSE;
				
				drawable.texture = imageInfo;
				drawable.hasTexture = TRUE;
				
				imageInfo.referenceCount++;
				
				// assign OpenGL texture id
				[imageInfo genTextures];
				
				[engine addCachedImage:drawable.name value:imageInfo];
			} else {
				[imageInfo release];
				sq_pushinteger(v, ERR_ASSET_LOAD);
				return 1;
			}
			if (imageInfo != nil && !drawable.hasSheet) {
				if (drawable.width <= 0) {
					drawable.width = imageInfo.width;
				}
				if (drawable.height <= 0) {
					drawable.height = imageInfo.height;
				}
			}
			[imageInfo release];
		}
	}
		
    // parent x
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        parent.x = x;
    }
	
    // parent y
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat y;
        sq_getfloat(v, 4, &y);
        parent.y = y;
    }
	
    // parent width
    SQInteger width = engine.stage.width;
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        sq_getinteger(v, 5, &width);
    }
	
    // parent height
    SQInteger height = engine.stage.height;
    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
        sq_getinteger(v, 6, &height);
    }
	
	if (parent.width  <= 0) parent.width  = width;
    if (parent.height <= 0) parent.height = height;
	
    if ([parent bindVertex]) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_CREATE_VERTEX);
    }
	
    return 1;
}

SQInteger emoDrawableAddTileRow(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoMapDrawable* drawable = (EmoMapDrawable*)[engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) == OT_ARRAY) {
		NSMutableArray *tile = [NSMutableArray array];
        sq_pushnull(v);
        while(SQ_SUCCEEDED(sq_next(v, -2))) {
            if (sq_gettype(v, -1) != OT_NULL) {
                SQInteger value;
                sq_getinteger(v, -1, &value);
                [tile addObject: [NSNumber numberWithInt:value]];
            } else {
                [tile addObject: [NSNumber numberWithInt:0]];
            }
            sq_pop(v, 2);
        }
		
        [drawable addRow:tile];
		
        sq_pop(v, 1);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableClearTiles(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoMapDrawable* drawable = (EmoMapDrawable*)[engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if ([drawable clearTiles]) {
        sq_pushinteger(v, EMO_NO_ERROR);
        return 1;
    } else {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
}

SQInteger emoDrawableSetTileAt(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoMapDrawable* drawable = (EmoMapDrawable*)[engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 5 && sq_gettype(v, 3) != OT_NULL &&
		sq_gettype(v, 4) != OT_NULL &&
		sq_gettype(v, 5) != OT_NULL) {
        SQInteger row;
        SQInteger column;
        SQInteger value;
        sq_getinteger(v, 3, &row);
        sq_getinteger(v, 4, &column);
        sq_getinteger(v, 5, &value);
		
        if (![drawable setTileAt:row column:column value:value]) {
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

SQInteger emoDrawableGetTileAt(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }
	
    EmoMapDrawable* drawable = (EmoMapDrawable*)[engine getDrawable:id];
	
    if (drawable == nil) {
        return 0;
    }
	
    if (nargs >= 4 && sq_gettype(v, 3) != OT_NULL &&
		sq_gettype(v, 4) != OT_NULL) {
        SQInteger row;
        SQInteger column;
        sq_getinteger(v, 3, &row);
        sq_getinteger(v, 4, &column);
		
        sq_pushinteger(v, [drawable getTileAt:row column:column]);
    } else {
        return 0;
    }
	
    return 1;
}

SQInteger emoDrawableGetTileIndexAtCoord(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }
	
    EmoMapDrawable* drawable = (EmoMapDrawable*)[engine getDrawable:id];
	
    if (drawable == nil) {
        return 0;
    }
	
    if (nargs >= 4 && sq_gettype(v, 3) != OT_NULL &&
		sq_gettype(v, 4) != OT_NULL) {
        SQFloat x;
        SQFloat y;
        sq_getfloat(v, 3, &x);
        sq_getfloat(v, 4, &y);
		
        NSArray* index = [drawable getTileIndexAtCoord:x y:y];
		
        if ([index count] < 2) return 0;
		
        sq_newarray(v, 0);
		
        sq_pushinteger(v, [[index objectAtIndex:0] intValue]);
        sq_arrayappend(v, -2);
		
        sq_pushinteger(v, [[index objectAtIndex:1] intValue]);
        sq_arrayappend(v, -2);
		
        sq_push(v, -1);
    } else {
        return 0;
    }
	
    return 1;
}

SQInteger emoDrawableGetTilePositionAtCoord(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }
	
    EmoMapDrawable* drawable = (EmoMapDrawable*)[engine getDrawable:id];
	
    if (drawable == nil) {
        return 0;
    }
	
    if (nargs >= 4 && sq_gettype(v, 3) != OT_NULL &&
		sq_gettype(v, 4) != OT_NULL) {
        SQFloat x;
        SQFloat y;
        sq_getfloat(v, 3, &x);
        sq_getfloat(v, 4, &y);
		
        NSArray* position = [drawable getTilePositionAtCoord:x y:y];
		
        if ([position count] < 2) return 0;
		
        sq_newarray(v, 0);
		
        sq_pushfloat(v, [[position objectAtIndex:0] intValue]);
        sq_arrayappend(v, -2);
		
        sq_pushfloat(v, [[position objectAtIndex:1] intValue]);
        sq_arrayappend(v, -2);
    } else {
        return 0;
    }
	
    return 1;
}

SQInteger emoDrawableUseMeshMapSprite(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoMapDrawable* parent = (EmoMapDrawable*)[engine getDrawable:id];
	
    if (parent == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
    
    SQBool useMesh = false;
    
    if (getBool(v, 3, &useMesh)) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
    
    parent.useMesh = useMesh;
    
    return 1;
}

/*
 * move drawable
 */
SQInteger emoDrawableMove(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        drawable.x = x;
    }
	
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat y;
        sq_getfloat(v, 4, &y);
        drawable.y = y;
    }
	
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        SQFloat z;
        sq_getfloat(v, 5, &z);
        drawable.z = z;
        engine.sortOrderDirty = TRUE;
    }
	return 0;
}

/*
 * scale drawable
 */
SQInteger emoDrawableScale(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    // scale x
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 3, &f);
        [drawable setScale:0 withValue:f];
    }
	
    // scale y
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 4, &f);
        [drawable setScale:1 withValue:f];
    }
	
    // center x
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 5, &f);
        [drawable setScale:2 withValue:f];
    } else {
        [drawable setScale:2 withValue:drawable.width * 0.5f];
    }
	
    // center y
    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 6, &f);
        [drawable setScale:3 withValue:f];
    } else {
        [drawable setScale:3 withValue:drawable.height * 0.5f];
    }
	
    return 0;
}

/*
 * rotate drawable
 */
SQInteger emoDrawableRotate(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    // angle
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 3, &f);
        [drawable setRotate:0 withValue:f];
    }
	
    // center x
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 4, &f);
        [drawable setRotate:1 withValue:f];
    } else {
        [drawable setRotate:1 withValue:drawable.width * 0.5f];
    }
	
    // center y
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 5, &f);
        [drawable setRotate:2 withValue:f];
    } else {
        [drawable setRotate:2 withValue:drawable.height * 0.5f];
    }
	
    // rotate axis
    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 6, &f);
        [drawable setRotate:3 withValue:f];
    } else {
        [drawable setRotate:3 withValue:AXIS_Z];
    }
	
    return 0;
}

/*
 * set color of the drawable
 */
SQInteger emoDrawableColor(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat r;
        sq_getfloat(v, 3, &r);
        [drawable setColor:0 withValue:r];
    }
	
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat g;
        sq_getfloat(v, 4, &g);
        [drawable setColor:1 withValue:g];
    }
	
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        SQFloat b;
        sq_getfloat(v, 5, &b);
        [drawable setColor:2 withValue:b];
    }
	
    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
        SQFloat a;
        sq_getfloat(v, 6, &a);
        [drawable setColor:3 withValue:a];
    }
	return 0;
}

/*
 * remove drawable
 */
SQInteger emoDrawableRemove(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if ([engine removeDrawable:id]) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_ASSET_UNLOAD);
    }
	
    return 1;
}

/*
 * remove snapshot drawable
 */
SQInteger emoDrawableRemoveSnapshot(HSQUIRRELVM v) {
    [engine disableOffscreen];
    engine.stage.dirty = TRUE;
    return emoDrawableRemove(v);
}

/*
 * disable snapshot
 */
SQInteger emoDrawableDisableSnapshot(HSQUIRRELVM v) {
    if (!engine.useOffscreen) {
        sq_pushinteger(v, EMO_ERROR);
        return 1;
    }
    
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
    
    drawable.width  = engine.stage.width;
    drawable.height = engine.stage.height;
    
    // fix rotation and scale center
    [drawable setRotate:1 withValue:drawable.width  * 0.5f];
    [drawable setRotate:2 withValue:drawable.height * 0.5f];
    [drawable setScale:2  withValue:drawable.width  * 0.5f];
    [drawable setScale:3  withValue:drawable.height * 0.5f];
    
    [engine disableOffscreen];
    engine.stage.dirty = TRUE;
    
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * set onDraw interval
 */
SQInteger emoSetOnDrawInterval(HSQUIRRELVM v) {
    SQInteger oldInterval = engine.onDrawDrawablesInterval;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        SQInteger interval;
        sq_getinteger(v, 2, &interval);
        [engine updateOnDrawDrawablesInterval:interval];
    }
    sq_pushinteger(v, oldInterval);
	
	return 1;
}

SQInteger emoSetViewport(HSQUIRRELVM v) {
	
    SQInteger width  = engine.stage.viewport_width;
    SQInteger height = engine.stage.viewport_height;
	
    if (sq_gettype(v, 2) != OT_NULL && sq_gettype(v, 3) != OT_NULL) {
        sq_getinteger(v, 2, &width);
        sq_getinteger(v, 3, &height);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
	
    engine.stage.viewport_width  = width;
    engine.stage.viewport_height = height;
    engine.stage.dirty = TRUE;
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoSetStageSize(HSQUIRRELVM v) {
    SQInteger width  = engine.stage.width;
    SQInteger height = engine.stage.height;
	
    if (sq_gettype(v, 2) != OT_NULL && sq_gettype(v, 3) != OT_NULL) {
        sq_getinteger(v, 2, &width);
        sq_getinteger(v, 3, &height);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
	
    engine.stage.width  = width;
    engine.stage.height = height;
    engine.stage.dirty = TRUE;
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}
SQInteger emoSetStageColor(HSQUIRRELVM v) {
    float red   = [engine.stage getColor:0];
    float green = [engine.stage getColor:1];
    float blue  = [engine.stage getColor:2];
    float alpha = [engine.stage getColor:3];
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getfloat(v, 2, &red);
    }
    if (sq_gettype(v, 3) != OT_NULL) {
        sq_getfloat(v, 3, &green);
    }
    if (sq_gettype(v, 4) != OT_NULL) {
        sq_getfloat(v, 4, &blue);
    }
    if (sq_gettype(v, 5) != OT_NULL) {
        sq_getfloat(v, 5, &alpha);
    }
	
    [engine.stage setColor:0 withValue:red];
    [engine.stage setColor:1 withValue:green];
    [engine.stage setColor:2 withValue:blue];
    [engine.stage setColor:3 withValue:alpha];
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}
SQInteger emoGetWindowWidth(HSQUIRRELVM v) {
    sq_pushinteger(v, engine.stage.width);
    return 1;
}

SQInteger emoGetWindowHeight(HSQUIRRELVM v) {
    sq_pushinteger(v, engine.stage.height);
    return 1;
}

SQInteger emoDrawableShow(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		sq_pushinteger(v, ERR_INVALID_ID);
		return 1;
    }
	
    [drawable setColor:3 withValue:1];
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}

SQInteger emoDrawableHide(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		sq_pushinteger(v, ERR_INVALID_ID);
		return 1;
    }
	
    [drawable setColor:3 withValue:0];
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}

SQInteger emoDrawableColorRed(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat color;
        sq_getfloat(v, 3, &color);
        [drawable setColor:0 withValue:color];
    }
	
    sq_pushinteger(v, [drawable getColor:0]);
    return 1;
}

SQInteger emoDrawableColorGreen(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat color;
        sq_getfloat(v, 3, &color);
        [drawable setColor:1 withValue:color];
    }
	
    sq_pushinteger(v, [drawable getColor:1]);
    return 1;
}

SQInteger emoDrawableColorBlue(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat color;
        sq_getfloat(v, 3, &color);
        [drawable setColor:2 withValue:color];
    }
	
    sq_pushinteger(v, [drawable getColor:2]);
    return 1;
}

SQInteger emoDrawableColorAlpha(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
	}
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat color;
        sq_getfloat(v, 3, &color);
        [drawable setColor:3 withValue:color];
    }
	
    sq_pushinteger(v, [drawable getColor:3]);
    return 1;
}

SQInteger emoDrawableSetX(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        drawable.x = x;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableSetY(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat y;
        sq_getfloat(v, 3, &y);
        drawable.y = y;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableSetZ(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat z;
        sq_getfloat(v, 3, &z);
        drawable.z = z;
        engine.sortOrderDirty = TRUE;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableSetWidth(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQInteger w;
        sq_getinteger(v, 3, &w);
        drawable.width = w;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableSetHeight(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQInteger h;
        sq_getinteger(v, 3, &h);
        drawable.height = h;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableSetSize(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQInteger w;
        sq_getinteger(v, 3, &w);
        drawable.width = w;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQInteger h;
        sq_getinteger(v, 4, &h);
        drawable.height = h;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}
SQInteger emoDrawablePauseAt(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		sq_pushinteger(v, ERR_INVALID_ID);
		return 1;
	}
	
	SQInteger index;
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        sq_getinteger(v, 3, &index);		
    } else {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	
	if (![drawable pauseAt:index]) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawablePause(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		sq_pushinteger(v, ERR_INVALID_ID);
		return 1;
	}

	[drawable pause];
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableStop(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		sq_pushinteger(v, ERR_INVALID_ID);
		return 1;
	}
	
	[drawable stop];
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableAnimate(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		sq_pushinteger(v, ERR_INVALID_ID);
		return 1;
	}
    
    SQInteger start = 0;
    SQInteger count = 1;
    SQInteger interval = 0;
    SQInteger loop  = 0;
    
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        sq_getinteger(v, 3, &start);
    }
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        sq_getinteger(v, 4, &count);
    }
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        sq_getinteger(v, 5, &interval);
    }
    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
        sq_getinteger(v, 6, &loop);
    }
	
    EmoAnimationFrame* animation = [[EmoAnimationFrame alloc] init];
    animation.name  = @DEFAULT_ANIMATION_NAME;
    animation.start = start;
    animation.count = count;
    animation.interval   = interval;
    animation.loop       = loop;
    
    [drawable addAnimation:animation];
    [drawable setAnimation:animation.name];
    [drawable enableAnimation:TRUE];

	[animation release];
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}
SQInteger emoDrawableIsAnimationFinished(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    sq_pushbool(v, [drawable isAnimationFinished]);
    return 1;
}

SQInteger emoDrawableGetX(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    sq_pushinteger(v, drawable.x);
    return 1;
}

SQInteger emoDrawableGetY(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    sq_pushinteger(v, drawable.y);
    return 1;
}

SQInteger emoDrawableGetZ(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    sq_pushinteger(v, drawable.z);
    return 1;
}

SQInteger emoDrawableGetWidth(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    sq_pushinteger(v, drawable.width);
    return 1;
}

SQInteger emoDrawableGetHeight(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    sq_pushfloat(v, drawable.height);
    return 1;
}

SQInteger emoDrawableGetScaleX(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    sq_pushfloat(v, [drawable getScaleX]);
    return 1;
}

SQInteger emoDrawableGetScaleY(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    sq_pushfloat(v, [drawable getScaleY]);
    return 1;
}

SQInteger emoDrawableGetAngle(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    sq_pushfloat(v, [drawable getAngle]);
    return 1;
}

SQInteger emoDrawableGetFrameIndex(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    sq_pushinteger(v, drawable.frame_index);
    return 1;
}

SQInteger emoDrawableGetFrameCount(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
    }
	
    sq_pushinteger(v, drawable.frameCount);
    return 1;
}

SQInteger emoDrawableSetLinePosition(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    EmoLineDrawable* drawable = (EmoLineDrawable*)[engine getDrawable:id];
	
    if (drawable == nil) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (sq_gettype(v, 3) != OT_NULL) {
        SQFloat x1;
        sq_getfloat(v, 3, &x1);
        drawable.x = x1;
    }
    if (sq_gettype(v, 4) != OT_NULL) {
        SQFloat y1;
        sq_getfloat(v, 4, &y1);
        drawable.y = y1;
    }
    if (sq_gettype(v, 5) != OT_NULL) {
        SQFloat x2;
        sq_getfloat(v, 5, &x2);
        drawable.x2 = x2;
    }
    if (sq_gettype(v, 6) != OT_NULL) {
        SQFloat y2;
        sq_getfloat(v, 6, &y2);
        drawable.y2 = y2;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}
