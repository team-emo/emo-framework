#include <EmoDrawable.h>
#include <squirrel.h>
#include <common.h>
#include <sqfunc.h>
#include <EmoEngine.h>
#include <emo_engine_func.h>

extern EmoEngine* engine;

@interface EmoDrawable (PrivateMethods)
-(NSInteger)tex_coord_frame_startX;
-(NSInteger) tex_coord_frame_startY;
-(float)getTexCoordStartX;
-(float)getTexCoordEndX;
-(float)getTexCoordStartY;
-(float)getTexCoordEndY;
@end

@implementation EmoDrawable
@synthesize name;
@synthesize x, y, z;
@synthesize width, height;
@synthesize hasTexture, hasSheet;
@synthesize texture;
@synthesize frameCount, frame_index, border, margin;

-(BOOL)onDrawFrame:(NSTimeInterval)dt withStage:(EmoStage*)stage {
    if (!loaded) return FALSE;
	
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity (); 
	
    // update colors
    glColor4f(param_color[0], param_color[1], param_color[2], param_color[3]);
	
    // update position
    glTranslatef(x, y, z);
	
    // update width and height
    glScalef(width, height, 1);
	
    // rotate
    glTranslatef(param_rotate[1], param_rotate[2], 0);
    if (param_rotate[3] == AXIS_X) {
        glRotatef(param_rotate[0], 1, 0, 0);
    } else if (param_rotate[3] == AXIS_Y) {
        glRotatef(param_rotate[0], 0, 1, 0);
    } else {
        glRotatef(param_rotate[0], 0, 0, 1);
    }
    glTranslatef(-param_rotate[1], -param_rotate[2], 0);
	
    // scale
    glTranslatef(param_scale[2], param_scale[3], 0);
    glScalef(param_scale[0], param_scale[1], 1);
    glTranslatef(-param_scale[2], -param_scale[3], 0);
	
    // bind vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, [stage getPositionPointer]);
    glVertexPointer(3, GL_FLOAT, 0, 0);
	
    if (hasTexture) {
        // bind a texture
        glBindTexture(GL_TEXTURE_2D, texture.textureId);
		
        // bind texture coords
        glBindBuffer(GL_ARRAY_BUFFER, frames_vbos[frame_index]);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
    }
	
    // bind indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, [stage getIndicePointer]);
	
    // draw sprite
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, 0);
	
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return TRUE;
}
-(void)initDrawable {
	x = 0;
	y = 0;
	z = 0;
	width  = 0;
	height = 0;
	name = nil;

	hasTexture = FALSE;
	loaded = FALSE;

	// color param RGBA
    param_color[0] = 1.0f;
    param_color[1] = 1.0f;
    param_color[2] = 1.0f;
    param_color[3] = 1.0f;
	
    // rotate angle, center x, center y, axis
    param_rotate[0] = 0;
    param_rotate[1] = 0;
    param_rotate[2] = 0;
    param_rotate[3] = AXIS_Z;
	
    // scale param x, y, center x, center y
    param_scale[0] = 1;
    param_scale[1] = 1;
    param_scale[2] = 0;
    param_scale[3] = 0;
	
	frameCount  = 1;
    frame_index = 0;
    border      = 0;
    margin      = 0;
	
}

-(NSInteger)tex_coord_frame_startX {
    int xindex = frame_index % (int)floor((texture.width - margin) / (float)(width  + border));
    return ((border + width) * xindex) + margin;
}

-(NSInteger) tex_coord_frame_startY {
    int ycount = (int)floor((texture.height - margin) / (float)(height + border));
    int yindex = ycount - 1 - ((frame_index + 1) / (int)floor((texture.width - margin) / (float)(width  + border)));
    return ((border + height) * yindex) + margin;
}

-(float)getTexCoordStartX {
    if (hasSheet) {
        return [self tex_coord_frame_startX] / (float)texture.glWidth;
    } else {
        return 0;
    }
}

-(float)getTexCoordEndX {
    if (hasSheet) {
        return (float)([self tex_coord_frame_startX] + width) / (float)texture.glWidth;
    } else {
        return (float)texture.width / (float)texture.glWidth;
    }
}

-(float)getTexCoordStartY {
    if (hasSheet) {
        return (float)([self tex_coord_frame_startY] + height) / (float)texture.glHeight;
    } else {
        return (float)texture.height / (float)texture.glHeight;
    }
}

-(float)getTexCoordEndY {
    if (hasSheet) {
        return [self tex_coord_frame_startY] / (float)texture.glHeight;
    } else {
        return 0;
    }
}

-(BOOL)bindVertex {
    clearGLErrors("EmoDrawable:bindVertex");
	
    vertex_tex_coords[0] = [self getTexCoordStartX];
    vertex_tex_coords[1] = [self getTexCoordStartY];
	
    vertex_tex_coords[2] = [self getTexCoordStartX];
    vertex_tex_coords[3] = [self getTexCoordEndY];
	
    vertex_tex_coords[4] = [self getTexCoordEndX];
    vertex_tex_coords[5] = [self getTexCoordEndY];
	
    vertex_tex_coords[6] = [self getTexCoordEndX];
    vertex_tex_coords[7] = [self getTexCoordStartY];
	
	if (frames_vbos[frame_index] == 0) {
		glGenBuffers (1, &frames_vbos[frame_index]);
	}
	
    glBindBuffer(GL_ARRAY_BUFFER, frames_vbos[frame_index]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, vertex_tex_coords, GL_STATIC_DRAW);
	
	printGLErrors("Could not create OpenGL vertex");
	
    if (hasTexture) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture.textureId);
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (texture.hasAlpha) {
            GLubyte* holder = (GLubyte*)malloc(sizeof(GLubyte) * texture.glWidth * texture.glHeight * 4);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.glWidth, texture.glHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, holder);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 
							0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
            free(holder);
        } else {
            GLubyte* holder = (GLubyte*)malloc(sizeof(GLubyte) * texture.glWidth * texture.glHeight * 3);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.glWidth, texture.glHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, holder);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 
							0, 0, texture.width, texture.height, GL_RGB, GL_UNSIGNED_BYTE, texture.data);
            free(holder);
        }
		glBindTexture(GL_TEXTURE_2D, 0);
        printGLErrors("Could not bind OpenGL textures");
    }
	
	loaded = TRUE;
	
	return TRUE;
}
-(void)updateKey:(char*)key {
	double uptime = [engine uptime];
	int uptimes = (int)floor(uptime);
	int uptimem = (int)floor((uptime - uptimes) * 1000);
    sprintf(key, "%d%d-%d", uptimes, uptimem, frames_vbos[frame_index]);
}
-(void)setScale:(NSInteger)index withValue:(float)value {
	param_scale[index] = value;
}
-(void)setRotate:(NSInteger)index withValue:(float)value {
	param_rotate[index] = value;
}
-(void)setColor:(NSInteger)index withValue:(float)value {
	param_color[index] = value;
}
-(float)getColor:(NSInteger)index {
	return param_color[index];
}

-(void)createTextureBuffer {
    frames_vbos = (GLuint *)malloc(sizeof(GLuint) * frameCount);
	
	for (int i = 0; i < frameCount; i++) {
		frames_vbos[i] = 0;
	}
	glGenBuffers (1, &frames_vbos[frame_index]);
}
-(void)doUnload {
	if (hasTexture) {
		[texture doUnload];
		[texture release];
		hasTexture = FALSE;
	}
	for (int i = 0; i < frameCount; i++) {
		if (frames_vbos[i] > 0) {
			glDeleteBuffers(1, &frames_vbos[i]);
		}
	}
	frameCount  = 1;
	frame_index = 0;
	free(frames_vbos);
}
-(void)dealloc {
	[name release];
	[super dealloc];
}
@end

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
		
        drawable.texture = imageInfo;
        drawable.hasTexture = true;
		
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
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
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
		return 0;
    }
	
    EmoDrawable* drawable = [engine getDrawable:id];
	
    if (drawable == nil) {
		return 0;
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
