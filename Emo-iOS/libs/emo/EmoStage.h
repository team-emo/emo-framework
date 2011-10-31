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
#import "OpenGLES/ES1/gl.h"
#import "OpenGLES/ES1/glext.h"

@interface EmoStage : NSObject {
	GLuint vbo[2];

	BOOL loaded;
	BOOL dirty;
	
	short indices[4];
	float positions[12];
	float color[4];
	
	GLint width;
	GLint height;
	GLint viewport_width;
	GLint viewport_height;
    
    GLint bufferWidth;
    GLint bufferHeight;
}
@property (readwrite) GLint width;
@property (readwrite) GLint height;
@property (readwrite) BOOL dirty;
@property (readwrite) GLint viewport_width;
@property (readwrite) GLint viewport_height;
@property (readonly) GLint bufferWidth;
@property (readonly) GLint bufferHeight;

-(GLuint)getPositionPointer;
-(GLuint)getIndicePointer;

-(void)setColor:(NSInteger)index withValue:(float)value;
-(float)getColor:(NSInteger)index;
-(void)setSize:(GLint)_width height:(GLint)_height;
-(void)setBufferSize:(GLint)_width height:(GLint)_height;
-(BOOL)loadBuffer;
-(BOOL)onDrawFrame:(NSTimeInterval)dt;
-(void)unloadBuffer;
-(void)invertSize;
@end
