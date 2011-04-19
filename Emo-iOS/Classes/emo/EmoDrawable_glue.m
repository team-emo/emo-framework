#import "EmoDrawable_glue.h"
#import "EmoEngine_glue.h"
#import "Constants.h"
#import "EmoEngine.h"
#import "EmoDrawable.h"
#import "Util.h"

extern EmoEngine* engine;

/*
 * create drawable instance(single sprite)
 */
SQInteger emoDrawableCreateSprite(HSQUIRRELVM v) {
	EmoDrawable* drawable = [[[EmoDrawable alloc]init] autorelease];
	
    [drawable initDrawable];
	
    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);
		
        if (strlen(name) > 0) {
            drawable.name = [[NSString alloc] initWithUTF8String:name];
        }
    }
	
    SQFloat x = 0;
    SQFloat y = 0;
    SQFloat z = 0;
	
    if (nargs >= 3) sq_getfloat(v, 3, &x);
    if (nargs >= 4) sq_getfloat(v, 4, &y);
    if (nargs >= 5) sq_getfloat(v, 5, &z);
	
    drawable.x    = x;
    drawable.y    = y;
    drawable.z    = z;
	
    [drawable createTextureBuffer];
	
    char key[DRAWABLE_KEY_LENGTH];
	[drawable updateKey:key];
    [engine addDrawable:drawable withKey:key];
	
    sq_pushstring(v, key, strlen(key));
	
    return 1;
}

/*
 * create drawable instance (sprite sheet)
 */
SQInteger emoDrawableCreateSpriteSheet(HSQUIRRELVM v) {
	EmoDrawable* drawable = [[[EmoDrawable alloc]init] autorelease];
	
    [drawable initDrawable];
	
    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);
		
        if (strlen(name) > 0) {
            drawable.name = [[NSString alloc] initWithUTF8String:name];
        }
    }
	
    SQInteger frameIndex = 0;
    SQInteger frameWidth, frameHeight;
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
	
    if (!loadPngSizeFromAsset(drawable.name, &width, &height) || 
		width <= 0 || height <= 0 || frameWidth <= 0 || frameHeight <= 0) {
        free(drawable);
        sq_pushinteger(v, -1);
        return 1;
    }
	
    drawable.hasSheet = true;
    drawable.frame_index = frameIndex;
    drawable.width  = frameWidth;
    drawable.height = frameHeight;
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
	
	EmoImage* imageInfo = [[EmoImage alloc]init];
    if (drawable.name != nil && loadPngFromResource(drawable.name, imageInfo)) {
		
        // calculate the size of power of two
        imageInfo.glWidth  = nextPowerOfTwo(imageInfo.width);
        imageInfo.glHeight = nextPowerOfTwo(imageInfo.height);
		imageInfo.loaded = FALSE;
		
        drawable.texture = imageInfo;
        drawable.hasTexture = TRUE;
		
		if (!drawable.hasSheet) {
			drawable.width  = imageInfo.width;
			drawable.height = imageInfo.height;
        }
		
        // assign OpenGL texture id
		[imageInfo genTextures];
    } else {
        [imageInfo release];
        sq_pushinteger(v, ERR_ASSET_LOAD);
        return 1;
    }
	
    // drawable x
    if (nargs >= 3) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        drawable.x = x;
    }
	
    // drawable y
    if (nargs >= 4) {
        SQFloat y;
        sq_getfloat(v, 4, &y);
        drawable.y = y;
    }
	
    // drawable width
    if (nargs >= 5) {
        SQFloat width;
        sq_getfloat(v, 5, &width);
        drawable.width = width;
    }
	
    // drawable height
    if (nargs >= 6) {
        SQFloat height;
        sq_getfloat(v, 6, &height);
        drawable.height = height;
    }
	
    if ([drawable bindVertex]) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_CREATE_VERTEX);
    }
	
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
	
    if (nargs >= 3) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        drawable.x = x;
    }
	
    if (nargs >= 4) {
        SQFloat y;
        sq_getfloat(v, 4, &y);
        drawable.y = y;
    }
	
    if (nargs >= 5) {
        SQFloat z;
        sq_getfloat(v, 5, &z);
        drawable.z = z;
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
    if (nargs >= 3) {
        SQFloat f;
        sq_getfloat(v, 3, &f);
        [drawable setScale:0 withValue:f];
    }
	
    // scale y
    if (nargs >= 4) {
        SQFloat f;
        sq_getfloat(v, 4, &f);
        [drawable setScale:1 withValue:f];
    }
	
    // center x
    if (nargs >= 5) {
        SQFloat f;
        sq_getfloat(v, 5, &f);
        [drawable setScale:2 withValue:f];
    } else {
        [drawable setScale:2 withValue:drawable.width * 0.5f];
    }
	
    // center y
    if (nargs >= 6) {
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
    if (nargs >= 3) {
        SQFloat f;
        sq_getfloat(v, 3, &f);
        [drawable setRotate:0 withValue:f];
    }
	
    // center x
    if (nargs >= 4) {
        SQFloat f;
        sq_getfloat(v, 4, &f);
        [drawable setRotate:1 withValue:f];
    } else {
        [drawable setRotate:1 withValue:drawable.width * 0.5f];
    }
	
    // center y
    if (nargs >= 5) {
        SQFloat f;
        sq_getfloat(v, 5, &f);
        [drawable setRotate:2 withValue:f];
    } else {
        [drawable setRotate:2 withValue:drawable.height * 0.5f];
    }
	
    // rotate axis
    if (nargs >= 6) {
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
	
    if (nargs >= 3) {
        SQFloat r;
        sq_getfloat(v, 3, &r);
        [drawable setColor:0 withValue:r];
    }
	
    if (nargs >= 4) {
        SQFloat g;
        sq_getfloat(v, 4, &g);
        [drawable setColor:1 withValue:g];
    }
	
    if (nargs >= 5) {
        SQFloat b;
        sq_getfloat(v, 5, &b);
        [drawable setColor:2 withValue:b];
    }
	
    if (nargs >= 6) {
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
 * set onDraw interval
 */
SQInteger emoSetOnDrawInterval(HSQUIRRELVM v) {
    SQInteger oldInterval = engine.onDrawDrawablesInterval;
	
    if (sq_gettype(v, 2) == OT_INTEGER) {
        SQInteger interval;
        sq_getinteger(v, 2, &interval);
        engine.onDrawDrawablesInterval = interval;
    }
    sq_pushinteger(v, oldInterval);
	
	return 1;
}

SQInteger emoSetViewport(HSQUIRRELVM v) {
	
    SQInteger width  = engine.stage.viewport_width;
    SQInteger height = engine.stage.viewport_height;
	
    if (sq_gettype(v, 2) == OT_INTEGER && sq_gettype(v, 3) == OT_INTEGER) {
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
	
    if (sq_gettype(v, 2) == OT_INTEGER && sq_gettype(v, 3) == OT_INTEGER) {
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
