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
#import "EmoLineDrawable.h"

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
