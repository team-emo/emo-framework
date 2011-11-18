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
#import "squirrel.h"
#import "EmoImage.h"
#import "EmoStage.h"

@interface EmoAnimationFrame : NSObject {
	NSString*   name;
	NSInteger   start;
	NSInteger   count;
	NSInteger   loop;
	NSInteger   interval;
	
	NSInteger   currentLoopCount;
	NSInteger   currentCount;
	
	NSTimeInterval lastOnAnimationInterval;
    
    NSInteger*  frames;
}
@property (copy, readwrite) NSString* name;
@property (readwrite)NSInteger start;
@property (readwrite)NSInteger count;
@property (readwrite)NSInteger loop;
@property (readwrite)NSInteger interval;
@property (readwrite)NSTimeInterval lastOnAnimationInterval;
@property (readonly) NSInteger* frames;

-(NSInteger)getNextIndex:(NSInteger)frameCount withIndex:(NSInteger)currentIndex;
-(NSTimeInterval)getLastOnAnimationDelta:(NSTimeInterval)uptime;
-(void)initializeFrames;
-(void)setFrame:(NSInteger)index withValue:(NSInteger)value;
@end

@interface EmoImagePackInfo : NSObject {
    NSString* name;
    NSInteger x;
    NSInteger y;
    NSInteger width;
    NSInteger height;
    NSInteger index;
}
@property (copy, readwrite) NSString* name;
@property (readwrite) NSInteger x, y, width, height, index;
@end

@interface EmoDrawable : NSObject <NSXMLParserDelegate> {
	GLuint* frames_vbos;
	NSString* name;
	float x;
	float y;
	float z;
	NSInteger width;
	NSInteger height;
	NSInteger frameWidth;
	NSInteger frameHeight;
	
	BOOL hasTexture;
	BOOL loaded;
	BOOL hasSheet;
	BOOL animating;
	BOOL independent;
    BOOL isPackedAtlas;
	
    float      vertex_tex_coords[8];
	
    float      param_rotate[4];
    float      param_scale[4];
    float      param_color[4];
	
	NSInteger frameCount;
	NSInteger frame_index;
	NSInteger border;
	NSInteger margin;
	
	EmoImage*  texture;
	
	NSInteger nextFrameIndex;
	BOOL frameIndexChanged;
	NSMutableDictionary* animations;
	EmoAnimationFrame* currentAnimation;
	NSString* animationName;
    
    float orthFactorX;
    float orthFactorY;
    
    BOOL isScreenEntity;
    BOOL useFont;
    
	NSMutableDictionary* imagepacks;
    NSMutableArray* imagepacks_names;
}
@property (copy, readwrite) NSString* name;
@property (readwrite) float x;
@property (readwrite) float y;
@property (readwrite) float z;
@property (readwrite) NSInteger width;
@property (readwrite) NSInteger height;
@property (readwrite) NSInteger frameWidth;
@property (readwrite) NSInteger frameHeight;
@property (readwrite) BOOL hasTexture;
@property (readwrite) BOOL hasSheet;
@property (readwrite) NSInteger frameCount;
@property (readwrite) NSInteger frame_index;
@property (readwrite) NSInteger border;
@property (readwrite) NSInteger margin;
@property (retain, readwrite, nonatomic) EmoImage* texture;
@property (readwrite) BOOL independent;
@property (readonly) BOOL loaded;
@property (readonly) BOOL isScreenEntity;
@property (readwrite) BOOL isPackedAtlas;
@property (readwrite) BOOL useFont;

-(void)doUnload:(BOOL)doAll;
-(void)initDrawable;
-(void)createTextureBuffer;
-(BOOL)bindVertex;
-(void)updateKey:(char*)key;
-(void)setScale:(NSInteger)index withValue:(float)value;
-(void)setRotate:(NSInteger)index withValue:(float)value;
-(void)setColor:(NSInteger)index withValue:(float)value;
-(BOOL)setFrameIndex:(NSInteger)index force:(BOOL)force;
-(BOOL)setFrameIndex:(NSInteger)index;
-(BOOL)pauseAt:(NSInteger)index;
-(float)getColor:(NSInteger)index;
-(BOOL)onDrawFrame:(NSTimeInterval)dt withStage:(EmoStage*)stage;
-(void)pause;
-(void)stop;
-(void)addAnimation:(EmoAnimationFrame*)animation;
-(BOOL)setAnimation:(NSString*)_name;
-(EmoAnimationFrame*)getAnimation:(NSString*)_name;
-(BOOL)deleteAnimation:(NSString*)_name;
-(void)deleteAnimations;
-(BOOL)enableAnimation:(BOOL)enable;
-(float)getScaleX;
-(float)getScaleY;
-(float)getAngle;
-(float)getScaledWidth;
-(float)getScaledHeight;
-(BOOL)isAnimationFinished;
-(BOOL)isVisible;
-(float)getTexCoordStartX;
-(float)getTexCoordEndX;
-(float)getTexCoordStartY;
-(float)getTexCoordEndY;
-(BOOL)loadPackedAtlasXml:(NSInteger)initialFrameIndex;
-(BOOL)selectFrame:(NSString*)_name;
-(void)addImagePack:(EmoImagePackInfo*) info;
-(EmoImagePackInfo*)getImagePack:(NSString*)_name;
-(BOOL)deleteImagePack:(NSString*)_name;
-(void)deleteImagePacks;
@end

@interface EmoImagePackParser : NSObject <NSXMLParserDelegate> {
    EmoDrawable* drawable;
    NSInteger frameIndex;
    NSInteger itemCount;
}
@property (assign, readwrite) EmoDrawable* drawable;
@property (readwrite) NSInteger frameIndex;
@end

