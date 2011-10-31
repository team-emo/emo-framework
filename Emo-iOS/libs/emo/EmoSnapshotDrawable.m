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

#import "EmoSnapshotDrawable.h"
#import "EmoEngine.h"
#import "EmoEngine_glue.h"

extern EmoEngine* engine;

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