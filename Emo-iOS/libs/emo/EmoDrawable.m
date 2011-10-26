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
#import <OpenGLES/ES2/glext.h>

#include <EmoDrawable.h>
#include <squirrel.h>
#include <Constants.h>
#include <VmFunc.h>
#include <EmoEngine.h>
#include <EmoEngine_glue.h>
#include <Util.h>

extern EmoEngine* engine;

@implementation EmoAnimationFrame
@synthesize name;
@synthesize start, count, loop, interval;
@synthesize lastOnAnimationInterval;
@synthesize frames;
- (id)init {
    self = [super init];
    if (self != nil) {
        start      = 0;
        count      = 1;
        interval   = 0;
        loop       = 0;
        currentLoopCount = 0;
        currentCount = 0;
        lastOnAnimationInterval = [engine uptime];
        frames = nil;
    }
    return self;
}

-(BOOL)isFinished {
	return (loop >= 0 && currentLoopCount > loop);
}

-(void)initializeFrames {
    if (frames != nil) free(frames);
    frames = (NSInteger *)malloc(sizeof(NSInteger) * count);
    for (int i = 0; i < count; i++) {
        frames[i] = 0;
    }
}

-(void)setFrame:(NSInteger)index withValue:(NSInteger)value {
    frames[index] = value;
}

-(NSInteger)getNextIndex:(NSInteger)frameCount withIndex:(NSInteger)currentIndex {
	
	if ([self isFinished]) {
		return currentIndex;
	}
	
	currentCount++;
	
	if (currentCount >= count) {
		currentCount = 0;
		if (loop >= 0) {
			currentLoopCount++;
		}
	}
	
	if ([self isFinished]) {
		return currentIndex;
	} else if (currentCount + start >= frameCount) {
		currentCount = 0;
	}
	
    if (frames != nil) {
        return frames[currentCount];
    } else {
        return currentCount + start;
	}
}
-(NSTimeInterval)getLastOnAnimationDelta:(NSTimeInterval)uptime {
	return (uptime - lastOnAnimationInterval) * 1000;
}
-(void)dealloc {
	[name release];
	name = nil;
    
    if (frames != nil) free(frames);
    
	[super dealloc];
}

@end

@implementation EmoImagePackInfo 
@synthesize name;
@synthesize x, y, width, height, index;
-(void)dealloc {
    [name release];
    name = nil;
    [super dealloc];
}
@end

@implementation EmoImagePackParser
@synthesize drawable;
@synthesize frameIndex;

- (id)init {
    self = [super init];
    if (self != nil) {
        itemCount  = 0;
        frameIndex = 0;
    }
    return self;
}

- (void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName
  namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName
    attributes:(NSDictionary *)attributeDict {
    if ([elementName isEqualToString:@"Imageset"] || [elementName isEqualToString:@"TextureAtlas"]) {
        for (id key in attributeDict) {
            if ([key isEqualToString:@"Imagefile"] || [key isEqualToString:@"imagePath"]) {
                drawable.name = [attributeDict objectForKey:key];
                break;
            }
        }
    } else if ([elementName isEqualToString:@"Image"] || [elementName isEqualToString:@"SubTexture"]) {
        EmoImagePackInfo* info = [[EmoImagePackInfo alloc]init];
        
        for (id key in attributeDict) {
            if ([key isEqualToString:@"name"] || [key isEqualToString:@"Name"]) {
                info.name = [attributeDict objectForKey:key];
            } else if ([key isEqualToString:@"x"] || [key isEqualToString:@"XPos"]) {
                info.x = [[attributeDict objectForKey:key] intValue];
            } else if ([key isEqualToString:@"y"] || [key isEqualToString:@"YPos"]) {
                info.y = [[attributeDict objectForKey:key] intValue];
            } else if ([key isEqualToString:@"width"] || [key isEqualToString:@"Width"]) {
                info.width = [[attributeDict objectForKey:key] intValue];
            } else if ([key isEqualToString:@"height"] || [key isEqualToString:@"Height"]) {
                info.height = [[attributeDict objectForKey:key] intValue];
            }
        }
        if ([info.name length] > 0) {
            info.index = itemCount;
            if (info.index == frameIndex) {
                drawable.width  = info.width;
                drawable.height = info.height;
                drawable.frameWidth  = info.width;
                drawable.frameHeight = info.height;
                drawable.margin = 0;
                drawable.border = 0;
            }
            [drawable addImagePack:info];
            itemCount++;
        }
        [info release];
    }
}

@end

@interface EmoDrawable (PrivateMethods)
-(NSInteger)tex_coord_frame_startX;
-(NSInteger)tex_coord_frame_startY;
-(float)getTexelHalfX;
-(float)getTexelHalfY;
@end

@implementation EmoDrawable
@synthesize name;
@synthesize x, y, z;
@synthesize width, height;
@synthesize frameWidth, frameHeight;
@synthesize hasTexture, hasSheet;
@synthesize texture;
@synthesize frameCount, frame_index, border, margin;
@synthesize independent;
@synthesize loaded;
@synthesize isScreenEntity;
@synthesize isPackedAtlas;
@synthesize useFont;

-(BOOL)loadPackedAtlasXml:(NSInteger)initialFrameIndex {
    // check if the length is shorter than the length of ".xml"
    if ([name length] <= 4) return false;

    NSString* path = [[NSBundle mainBundle] pathForResource:name ofType:nil];
    
    if (path == nil) {
        NSLOGE(@"Requested resource is not found:");
        NSLOGE(name);
        return FALSE;
    }
    NSURL *url = [NSURL fileURLWithPath:path];
    
    EmoImagePackParser* pack = [[EmoImagePackParser alloc]init];
    pack.drawable = self;
    pack.frameIndex = initialFrameIndex;
    
    NSXMLParser *parser = [[NSXMLParser alloc] initWithContentsOfURL:url];
    [parser setDelegate:pack];
    [parser parse];
    [parser release];
    
    [pack release];

    frameCount = [imagepacks_names count];
    
    return TRUE;
}

-(BOOL)selectFrame:(NSString*)_name {
    EmoImagePackInfo* info = [self getImagePack:_name];
    if (info == nil) return FALSE;
    
    return [self setFrameIndex:info.index];
}

-(void)addImagePack:(EmoImagePackInfo*) info {
	[self deleteImagePack:info.name];
	[imagepacks setObject:info forKey:info.name];
    [imagepacks_names addObject:info.name];
}

-(EmoImagePackInfo*)getImagePack:(NSString*)_name {
	return [imagepacks objectForKey:_name];
}

-(BOOL)deleteImagePack:(NSString*)_name {
	EmoImagePackInfo* info = [self getImagePack:_name];
    
	if (info == nil) return FALSE;
	
	[imagepacks removeObjectForKey:_name];
	
	return TRUE;
}

-(void)deleteImagePacks {
	[imagepacks removeAllObjects];
}


-(BOOL)onDrawFrame:(NSTimeInterval)dt withStage:(EmoStage*)stage {
    if (!loaded) return FALSE;
	
	if (frameIndexChanged) {
		frame_index = nextFrameIndex;
		frameIndexChanged = FALSE;
	}

	if (animating && currentAnimation != nil) {
		EmoAnimationFrame* animation = currentAnimation;
		NSTimeInterval delta = [animation getLastOnAnimationDelta:[engine uptime]];
		if (delta >= animation.interval) {
			[self setFrameIndex:[animation getNextIndex:frameCount withIndex:frame_index]];
			animation.lastOnAnimationInterval = [engine uptime];
		}
	}
	
	if (frames_vbos[frame_index] <= 0) {
		[self bindVertex];
	}
	
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity (); 
	
    // update colors
    glColor4f(param_color[0], param_color[1], param_color[2], param_color[3]);
	
    // update position
    glTranslatef(x * orthFactorX, y * orthFactorY, 0);
	
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
	
    // update width and height
    glScalef(width, height, 1);
	
    // bind vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, [stage getPositionPointer]);
    glVertexPointer(3, GL_FLOAT, 0, 0);
	
	// bind a texture
    if (hasTexture) {
		glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture.textureId);
        
        // bind texture coords
        glBindBuffer(GL_ARRAY_BUFFER, frames_vbos[frame_index]);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
	} else {
		glDisable(GL_TEXTURE_2D);
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
	loaded     = FALSE;
	hasSheet   = FALSE;
	animating  = FALSE;
	independent = TRUE;
    isPackedAtlas = FALSE;

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
	
	animations        = [[NSMutableDictionary alloc]init];
	nextFrameIndex    = 0;
	frameIndexChanged = FALSE;
	currentAnimation  = nil;
    
    orthFactorX = 1.0;
    orthFactorY = 1.0;
    
    isScreenEntity = TRUE;
    useFont = FALSE;
    
    imagepacks = [[NSMutableDictionary alloc]init];
    imagepacks_names = [[NSMutableArray alloc]init];
}

-(NSInteger)tex_coord_frame_startX {
    if (isPackedAtlas) {
        return [self getImagePack:[imagepacks_names objectAtIndex:frame_index]].x;
    }
	int xcount = (int)round((texture.width - (margin * 2) + border) / (float)(frameWidth  + border));
	int xindex = frame_index % xcount;
	return ((border + frameWidth) * xindex) + margin;
}

-(NSInteger) tex_coord_frame_startY {
    if (isPackedAtlas) {
        return texture.height - frameHeight - [self getImagePack:[imagepacks_names objectAtIndex:frame_index]].y;
    }
	int xcount = (int)round((texture.width - (margin * 2) + border) / (float)(frameWidth  + border));
	int ycount = (int)round((texture.height - (margin * 2) + border) / (float)(frameHeight + border));
	int yindex = ycount - (frame_index / xcount) - 1;
	return ((border + frameHeight) * yindex) + margin;
}

-(float) getTexelHalfX {
    if (hasTexture) {
        return (1.0 / texture.glWidth) * 0.5;
    } else {
        return 0;
    }   
}   

-(float) getTexelHalfY {
    if (hasTexture) {
        return (1.0 / texture.glHeight) * 0.5;
    } else {
        return 0;
    }   
}   

-(float)getTexCoordStartX {
	if (hasSheet) {
        return [self tex_coord_frame_startX] / (float)texture.glWidth + [self getTexelHalfX];
    } else {
        return 0;
    }
}

-(float)getTexCoordEndX {
	if (!hasTexture) {
		return 1 - [self getTexelHalfX];
    } else if (hasSheet) {
        return (float)([self tex_coord_frame_startX] + frameWidth) / (float)texture.glWidth - [self getTexelHalfX];
    } else {
        return (float)texture.width / (float)texture.glWidth - [self getTexelHalfX];
    }
}

-(float)getTexCoordStartY {
	if (!hasTexture) {
		return 1 - [self getTexelHalfY];
	} else if (hasSheet) {
        return (float)([self tex_coord_frame_startY] + frameHeight) / (float)texture.glHeight - [self getTexelHalfY];
    } else {
        return (float)texture.height / (float)texture.glHeight - [self getTexelHalfY];
    }
}

-(float)getTexCoordEndY {
    if (hasSheet) {
        return [self tex_coord_frame_startY] / (float)texture.glHeight + [self getTexelHalfY];
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
	
	glEnable(GL_TEXTURE_2D);
    glBindBuffer(GL_ARRAY_BUFFER, frames_vbos[frame_index]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, vertex_tex_coords, GL_STATIC_DRAW);
	
	printGLErrors("Could not create OpenGL vertex");
	
    if (hasTexture && !texture.loaded) {
		glBindTexture(GL_TEXTURE_2D, texture.textureId);
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (texture.isPVRTC_2) {
            glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,
                                   texture.width, texture.height, 0, texture.dataLength, texture.data);
        } else if (texture.isPVRTC_4) {
            glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,
                                   texture.width, texture.height, 0, texture.dataLength, texture.data);
        } else if (texture.hasAlpha) {
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
		
		texture.loaded = TRUE;
        printGLErrors("Could not bind OpenGL textures");
    }
	
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
-(BOOL)isVisible {
    return param_color[3] != 0;
}
-(float)getScaleX {
	return param_scale[0];
}
-(float)getScaleY {
	return param_scale[1];
}
-(float)getAngle {
	return param_rotate[0];
}
-(float)getScaledWidth {
	return [self getScaleX] * width;
}
-(float)getScaledHeight {
	return [self getScaleY] * height;
}
-(void)createTextureBuffer {
    frames_vbos = (GLuint *)malloc(sizeof(GLuint) * frameCount);
	
	for (int i = 0; i < frameCount; i++) {
		frames_vbos[i] = 0;
	}
	glGenBuffers (1, &frames_vbos[frame_index]);
}
-(void)doUnload:(BOOL)doAll {
	if (!loaded) return;
	if (hasTexture) {
		texture.referenceCount--;
		if (texture.referenceCount <= 0) {
			[texture doUnload];
            texture.loaded = FALSE;
			if (name != nil) [engine removeCachedImage:name];
		}
		if (doAll) {
            [texture release];
            hasTexture = FALSE;
        }
	}
	for (int i = 0; i < frameCount; i++) {
		if (frames_vbos[i] > 0) {
			glDeleteBuffers(1, &frames_vbos[i]);
		}
	}
	frameCount  = 1;
	frame_index = 0;
	free(frames_vbos);
	
    if (doAll) {
        [self deleteAnimations];
        [self deleteImagePacks];
    }
	
	loaded = FALSE;
}
-(BOOL)setFrameIndex:(NSInteger)index force:(BOOL)force {
	if (index < 0 || frameCount <= index) {
		return FALSE;
	}
    if (force) {
        frame_index = index;
    } else {
        nextFrameIndex = index;
        frameIndexChanged = TRUE;
    }
    
    if (isPackedAtlas) {
        EmoImagePackInfo* info = [self getImagePack:[imagepacks_names objectAtIndex:index]];
        self.width  = info.width;
        self.height = info.height;
        self.frameWidth  = info.width;
        self.frameHeight = info.height;
    }
    
	return TRUE;
}

-(BOOL)setFrameIndex:(NSInteger)index {
    return [self setFrameIndex:index force:FALSE];
}
-(BOOL)pauseAt:(NSInteger)index {
	if (![self setFrameIndex:index]) {
		return FALSE;
	}
	animating = FALSE;
	
	return TRUE;
}
-(void)pause {
	animating = FALSE;
}
-(void)stop {
	animating = FALSE;
	[self setFrameIndex:0];
}

-(void)addAnimation:(EmoAnimationFrame*)animation {
	[self deleteAnimation:animation.name];
	[animations setObject:animation forKey:animation.name];
}

-(BOOL)setAnimation:(NSString*)_name {
	if ([self getAnimation:_name] == nil) {
		animationName = nil;
		return FALSE;
	} else {
		animationName = _name;
	}
	return TRUE;
}

-(EmoAnimationFrame*)getAnimation:(NSString*)_name {
	return [animations objectForKey:_name];
}

-(BOOL)deleteAnimation:(NSString*)_name {
	EmoAnimationFrame* animation = [self getAnimation:_name];

	if (animation == nil) return FALSE;
	
	[animations removeObjectForKey:_name];
	
	return TRUE;
}

-(void)deleteAnimations {
	[animations removeAllObjects];
}

-(BOOL)enableAnimation:(BOOL)enable {
	animating = enable;
	
	currentAnimation = nil;
	
	if (enable) {
		if (animationName == nil) return FALSE;
		EmoAnimationFrame* animation = [self getAnimation:animationName];
		if (animation == nil) {
			return FALSE;
		} else {
			currentAnimation = animation;
			animation.lastOnAnimationInterval = [engine uptime];
			[self setFrameIndex:animation.start];
		}
	}
	return TRUE;
}
- (NSComparisonResult) compareZ:(EmoDrawable*)other {
	if (self.z < other.z) {
		return NSOrderedAscending;
	} else if (self.z > other.z) {
		return NSOrderedDescending;
	} else {
		return NSOrderedSame;
	}
}
-(void)dealloc {
	[animations release];
	[name release];
    [imagepacks release];
    [imagepacks_names release];
    
	name = nil;
	[super dealloc];
}
-(BOOL)isAnimationFinished {
	if (animating && currentAnimation != nil) {
		return [currentAnimation isFinished];
	}
	return TRUE;
}
@end

@implementation EmoLineDrawable
@synthesize x2, y2;

-(void)initDrawable {
	[super initDrawable];
	
	x2 = 0;
	y2 = 0;
	
    param_color[0] = 0;
    param_color[1] = 1;
    param_color[2] = 0;
}

-(BOOL)bindVertex {
	loaded = TRUE;
	return TRUE;
}

-(BOOL)onDrawFrame:(NSTimeInterval)dt withStage:(EmoStage*)stage {
	if (!loaded) return FALSE;
	
	vertex_tex_coords[0] = x;
	vertex_tex_coords[1] = y;
	vertex_tex_coords[2] = x2;
	vertex_tex_coords[3] = y2;
	
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity (); 
	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glLineWidth(width);
	
    // update colors
    glColor4f(param_color[0], param_color[1], param_color[2], param_color[3]);
	
    glVertexPointer(2, GL_FLOAT, 0, vertex_tex_coords);
    glDrawArrays(GL_LINES, 0, 2);
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	return TRUE;
}
@end

@implementation EmoSnapshotDrawable
/*
 * initialize drawable
 */
-(void)initDrawable {
	[super initDrawable];
    
    // snapshot drawable should be the first drawable
    z = -1;
    
    isScreenEntity = FALSE;
}
/*
 * create texture and bind OpenGL vertex
 * width and height should be set before calling bindVertex.
 */
-(BOOL)bindVertex {
    
    clearGLErrors("EmoSnapshotDrawable:bindVertex");
    
    if (!hasTexture) {
        EmoImage* imageInfo = [[EmoImage alloc]init];
        
        imageInfo.width  = width;
        imageInfo.height = height;
        
        imageInfo.glWidth  = imageInfo.width;
        imageInfo.glHeight = imageInfo.height;
        imageInfo.loaded = FALSE;
			
        imageInfo.referenceCount++;
            
        // assign OpenGL texture id
        [imageInfo genTextures];
        
        texture = imageInfo;
        hasTexture = TRUE;
    }
    
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.textureId, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    texture.loaded = TRUE;
    
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
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	printGLErrors("Could not create OpenGL vertex");
	
	loaded = TRUE;
	
	return TRUE;
}

-(BOOL)onDrawFrame:(NSTimeInterval)dt withStage:(EmoStage*)stage {
    // if the snapshot ends, use default onDrawFrame
    if (!engine.useOffscreen) {
        orthFactorX = width  / (float)stage.width;
        orthFactorY = height / (float)stage.height;
        return [super onDrawFrame:dt withStage:stage];
    }
    // vewport should be reset everytime on drawing
    glViewport(0, 0, width, height); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0, width, height, 0, -1, 1);
    
    orthFactorX = 1.0;
    orthFactorY = 1.0;
    
    glClearColor([engine.stage getColor:0], [engine.stage getColor:1],
                 [engine.stage getColor:2], [engine.stage getColor:3]);
    glClear(GL_COLOR_BUFFER_BIT);
    
    [super onDrawFrame:dt withStage:stage];
    engine.stage.dirty = TRUE;
    
    return TRUE;
}
@end

@implementation EmoFontDrawable
@synthesize fontSize, fontFace, isBold, isItalic;
@synthesize param1, param2, param3, param4, param5, param6;

-(void)initDrawable {
	[super initDrawable];
    
    fontSize = 0;
    isBold   = FALSE;
    isItalic = FALSE;
}

-(void)dealloc {
    [fontFace release];
    [param1 release];
    [param2 release];
    [param3 release];
    [param4 release];
    [param5 release];
    [param6 release];
    
    [super dealloc];
}

-(void)loadTextBitmap {
    [texture freeData];
    
    UIFont* font = [UIFont systemFontOfSize:[UIFont systemFontSize]];
    
    if ([fontFace length] > 0) {
        NSInteger size = fontSize > 0 ? fontSize : [UIFont systemFontSize];
        font = [UIFont fontWithName:fontFace size:size];
    } else if (fontSize > 0) {
        font = [UIFont systemFontOfSize:fontSize];
    }
    
    NSString* text = @" ";
    
    // extract property name
    NSString* propName = 
            [name substringFromIndex:[name rangeOfString:@"::"].location+2];

    // retrieve property value
    NSDictionary* plist = [NSDictionary dictionaryWithContentsOfFile:[
            [NSBundle mainBundle] pathForResource:@"strings" ofType:@"plist"]];  
    if ([plist objectForKey:propName] != nil) {
        NSString* formatStr = [[plist objectForKey:propName] 
                               stringByReplacingOccurrencesOfString:@"%s" withString:@"%@"];
        
        text = [NSString stringWithFormat:formatStr,
                param1, param2, param3, param4, param5, param6];
    }
    
    CGSize textSize = [text sizeWithFont:font]; 
    
    int textWidth  = textSize.width;
    int textHeight = textSize.height;
    
    GLubyte *bitmap = (GLubyte *)malloc(textWidth * textHeight * 4);
    memset(bitmap, 0, textWidth * textHeight * 4);

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(
                    bitmap, textWidth, textHeight,
                    8, textWidth * 4, colorSpace,
                    kCGImageAlphaPremultipliedLast);
    
    UIGraphicsPushContext(context);

    [[UIColor whiteColor] set];
    [text drawAtPoint:CGPointMake(0, 0) withFont:font];
    
    UIGraphicsPopContext();
    
    CGColorSpaceRelease(colorSpace);
    CGContextRelease(context);
    
    texture.width  = textWidth;
    texture.height = textHeight;
    texture.glWidth  = nextPowerOfTwo(texture.width);
    texture.glHeight = nextPowerOfTwo(texture.height);
    texture.data   = bitmap;
    texture.hasAlpha = TRUE;
    texture.freed = FALSE;
    
}
@end
