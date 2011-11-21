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
#import "EmoPointDrawable.h"

@implementation EmoPointDrawable
@synthesize pointCount;

-(void)initDrawable {
	[super initDrawable];
    
    pointCoords = NULL;
    glGetIntegerv(GL_SMOOTH_POINT_SIZE_RANGE, pointSizeRange);
    
    [self updatePointCount:1];
}

-(BOOL)bindVertex {
    if (![self updatePointCount:pointCount]) return FALSE;
    if (![super bindVertex]) return FALSE;
    
	return TRUE;
}

-(BOOL)updatePointCount:(NSInteger)count {
    if (count <= 0) return FALSE;
    if (count == pointCount) return TRUE;
    
    if (pointCoords != NULL) free(pointCoords);
    
    pointCount  = count;
    pointCoords = (float*)malloc(sizeof(float) * pointCount * 2);
    
    for (int i = 0; i < pointCount * 2; i++) {
        pointCoords[i] = 0.0f;
    }
    
    return TRUE;
}

-(BOOL)updatePointCoords:(NSInteger)index px:(float)px py:(float)py {
    if (index >= pointCount) return FALSE;
    
    int realIndex = index * 2;
    pointCoords[realIndex]     = px;
    pointCoords[realIndex + 1] = py;
    
    return TRUE;
}

-(BOOL)onDrawFrame:(NSTimeInterval)dt withStage:(EmoStage*)stage {
	if (!loaded) return FALSE;
    if (pointCoords == NULL) return FALSE;
    
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
	
    // update colors
    glColor4f(param_color[0], param_color[1], param_color[2], param_color[3]);
    
	if ([self hasTexture]) {
        glEnable(GL_POINT_SPRITE_OES);
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, texture.textureId);
    } else {
        glDisable(GL_TEXTURE_2D);
    }
    
    if (width > pointSizeRange[1]) width = pointSizeRange[1];
    
    glPointSize(width);
    glVertexPointer(2, GL_FLOAT, 0, pointCoords);
    glTexCoordPointer(2, GL_FLOAT, 0, vertex_tex_coords);
    glDrawArrays(GL_POINTS, 0, pointCount);
    
    if ([self hasTexture]) {
        glDisable(GL_POINT_SPRITE_OES);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
    
	return TRUE;
}
-(void)doUnload:(BOOL)doAll {
	if (!loaded) return;
    
    if (pointCoords != NULL) free(pointCoords);
    pointCoords = NULL;
    
    [super doUnload:doAll];
}

@end
