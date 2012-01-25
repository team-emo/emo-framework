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
#import "EmoStage.h"
#import "EmoEngine.h"
#import "EmoEngine_glue.h"

extern EmoEngine* engine;

@implementation EmoStage
@synthesize width, height, viewport_width, viewport_height;
@synthesize dirty;
@synthesize bufferWidth, bufferHeight;

- (id)init {
    self = [super init];
    if (self != nil) {
		loaded = FALSE;
		dirty = TRUE;
		
		color[0] = 0;
		color[1] = 0;
		color[2] = 0;
		color[3] = 1;
        
        // experimental
        usePerspective = FALSE;
    }
    return self;
}
-(BOOL)loadBuffer {
    loaded = FALSE;
	dirty = TRUE;
	
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;
	
    positions[0] = 0;
    positions[1] = 0;
    positions[2] = 0;
	
    positions[3] = 0;
    positions[4] = 1;
    positions[5] = 0;
	
    positions[6] = 1;
    positions[7] = 1;
    positions[8] = 0;
	
    positions[9]  = 1;
    positions[10] = 0;
    positions[11] = 0;
	
    clearGLErrors("EmoStage:loadBuffer");
	
    glGenBuffers(2, vbo);
	
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, positions, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * 4, indices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	loaded = printGLErrors("Could not create OpenGL buffers");
	
	return loaded;
}

-(GLuint)getPositionPointer {
	return vbo[0];
}

-(GLuint)getIndicePointer {
	return vbo[1];
}

- (void)updateCameraInfo {
    float zfar = fmaxf(width, height) * 4;
    
    defaultPortraitCamera.eyeX = width  * 0.5;
    defaultPortraitCamera.eyeY = height * 0.5;
    defaultPortraitCamera.eyeZ = height * 0.5;
    defaultPortraitCamera.centerX = width * 0.5;
    defaultPortraitCamera.centerY = height * 0.5;
    defaultPortraitCamera.centerZ = 0;
    defaultPortraitCamera.upX = 0;
    defaultPortraitCamera.upY = 1;
    defaultPortraitCamera.upZ = 0;
    defaultPortraitCamera.zNear = 1;
    defaultPortraitCamera.zFar  = zfar;
    defaultPortraitCamera.loaded = TRUE;
    
    defaultLandscapeCamera.eyeX = width  * 0.5;
    defaultLandscapeCamera.eyeY = height * 0.5;
    defaultLandscapeCamera.eyeZ = width  * 0.5;
    defaultLandscapeCamera.centerX = width  * 0.5;
    defaultLandscapeCamera.centerY = height * 0.5;
    defaultLandscapeCamera.centerZ = 0;
    defaultLandscapeCamera.upX = 1;
    defaultLandscapeCamera.upY = 0;
    defaultLandscapeCamera.upZ = 0;
    defaultLandscapeCamera.zNear = 1;
    defaultLandscapeCamera.zFar  = zfar;
    defaultLandscapeCamera.loaded = TRUE;
}

-(BOOL)onDrawFrame:(NSTimeInterval)dt {
    if (dirty) {
        if (usePerspective) {
            [self updateCameraInfo];
            CameraInfo camera = defaultPortraitCamera;
            
            if (engine.currentOrientation == OPT_ORIENTATION_LANDSCAPE_LEFT) {
                float ratio = viewport_height / (float)viewport_width;
                glViewport(0, 0, viewport_height, viewport_width); 
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                camera = defaultLandscapeCamera;
                glFrustumf(ratio, -ratio, -1, 1, camera.zNear, camera.zFar);
            } else if (engine.currentOrientation == OPT_ORIENTATION_LANDSCAPE_RIGHT) {
                float ratio = viewport_height / (float)viewport_width;
                glViewport(0, 0, viewport_height, viewport_width); 
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                camera = defaultLandscapeCamera;
                glFrustumf(-ratio, ratio, 1, -1, camera.zNear, camera.zFar);
            } else {
                float ratio = viewport_width / (float)viewport_height;
                glViewport(0, 0, viewport_width, viewport_height); 
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glFrustumf(-ratio, ratio, 1, -1, camera.zNear, camera.zFar);
            }
            
            gluLookAt(camera.eyeX,    camera.eyeY,    camera.eyeZ, 
                      camera.centerX, camera.centerY, camera.centerZ,
                      camera.upX,     camera.upY,     camera.upZ);
        } else {
            glViewport(0, 0, viewport_width, viewport_height); 
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrthof(0, width, height, 0, -1, 1);
        }
        dirty = FALSE;
    }
	
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
	return TRUE;
}

-(void)unloadBuffer{
    if (!loaded) return;
	
    glDeleteBuffers(2, vbo);
    loaded = FALSE;
}
-(void)setSize:(GLint)_width height:(GLint)_height {
	width  = _width;
	height = _height;
	viewport_width  = _width;
	viewport_height = _height;
	
	dirty = TRUE;
}

-(void)setBufferSize:(GLint)_width height:(GLint)_height {
    bufferWidth  = _width;
    bufferHeight = _height;
}

-(void)setColor:(NSInteger)index withValue:(float)value {
	color[index] = value;
}
-(float)getColor:(NSInteger)index {
	return color[index];
}
-(void)invertSize {
	[self setSize:height height:width];
    [self setBufferSize:bufferHeight height:bufferWidth];
}
@end
