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
#import "EmoLiquidDrawable.h"

@implementation EmoLiquidDrawable
@synthesize segmentCount;
-(void)initDrawable {
	[super initDrawable];
    
    textureCoords = NULL;
    segmentCoords = NULL;
    segmentCount  = 0;
}

-(BOOL)bindVertex {
    // set default segment count if empty
    int count = segmentCount <= 0 ? 18 : segmentCount;
    
    if (![self updateSegmentCount:count]) return FALSE;
    if (![super bindVertex]) return FALSE;
    
	return TRUE;
}

-(BOOL)updateSegmentCount:(NSInteger)count {
    if (count <= 0) return FALSE;
    if (count == segmentCount) return TRUE;
    
    segmentCount = count;
    
    if (textureCoords != NULL) free(textureCoords);
    if (segmentCoords != NULL) free(segmentCoords);
    
    textureCoords = (float*)malloc(sizeof(float) * segmentCount * 2);
    segmentCoords = (float*)malloc(sizeof(float) * segmentCount * 2);
    
    for (int i = 0; i < segmentCount * 2; i++) {
        textureCoords[i] = 0.0f;
        segmentCoords[i] = 0.0f;
    }
    
    return TRUE;
}

-(BOOL)updateTextureCoords:(NSInteger)index tx:(float)tx ty:(float)ty {
    if (index >= segmentCount) return FALSE;
    
    int realIndex = index * 2;
    textureCoords[realIndex]     = tx;
    textureCoords[realIndex + 1] = ty;
    
    return TRUE;
}

-(BOOL)updateSegmentCoords:(NSInteger)index sx:(float)sx sy:(float)sy {
    if (index >= segmentCount) return FALSE;
    
    int realIndex = index * 2;
    segmentCoords[realIndex]     = sx;
    segmentCoords[realIndex + 1] = sy;
    
    return TRUE;
}

-(BOOL)onDrawFrame:(NSTimeInterval)dt withStage:(EmoStage*)stage {
	if (!loaded) return FALSE;
    if (segmentCount <= 0) return FALSE;
    
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
	
    // update colors
    glColor4f(param_color[0], param_color[1], param_color[2], param_color[3]);
    
	if ([self hasTexture]) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture.textureId);
        
        glTexCoordPointer(2, GL_FLOAT, 0, textureCoords);
    } else {
        glDisable(GL_TEXTURE_2D);
    }
    
    glVertexPointer(2, GL_FLOAT, 0, segmentCoords);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segmentCount);
    
	return TRUE;
}
-(void)doUnload:(BOOL)doAll {
	if (!loaded) return;
    
    if (textureCoords != NULL) free(textureCoords);
    if (segmentCoords != NULL) free(segmentCoords);
    
    textureCoords = NULL;
    segmentCoords = NULL;
    
    [super doUnload:doAll];
}

@end
