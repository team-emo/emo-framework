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
#import "Constants.h"
#import "png.h"

@interface EmoImage : NSObject {
    NSString* filename;
    GLuint   textureId;
    int      width;
    int      height;
	int      glWidth;
	int      glHeight;
    GLubyte* data;
    BOOL     hasAlpha;
	BOOL     loaded;
	int      referenceCount;
    BOOL     freed;
    BOOL     isPVRTC_2;
    BOOL     isPVRTC_4;
    int      dataLength;
}
@property (copy, readwrite) NSString* filename;
@property (readwrite) GLuint textureId;
@property (readwrite) int    width, height, glWidth, glHeight;
@property (assign, readwrite) GLubyte* data;
@property (readwrite) BOOL hasAlpha;
@property (readwrite) BOOL loaded;
@property (readwrite) int referenceCount;
@property (readwrite) BOOL freed;
@property (readwrite) BOOL isPVRTC_2;
@property (readwrite) BOOL isPVRTC_4;
@property (readwrite) int  dataLength;

-(void)genTextures;
-(void)doUnload;
-(void)freeData;
@end

BOOL loadImageSizeFromResource(NSString* filename, int *width, int *height);
BOOL loadImageFromResource(NSString* filename, EmoImage* imageInfo);

