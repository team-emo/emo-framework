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
#ifndef EMO_STAGE_H
#define EMO_STAGE_H

#include <EGL/egl.h>
#include <GLES/gl.h>

typedef struct CameraInfo {
    bool    loaded;
    GLfloat eyeX;
    GLfloat eyeY;
    GLfloat eyeZ;
    GLfloat centerX;
    GLfloat centerY;
    GLfloat centerZ;
    GLfloat upX;   // read only
    GLfloat upY;   // read only
    GLfloat upZ;   // read only
    GLfloat zNear; // read only
    GLfloat zFar;  // read only
} CameraInfo;

namespace emo {
    class Stage {
    public:
        Stage();
        ~Stage();
        bool onLoad();
        void onDrawFrame();
        void deleteBuffer();
        void rebindBuffer();
        void setSizeAndView(int width, int height);
        void setBufferSize(int width, int height);
        void invertSize();
        bool dirty;

        int width;
        int height;
        int viewport_width;
        int viewport_height;
        GLuint vbo[2];
        float color[4];

        int bufferWidth;
        int bufferHeight;

        CameraInfo defaultPortraitCamera;
        CameraInfo defaultLandscapeCamera;
        bool usePerspective;
    protected:
        bool loaded;

        short  indices[4];
        float  positions[12];

        void updateCameraInfo();
    };
}
#endif
