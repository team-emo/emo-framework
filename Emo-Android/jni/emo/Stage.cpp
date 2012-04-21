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
#include "Engine.h"
#include "Runtime.h"
#include "Stage.h"

extern emo::Engine* engine;

namespace emo {
    Stage::Stage() {
        this->loaded    = false;
        this->dirty = true;

        this->indices[0] = 0;
        this->indices[1] = 1;
        this->indices[2] = 2;
        this->indices[3] = 3;

        this->positions[0] = 0;
        this->positions[1] = 0;
        this->positions[2] = 0;

        this->positions[3] = 0;
        this->positions[4] = 1;
        this->positions[5] = 0;

        this->positions[6] = 1;
        this->positions[7] = 1;
        this->positions[8] = 0;

        this->positions[9]  = 1;
        this->positions[10] = 0;
        this->positions[11] = 0;

        this->vbo[0] = 0;
        this->vbo[1] = 0;

        this->color[0] = 0;
        this->color[1] = 0;
        this->color[2] = 0;
        this->color[3] = 1;

        this->width  = 0;
        this->height = 0;
        this->viewport_width  = 0;
        this->viewport_height = 0;

        this->bufferWidth  = 0;
        this->bufferHeight = 0;

        // experimental
        this->usePerspective = false;
        this->resetRelativeCameraInfo();
    }

    Stage::~Stage() {

    }

    bool Stage::onLoad() {
        if (!engine->hasDisplay()) return false;

        clearGLErrors("Stage::onLoad");

        glGenBuffers(2, this->vbo);

        glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 , this->positions, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbo[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * 4, this->indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        printGLErrors("Could not create OpenGL buffers");

        this->loaded = true;

        return true;

    }

    void Stage::updatePerspectiveCameraInfo() {
        float zfar = max(width, height) * 4;
    
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
        defaultPortraitCamera.loaded = true;
    
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
        defaultLandscapeCamera.loaded = true;
    }

    void Stage::resetRelativeCameraInfo(){
        defaultRelativeCamera.x = 0;
        defaultRelativeCamera.y = 0;
    }

    void Stage::moveRelativeCamera(float x, float y){
        defaultRelativeCamera.x = x;
        defaultRelativeCamera.y = y;
    }

    void Stage::onDrawFrame() {
        if (!engine->hasDisplay()) return;
        if (this->dirty){
            if (this->usePerspective) {
            	this->updatePerspectiveCameraInfo();
                PerspectiveCameraInfo camera = defaultPortraitCamera;
            
                float ratio = viewport_width / (float)viewport_height;
                glViewport(0, 0, viewport_width, viewport_height); 
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glFrustumf(-ratio, ratio, 1, -1, camera.zNear, camera.zFar);
            
                gluLookAt(camera.eyeX,    camera.eyeY,    camera.eyeZ, 
                      camera.centerX, camera.centerY, camera.centerZ,
                      camera.upX,     camera.upY,     camera.upZ);
            } else {
                glViewport(0, 0, this->viewport_width, this->viewport_height); 
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrthof(0, this->width, this->height, 0, -1, 1);
            }
            this->dirty = false;
        }

        glClearColor(this->color[0], this->color[1], this->color[2], this->color[3]);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Stage::deleteBuffer() {
        if (!this->loaded) return;

        if (engine->hasDisplay()) {
            glDeleteBuffers(2, this->vbo);
        }
        this->loaded = false;

        this->vbo[0] = 0;
        this->vbo[1] = 0;
    }

    void Stage::rebindBuffer() {
        if (!engine->hasDisplay()) return;
        glGenBuffers(2, this->vbo);

        glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 , this->positions, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbo[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * 4, this->indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        printGLErrors("Could not create OpenGL buffers");

        this->dirty = true;
        this->loaded = true;
    }

    void Stage::setSizeAndView(int w, int h) {
    	this->width  = w;
    	this->height = h;
    	this->viewport_width  = w;
    	this->viewport_height = h;
    }

    void Stage::setBufferSize(int w, int h) {
        this->bufferWidth  = w;
        this->bufferHeight = h;
    }

    void Stage::invertSize() {
    	int w = this->width;
    	this->width  = this->height;
    	this->height = w;

    	this->viewport_width  = this->width;
    	this->viewport_height = this->height;

        this->setBufferSize(this->bufferHeight, this->bufferWidth);

    	this->dirty = true;
    }
}
