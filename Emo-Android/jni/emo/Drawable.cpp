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
#include "stdlib.h"
#include "math.h"

#include "Drawable.h"
#include "Constants.h"
#include "Runtime.h"
#include "Engine.h"
#include "Database.h"
#include "Util.h"

#include <GLES/glext.h>
#include <rapidxml/rapidxml.hpp>

extern emo::Engine* engine;

namespace emo {

    AnimationFrame::AnimationFrame() {
        this->start      = 0;
        this->count      = 1;
        this->interval   = 0;
        this->loop       = 0;
        this->currentLoopCount = 0;
        this->currentCount = 0;
        this->lastOnAnimationInterval = engine->uptime;
        this->frames = NULL;
    }

    AnimationFrame::~AnimationFrame() {
        if (this->frames != NULL) delete[] this->frames;
    }

    void AnimationFrame::initializeFrames() {
        if (this->frames != NULL) delete[] this->frames;
        this->frames = new int[this->count];
        for (int i = 0; i < this->count; i++) {
            this->frames[i] = 0;
        }
    }

    void AnimationFrame::setFrame(int index, int value) {
        this->frames[index] = value;
    }

    int32_t AnimationFrame::getLastOnAnimationDelta() {
        int32_t deltaSec  = engine->uptime.time - this->lastOnAnimationInterval.time;
        int32_t deltaMsec = engine->uptime.millitm - this->lastOnAnimationInterval.millitm;

        return (deltaSec * 1000) + deltaMsec;
    }
    
    bool AnimationFrame::isFinished() {
        return (this->loop >= 0 && this->currentLoopCount > this->loop);
    }

    int AnimationFrame::getNextIndex(int frameCount, int currentIndex) {

        if (this->isFinished()) {
            return currentIndex;
        }

        this->currentCount++;

        if (currentCount >= this->count) {
            this->currentCount = 0;
            if (this->loop >= 0) {
                this->currentLoopCount++;
            }
        }

        if (this->isFinished()) {
            return currentIndex;
        } else if (this->currentCount + this->start >= frameCount) {
            currentCount = 0;
        }

        if (this->frames != NULL) {
            return this->frames[this->currentCount];
        } else {
            return this->currentCount + this->start;
        }
    }

    ImagePackInfo::ImagePackInfo() {
        this->x = 0;
        this->y = 0;
        this->width  = 1;
        this->height = 1;
    }

    ImagePackInfo::~ImagePackInfo() {

    }

    Drawable::Drawable() {
        this->hasTexture = false;
        this->hasBuffer  = false;
        this->loaded     = false;
        this->hasSheet   = false;
        this->animating  = false;
        this->frameCountLoaded = false;
        this->frameIndexChanged = false;
        this->independent = true;
        this->needTexture = false;
        this->isPackedAtlas = false;

        // color param RGBA
        this->param_color[0] = 1.0f;
        this->param_color[1] = 1.0f;
        this->param_color[2] = 1.0f;
        this->param_color[3] = 1.0f;

        // rotate angle, center x, center y, axis
        this->param_rotate[0] = 0;
        this->param_rotate[1] = 0;
        this->param_rotate[2] = 0;
        this->param_rotate[3] = AXIS_Z;

        // scale param x, y, center x, center y
        this->param_scale[0] = 1;
        this->param_scale[1] = 1;
        this->param_scale[2] = 0;
        this->param_scale[3] = 0;

        this->x = 0;
        this->y = 0;
        this->z = 0;

        this->width  = 0;
        this->height = 0;
        this->frameWidth  = 0;
        this->frameHeight = 0;

        this->frameCount  = 1;
        this->frame_index = 0;
        this->border      = 0;
        this->margin      = 0;

        this->animations = new animations_t();
        this->imagepacks = new imagepack_t();
        this->imagepacks_names = new std::vector<std::string>;

        this->useMesh = false;
        this->orthFactorX = 1.0;
        this->orthFactorY = 1.0;
        this->isScreenEntity = true;
        this->useFont = false;

        this->srcBlendFactor = GL_SRC_ALPHA;
        this->dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;

        this->isCameraObject = true;

    }

    Drawable::~Drawable() {
        this->deleteAnimations();
        this->deleteImagePacks();
        this->deleteBuffer(false);
        if (this->hasTexture) {
            this->texture->referenceCount--;
            if (this->texture->referenceCount <= 0) {
                engine->removeCachedImage(this->name);
                delete this->texture;
            }
        }
        if (this->frameCountLoaded) {
            delete[] this->frames_vbos;
        }
        delete this->animations;
        delete this->imagepacks;
        delete this->imagepacks_names;
    }

    void Drawable::load() {
        if (this->hasBuffer) return;

        this->generateBuffers();

        this->hasBuffer = true;
    }

    /*
     * generate buffer for current frame index
     */
    void Drawable::generateBuffers() {
        glGenBuffers(1, &this->frames_vbos[this->frame_index]);
    }

    void Drawable::reload() {
        if (this->hasBuffer) return;
        this->generateBuffers();

        if (this->hasTexture) {
            this->texture->genTextures();
        }

        // reload texture data when the data is freed
        if (this->hasTexture && this->texture->mustReload) {
            if (this->useFont) {
                engine->javaGlue->loadTextBitmap(this, this->texture, false);
            } else {
                loadPngFromAsset(this->name.c_str(), this->texture, false);
            }
        }

        this->hasBuffer = true;
    }

	bool Drawable::isVisible() {
		return this->param_color[3] != 0;
	}

    bool Drawable::isCurrentTexBufferLoaded() {
        return this->frames_vbos[this->frame_index] > 0;
    }

    void Drawable::deleteBuffer(bool force) {
        if (!this->hasBuffer) return;
        if (this->hasTexture && this->texture->textureId > 0) {
            if (!force && this->texture->referenceCount > 1) {
                // skip 
            } else {
                if (engine->hasDisplay()) {
                    glDeleteTextures(1, &this->texture->textureId);
                }
                this->texture->textureId = 0;
                this->texture->loaded    = false;
            }
        }

        for (int i = 0; i < this->frameCount; i++) {
            if (this->frames_vbos[i] > 0) {
                if (engine->hasDisplay()) {
                    glDeleteBuffers(1, &this->frames_vbos[i]);
                }
                this->frames_vbos[i] = 0;
            }
        }

        this->hasBuffer = false;
    }

    int Drawable::tex_coord_frame_startX() {
        if (this->isPackedAtlas) {
            return this->getImagePack(this->imagepacks_names->at(frame_index))->x;
        }
        int xcount = (int)round((this->texture->width - (this->margin * 2) + this->border) / (float)(this->frameWidth  + this->border));
        int xindex = this->frame_index % xcount;
        return ((this->border + this->frameWidth) * xindex) + this->margin;
    }

    int Drawable::tex_coord_frame_startY() {
        if (this->isPackedAtlas) {
            return this->texture->height - this->frameHeight - this->getImagePack(this->imagepacks_names->at(frame_index))->y;
        }
        int xcount = (int)round((this->texture->width - (this->margin * 2) + this->border) / (float)(this->frameWidth  + this->border));
        int ycount = (int)round((this->texture->height - (this->margin * 2) + this->border) / (float)(this->frameHeight + this->border));
        int yindex = ycount - (this->frame_index / xcount) - 1;
        return ((this->border + this->frameHeight) * yindex) + this->margin;
    }

    float Drawable::getTexelHalfX() {
        if (this->hasTexture) {
            return (1.0 / this->texture->glWidth) * 0.5;
        } else {
            return 0;
        }
    }

    float Drawable::getTexelHalfY() {
        if (this->hasTexture) {
            return (1.0 / this->texture->glHeight) * 0.5;
        } else {
            return 0;
        }
    }

    float Drawable::getTexCoordStartX() {
        if (this->hasSheet) {
            return this->tex_coord_frame_startX() / (float)this->texture->glWidth + this->getTexelHalfX();
        } else {
            return 0;
        }
    }

    float Drawable::getTexCoordEndX() {
        if (!this->hasTexture) {
            return 1 - this->getTexelHalfX();
        } else if (this->hasSheet) {
            return (float)(this->tex_coord_frame_startX() + this->frameWidth) / (float)this->texture->glWidth - this->getTexelHalfX();
        } else {
            return (float)this->texture->width / (float)this->texture->glWidth - this->getTexelHalfX();
        }
    }

    float Drawable::getTexCoordStartY() {
        if (!this->hasTexture) {
            return 1 - this->getTexelHalfY();
        } else if (this->hasSheet) {
            return (float)(this->tex_coord_frame_startY() + this->frameHeight) / (float)this->texture->glHeight - this->getTexelHalfY();
        } else {
            return (float)this->texture->height / (float)this->texture->glHeight - this->getTexelHalfY();
        }
    }

    float Drawable::getTexCoordEndY() {
        if (this->hasSheet) {
            return this->tex_coord_frame_startY() / (float)this->texture->glHeight + getTexelHalfY();
        } else {
            return 0;
        }
    }

    bool Drawable::bindVertex() {
        // Sprite#load is not called yet
        if (this->needTexture && !this->hasTexture) return false;

        clearGLErrors("bindDrawableVertex");

        this->vertex_tex_coords[0] = this->getTexCoordStartX();
        this->vertex_tex_coords[1] = this->getTexCoordStartY();

        this->vertex_tex_coords[2] = this->getTexCoordStartX();
        this->vertex_tex_coords[3] = this->getTexCoordEndY();

        this->vertex_tex_coords[4] = this->getTexCoordEndX();
        this->vertex_tex_coords[5] = this->getTexCoordEndY();

        this->vertex_tex_coords[6] = this->getTexCoordEndX();
        this->vertex_tex_coords[7] = this->getTexCoordStartY();

        // generate buffer on demand
        if (this->frames_vbos[this->frame_index] == 0) {
            this->generateBuffers();
        }

        glBindBuffer(GL_ARRAY_BUFFER, this->getCurrentBufferId());
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, this->vertex_tex_coords, GL_STATIC_DRAW);

        printGLErrors("Could not create OpenGL vertex");

        if (this->hasTexture && this->texture->hasData && !this->texture->loaded) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture   (GL_TEXTURE_2D, this->texture->textureId);

            glPixelStorei   (GL_UNPACK_ALIGNMENT, 1);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            if (this->texture->hasAlpha) {
                GLubyte* holder = (GLubyte*)malloc(sizeof(GLubyte) * this->texture->glWidth * this->texture->glHeight * 4);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->texture->glWidth, this->texture->glHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, holder);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 
                      0, 0, this->texture->width, this->texture->height, GL_RGBA, GL_UNSIGNED_BYTE, this->texture->data);
                free(holder);
            } else {
                GLubyte* holder = (GLubyte*)malloc(sizeof(GLubyte) * this->texture->glWidth * this->texture->glHeight * 3);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->texture->glWidth, this->texture->glHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, holder);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 
                       0, 0, this->texture->width, this->texture->height, GL_RGB, GL_UNSIGNED_BYTE, this->texture->data);
                free(holder);
            }
            this->texture->loaded = true;
            printGLErrors("Could not bind OpenGL textures");

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        this->loaded = true;

        return true;
    }

    void Drawable::onDrawFrame() {
        if (!this->loaded) return;
        if (!this->hasBuffer) return;

        if (this->frameIndexChanged) {
            this->frame_index = nextFrameIndex;
            frameIndexChanged = false;
        }

        engine->updateUptime();

        if (this->animating && this->currentAnimation != NULL) {
            AnimationFrame* animation = this->currentAnimation;
            int32_t delta = animation->getLastOnAnimationDelta();
            if (delta >= animation->interval) {
                this->setFrameIndex(animation->getNextIndex(this->frameCount, this->frame_index));
                animation->lastOnAnimationInterval = engine->uptime;
            }
        }

        if (this->frames_vbos[frame_index] <= 0) {
            this->bindVertex();
        }

        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity (); 

        // update colors
        glColor4f(this->param_color[0], this->param_color[1], this->param_color[2], this->param_color[3]);

        // update position
        if(engine->stage->usePerspective || this->isCameraObject == false){
            glTranslatef(this->x * this->orthFactorX, this->y * this->orthFactorY, 0);
        }else{
            glTranslatef( (this->x - engine->stage->defaultRelativeCamera.x) * this->orthFactorX,
                          (this->y - engine->stage->defaultRelativeCamera.y) * this->orthFactorY, 0);
        }

        // rotate
        glTranslatef(this->param_rotate[1], this->param_rotate[2], 0);
        if (this->param_rotate[3] == AXIS_X) {
            glRotatef(this->param_rotate[0], 1, 0, 0);
        } else if (this->param_rotate[3] == AXIS_Y) {
            glRotatef(this->param_rotate[0], 0, 1, 0);
        } else {
            glRotatef(this->param_rotate[0], 0, 0, 1);
        }
        glTranslatef(-this->param_rotate[1], -this->param_rotate[2], 0);

        // scale
        glTranslatef(this->param_scale[2], this->param_scale[3], 0);
        glScalef(this->param_scale[0], this->param_scale[1], 1);
        glTranslatef(-this->param_scale[2], -this->param_scale[3], 0);

        // update width and height
        glScalef(this->width, this->height, 1);

        // bind vertex positions
        glBindBuffer(GL_ARRAY_BUFFER, engine->stage->vbo[0]);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        // bind a texture
        if (this->hasTexture) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, this->texture->textureId);

            // bind texture coords
            glBindBuffer(GL_ARRAY_BUFFER, this->getCurrentBufferId());
            glTexCoordPointer(2, GL_FLOAT, 0, 0);
        } else {
            glDisable(GL_TEXTURE_2D);
        }

        // bind indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, engine->stage->vbo[1]);

        // draw sprite
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Drawable::setFrameCount(int count) {
        if (this->frameCountLoaded) {
            delete[] this->frames_vbos;
        }
        this->frames_vbos = new GLuint[count];
        this->frameCount = count;

        for (int i = 0; i < this->frameCount; i++) {
            this->frames_vbos[i] = 0;
        }

        this->frameCountLoaded = true;
    }

    int Drawable::getFrameCount() {
        return this->frameCount;
    }

    bool Drawable::setFrameIndex(int index) {
        if (index < 0 || this->frameCount <= index) return false;
        this->nextFrameIndex = index;
        this->frameIndexChanged = true;

        if (this->isPackedAtlas) {
            ImagePackInfo* info = this->getImagePack(this->imagepacks_names->at(index));
            this->width  = info->width;
            this->height = info->height;
            this->frameWidth  = info->width;
            this->frameHeight = info->height;
        }

        return true;
    }

    bool Drawable::forceSetFrameIndex(int index) {
        if (index < 0 || this->frameCount <= index) return false;
        this->frame_index = index;
        return true;
    }

    int Drawable::getFrameIndex() {
        return this->frame_index;
    }

    GLuint Drawable::getCurrentBufferId() {
        return this->frames_vbos[this->frame_index];
    }

    void Drawable::setTexture(Image* image) {
        this->texture = image;
    }

    Image* Drawable::getTexture() {
        return this->texture;
    }

    float Drawable::getScaledWidth() {
        return this->width * this->param_scale[0];
    }

    float Drawable::getScaledHeight() {
        return this->height * this->param_scale[1];
    }

    void Drawable::setChild(Drawable* child) {
        this->child = child;
    }

    Drawable* Drawable::getChild() {
        return this->child;
    }

    void Drawable::addImagePack(ImagePackInfo* info) {
        this->deleteImagePack(info->name);
        this->imagepacks->insert(std::make_pair(info->name, info)); 
        this->imagepacks_names->push_back(info->name);
    }

    ImagePackInfo* Drawable::getImagePack(std::string name) {
        imagepack_t::iterator iter = this->imagepacks->find(name);
        if (iter != this->imagepacks->end()) {
            return iter->second;
        }
        return NULL;
    }

    bool Drawable::selectFrame(std::string name) {
        ImagePackInfo* info = this->getImagePack(name);
        if (info == NULL) return false;
        return this->setFrameIndex(info->index);
    }

    bool Drawable::deleteImagePack(std::string name) {
        imagepack_t::iterator iter = this->imagepacks->find(name);
        if (iter != this->imagepacks->end()) {
            ImagePackInfo* info = iter->second;
            if (this->imagepacks->erase(iter->first)){
                delete info;
            }
            return true;
        }
        return false;
    }

    void Drawable::deleteImagePacks() {
        imagepack_t::iterator iter;
        for(iter = this->imagepacks->begin(); iter != this->imagepacks->end(); iter++) {
            ImagePackInfo* info = iter->second;
            delete info;
        }
        this->imagepacks->clear();
        this->imagepacks_names->clear();
    }

    void Drawable::addAnimation(AnimationFrame* animation) {
        this->deleteAnimation(animation->name);
        this->animations->insert(std::make_pair(animation->name, animation)); 
    }

    bool Drawable::setAnimation(std::string name) {
        if (this->getAnimation(name) == NULL) {
            return false;
        } else {
            this->animationName = name;
        }
        return true;
    }

    AnimationFrame* Drawable::getAnimation(std::string name) {
        animations_t::iterator iter = this->animations->find(name);
        if (iter != this->animations->end()) {
            return iter->second;
        }
        return NULL;
    }

    bool Drawable::deleteAnimation(std::string name) {
        animations_t::iterator iter = this->animations->find(name);
        if (iter != this->animations->end()) {
            AnimationFrame* animation = iter->second;
            if (this->animations->erase(iter->first)){
                delete animation;
            }
            return true;
        }
        return false;
    }

    void Drawable::deleteAnimations() {
        animations_t::iterator iter;
        for(iter = this->animations->begin(); iter != this->animations->end(); iter++) {
            AnimationFrame* animation = iter->second;
            delete animation;
        }
        this->animations->clear();
    }

    bool Drawable::enableAnimation(bool enable) {
        this->animating = enable;

        this->currentAnimation = NULL;

        if (enable) {
            if (this->animationName.empty()) return false;
            AnimationFrame* animation = this->getAnimation(this->animationName);
            if (animation == NULL) {
                return false;
            } else {
                this->currentAnimation = animation;
                engine->updateUptime();
                animation->lastOnAnimationInterval = engine->uptime;
                this->setFrameIndex(animation->start);
            }
        }
        return true;
    }

    bool Drawable::isAnimationFinished() {
        if (this->animating && this->currentAnimation != NULL) {
            return this->currentAnimation->isFinished();
        }
        return true;
    }

    bool Drawable::loadPackedAtlasXml(int initialFrameIndex) {
        // check if the length is shorter than the length of ".xml"
        if (this->name.length() <= 4) return false;

        std::string data = loadContentFromAsset(this->name);
        unsigned int pos = this->name.find_last_of("/");
        
        std::string base_dir = "";
        if (pos != std::string::npos) base_dir = this->name.substr(0, pos + 1);

        rapidxml::xml_document<char> doc;
        doc.parse<0>((char*)data.c_str());

        if (doc.first_node() == 0) {
            return false;
        }

        int itemCount = 0;
        ImagePackInfo* selectedItem = NULL;

        for (rapidxml::xml_node<> *node = doc.first_node(); node; node = node->next_sibling()) {
            if (strcmp(node->name(), "Imageset") != 0 && strcmp(node->name(), "TextureAtlas") != 0) continue;
            for (rapidxml::xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
                if (strcmp(attr->name(), "Imagefile") == 0 || strcmp(attr->name(), "imagePath") == 0) {
                    this->name = base_dir + attr->value();
                }
            }
            for (rapidxml::xml_node<> *child  = node->first_node(); child; child = child->next_sibling()) {
                if (strcmp(child->name(), "Image") != 0 && strcmp(child->name(), "SubTexture") != 0) continue;

                ImagePackInfo* info = new ImagePackInfo();
                for (rapidxml::xml_attribute<> *attr = child->first_attribute(); attr; attr = attr->next_attribute()) {
                    if (strcmp(attr->name(), "name") == 0 || strcmp(attr->name(), "Name") == 0) {
                        info->name = attr->value();
                    } else if (strcmp(attr->name(), "x") == 0 || strcmp(attr->name(), "XPos") == 0) {
                        info->x = atoi(attr->value());
                    } else if (strcmp(attr->name(), "y") == 0 || strcmp(attr->name(), "YPos") == 0) {
                        info->y = atoi(attr->value());
                    } else if (strcmp(attr->name(), "width") == 0 || strcmp(attr->name(), "Width") == 0) {
                        info->width = atoi(attr->value());
                    } else if (strcmp(attr->name(), "height") == 0 || strcmp(attr->name(), "Height") == 0) {
                        info->height = atoi(attr->value());
                    }
                }
                if (!info->name.empty()) {
                    if (itemCount == initialFrameIndex) selectedItem = info;
                    info->index = itemCount;
                    this->addImagePack(info);
                    itemCount++;
                }
            }
        }

        if (selectedItem == NULL) return false;

        this->width       = selectedItem->width;
        this->height      = selectedItem->height;
        this->frameWidth  = selectedItem->width;
        this->frameHeight = selectedItem->height;

        this->setFrameCount(itemCount);
        this->margin = 0;
        this->border = 0;

        return true;
    }

    MapDrawable::MapDrawable(Drawable* drawable) : Drawable() {
        this->drawable = drawable;
        this->tiles = new std::vector<std::vector<int>*>;
        this->setChild(drawable);
        this->meshLoaded = false;
        this->meshIndiceCount = 0;
    }

    MapDrawable::~MapDrawable() {
        for (unsigned int i = 0; i < this->tiles->size(); i++) {
            delete this->tiles->at(i);
        }
        delete this->tiles;

        this->unbindMeshVertex();
    }

    void MapDrawable::deleteBuffer(bool force) {
        Drawable::deleteBuffer(force);
        this->unbindMeshVertex();
    }

    void MapDrawable::unbindMeshVertex() {
        if (this->useMesh && this->meshLoaded) {
            free(this->meshIndices);
            free(this->meshPositions);
            free(this->meshTexCoords);
        
            glDeleteBuffers(3, this->mesh_vbos);
        
            this->meshLoaded = false;
        }
    }

    void MapDrawable::addRow(int rowdata[], int count) {
        std::vector<int>* row = new std::vector<int>;
        for (int i = 0; i < count; i++) {
            row->push_back(rowdata[i]);
        }
        this->tiles->push_back(row);

        this->columns = count;
        this->rows    = this->tiles->size();

       this->width  = this->drawable->width  * this->columns;
       this->height = this->drawable->height * this->rows;
    }

    bool MapDrawable::setTileAt(int row, int column, int value) {
        if (row    >= (int)this->tiles->size())          return false;
        if (column >= (int)this->tiles->at(row)->size()) return false;

        this->tiles->at(row)->at(column) = value;
        return true;
    }

    int MapDrawable::getTileAt(int row, int column) {
        if (row    >= (int)this->tiles->size())          return -1;
        if (column >= (int)this->tiles->at(row)->size()) return -1;

        return this->tiles->at(row)->at(column);
    }

    std::vector<int> MapDrawable::getTileIndexAtCoord(float x, float y) {
        std::vector<int> index;

        int col = (int)floor((x - this->x) / (double)this->drawable->getScaledWidth());
        int row = (int)floor((y - this->y) / (double)this->drawable->getScaledHeight());

        index.push_back(row);
        index.push_back(col);

        return index;
    }

    std::vector<float> MapDrawable::getTilePositionAtCoord(float x, float y) {
        std::vector<float> index;

        std::vector<int> tile = this->getTileIndexAtCoord(x, y);
        index.push_back((tile.at(1) * this->drawable->getScaledWidth())  + this->x);
        index.push_back((tile.at(0) * this->drawable->getScaledHeight()) + this->y);
        
        return index;
    }

    bool MapDrawable::clearTiles() {
        this->tiles->clear();
        return true;
    }

    void MapDrawable::load() {
        Drawable::load();
        this->drawable->load();
    }

    void MapDrawable::reload() {
        Drawable::reload();
        this->drawable->reload();
    }

    Drawable* MapDrawable::getChild() {
        return Drawable::getChild();
    }

    void MapDrawable::setChild(Drawable* child) {
        Drawable::setChild(child);
    }

    bool MapDrawable::bindVertex() {
        if (this->useMesh && !this->meshLoaded) {
            this->unbindMeshVertex();
        
            glGenBuffers(3, this->mesh_vbos);
        
            this->createMeshIndiceBuffer();
            this->createMeshPositionBuffer();
            this->createMeshTextureBuffer();
            
            meshLoaded = true;

            this->loaded = this->drawable->bindVertex();
        } else if (!this->useMesh) {
            this->loaded = this->drawable->bindVertex();
        }
        return this->loaded;
    }

    void MapDrawable::createMeshPositionBuffer() {
        int vertexCount = this->rows * this->columns * POINTS_3D_SIZE * POINTS_RECTANGLE;
    
        this->meshPositions = (float *)malloc(sizeof(float) * vertexCount);
    
        int childWidth  = child->getScaledWidth();
        int childHeight = child->getScaledHeight();

        int index = 0;
        for (int row = 0; row < rows; row++) {
            for (int column = 0; column < columns; column++) {
                int pX = childWidth  * column;
                int pY = childHeight * row;
            
                this->meshPositions[index++] = pX;
                this->meshPositions[index++] = pY;
                this->meshPositions[index++] = 0;
            
                this->meshPositions[index++] = pX;
                this->meshPositions[index++] = pY + childHeight;
                this->meshPositions[index++] = 0;
            
                this->meshPositions[index++] = pX + childWidth;
                this->meshPositions[index++] = pY;
                this->meshPositions[index++] = 0;
            
                this->meshPositions[index++] = pX + childWidth;
                this->meshPositions[index++] = pY + childHeight;
                this->meshPositions[index++] = 0;
            }
        }
    
        glBindBuffer(GL_ARRAY_BUFFER, this->mesh_vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount, this->meshPositions, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
        printGLErrors("Could not create position buffer");
    }

    int MapDrawable::getMeshIndiceCount() {
        return this->rows * this->columns * POINTS_2D_SIZE * POINTS_TRIANGLE;
    }

    void MapDrawable::createMeshIndiceBuffer() {
        this->meshIndiceCount = this->getMeshIndiceCount();
    
        this->meshIndices = (short *)malloc(sizeof(short) * this->meshIndiceCount);
    
        int index = 0;
        int trIndex = 0;
        for (int row = 0; row < rows; row++) {
            for (int column = 0; column < columns; column++) {
                this->meshIndices[index++] = (short)(trIndex + 0);
                this->meshIndices[index++] = (short)(trIndex + 1);
                this->meshIndices[index++] = (short)(trIndex + 2);
            
                this->meshIndices[index++] = (short)(trIndex + 2);
                this->meshIndices[index++] = (short)(trIndex + 1);
                this->meshIndices[index++] = (short)(trIndex + 3);
            
                trIndex += 4;
            }
        }
    
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mesh_vbos[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * this->meshIndiceCount, this->meshIndices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
        printGLErrors("Could not create indice buffer");
    }

    void MapDrawable::createMeshTextureBuffer() {
        int coordCount = this->rows * this->columns * POINTS_2D_SIZE * POINTS_RECTANGLE;
    
        this->meshTexCoords = (float *)malloc(sizeof(float) * coordCount);
    
        int index = 0;
        for (int row = 0; row < rows; row++) {
            for (int column = 0; column < columns; column++) {
                int frameIndex = this->getTileAt(row, column);
                if (child->forceSetFrameIndex(frameIndex)) {
                    this->meshTexCoords[index++] = child->getTexCoordStartX();
                    this->meshTexCoords[index++] = child->getTexCoordStartY();
            
                    this->meshTexCoords[index++] = child->getTexCoordStartX();
                    this->meshTexCoords[index++] = child->getTexCoordEndY();
            
                    this->meshTexCoords[index++] = child->getTexCoordEndX();
                    this->meshTexCoords[index++] = child->getTexCoordStartY();
            
                    this->meshTexCoords[index++] = child->getTexCoordEndX();
                    this->meshTexCoords[index++] = child->getTexCoordEndY();
                } else {
                    // no texture for this cell
                    this->meshTexCoords[index++] = 0;
                    this->meshTexCoords[index++] = 0;
                
                    this->meshTexCoords[index++] = 0;
                    this->meshTexCoords[index++] = 0;
                
                    this->meshTexCoords[index++] = 0;
                    this->meshTexCoords[index++] = 0;
                
                    this->meshTexCoords[index++] = 0;
                    this->meshTexCoords[index++] = 0;
                }
            }
        }
    
        glEnable(GL_TEXTURE_2D);
        glBindBuffer(GL_ARRAY_BUFFER, this->mesh_vbos[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * coordCount, this->meshTexCoords, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
        printGLErrors("Could not create texture buffer");
    }

    bool MapDrawable::isInRange() {
        if (this->x > 0 && this->x  > engine->stage->width)  return false;
        if (this->y > 0 && this->y  > engine->stage->height) return false;
        if (this->x < 0 && -this->x > this->drawable->getScaledWidth()  * this->columns) return false;
        if (this->y < 0 && -this->y > this->drawable->getScaledHeight() * this->rows) return false;
    
        return true;
    }

    void MapDrawable::onDrawFrame() {

        if (!this->isInRange()) return;

        if (this->useMesh) {
            glMatrixMode (GL_MODELVIEW);
            glLoadIdentity (); 
        
            // update colors
            glColor4f(param_color[0], param_color[1], param_color[2], param_color[3]);
        
            // update position
            glTranslatef(x, y, 0);
        
            // bind vertex positions
            glBindBuffer(GL_ARRAY_BUFFER, mesh_vbos[0]);
            glVertexPointer(3, GL_FLOAT, 0, 0);
        
            // bind a texture
            if (child->hasTexture) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, child->getTexture()->textureId);
            
                // bind texture coords
                glBindBuffer(GL_ARRAY_BUFFER, mesh_vbos[2]);
                glTexCoordPointer(2, GL_FLOAT, 0, 0);
            } else {
                glDisable(GL_TEXTURE_2D);
            }
        
            // bind indices
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_vbos[1]);
        
            // draw sprite
            glDrawElements(GL_TRIANGLES, meshIndiceCount, GL_UNSIGNED_SHORT, 0);
        
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        
            return;
        }

        int invertX = -this->x;
        int invertY = -this->y;

        int columnCount = (int)ceil(this->getScaledWidth()  / (double)this->drawable->getScaledWidth());
        int rowCount    = (int)ceil(this->getScaledHeight() / (double)this->drawable->getScaledHeight());
        
        int firstColumn = max(0, min(this->columns, (invertX / this->drawable->getScaledWidth())));
        int lastColumn  = min(firstColumn + columnCount + 1, this->columns);
        
        int firstRow = max(0, min(this->rows, (invertY / this->drawable->getScaledHeight())));
        int lastRow  = min(firstRow + rowCount + 1, this->rows);

        for (int i = firstRow; i < lastRow; i++) {
            for (int j = firstColumn; j < lastColumn; j++) {
               if (((int)tiles->size()) <= i || ((int)tiles->at(i)->size()) <= j) break;
                this->drawable->x = j * this->drawable->getScaledWidth()  - invertX;
                this->drawable->y = i * this->drawable->getScaledHeight() - invertY;
                if (tiles->at(i)->at(j) < 0) continue;

                this->drawable->setFrameIndex(tiles->at(i)->at(j));
                this->drawable->onDrawFrame();
            }
        }
    }

    LineDrawable::LineDrawable() {
        this->x2 = 0;
        this->y2 = 0;

        this->param_color[0] = 0;
        this->param_color[1] = 1;
        this->param_color[2] = 0;
    }

    LineDrawable::~LineDrawable() {

    }

    bool LineDrawable::bindVertex() {
        this->loaded = true;
        return true;
    }

    void LineDrawable::setFrameCount(int count) {
        Drawable::setFrameCount(count);
    }

    void LineDrawable::onDrawFrame() {
        if (!this->loaded) return;
        this->vertex_tex_coords[0] = this->x;
        this->vertex_tex_coords[1] = this->y;
        this->vertex_tex_coords[2] = this->x2;
        this->vertex_tex_coords[3] = this->y2;

        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity (); 

        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        glLineWidth(this->width);
    
        // update colors
        glColor4f(this->param_color[0], this->param_color[1], this->param_color[2], this->param_color[3]);
        glVertexPointer(2, GL_FLOAT, 0, this->vertex_tex_coords);
        glDrawArrays(GL_LINES, 0, 2);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    SnapshotDrawable::SnapshotDrawable() {
        // snapshot drawable should be the first drawable
        this->z = -1;
        this->isScreenEntity = false;
    }

    SnapshotDrawable::~SnapshotDrawable() {

    }

   /*
    * create texture and bind OpenGL vertex
    * width and height should be set before calling bindVertex.
    */
    bool SnapshotDrawable::bindVertex() {
        clearGLErrors("SnapshotDrawable:bindVertex");
   
        if (!this->hasTexture) {
            Image* imageInfo = new emo::Image();
    
            imageInfo->width  = this->width;
            imageInfo->height = this->height;
    
            imageInfo->glWidth  = nextPowerOfTwo(imageInfo->width);
            imageInfo->glHeight = nextPowerOfTwo(imageInfo->height);
            imageInfo->loaded = false;

            imageInfo->referenceCount++;
    
            // assign OpenGL texture id
            imageInfo->genTextures();
    
            this->texture = imageInfo;
            this->hasTexture = true;
        }

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, this->texture->textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nextPowerOfTwo(this->width), nextPowerOfTwo(this->height), 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_TEXTURE_2D, this->texture->textureId, 0);

        GLenum status = glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES);
        if (status != GL_FRAMEBUFFER_COMPLETE_OES) {
            char str[256];
            sprintf(str, "Failed to create offscreen: status=0x%04x.", status);
            LOGE(str);
            engine->disableOffscreen();
        }

        this->texture->loaded = true;

        this->vertex_tex_coords[0] = this->getTexCoordStartX();
        this->vertex_tex_coords[1] = this->getTexCoordStartY();

        this->vertex_tex_coords[2] = this->getTexCoordStartX();
        this->vertex_tex_coords[3] = this->getTexCoordEndY();

        this->vertex_tex_coords[4] = this->getTexCoordEndX();
        this->vertex_tex_coords[5] = this->getTexCoordEndY();

        this->vertex_tex_coords[6] = this->getTexCoordEndX();
        this->vertex_tex_coords[7] = this->getTexCoordStartY();

        // generate buffer on demand
        if (this->frames_vbos[this->frame_index] == 0) {
            this->generateBuffers();
        }

        glBindBuffer(GL_ARRAY_BUFFER, this->getCurrentBufferId());
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, this->vertex_tex_coords, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        printGLErrors("SnapshotDrawable::bindVertex");

        loaded = true;

        return true;
    }

    void SnapshotDrawable::onDrawFrame() {
        // if the snapshot ends, use default onDrawFrame
        if (!engine->useOffscreen) {
            orthFactorX = this->width  / (float)engine->stage->width;
            orthFactorY = this->height / (float)engine->stage->height;
            Drawable::onDrawFrame();
            return;
        }
        // vewport should be reset everytime on drawing
        glViewport(0, 0, this->width, this->height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrthof(0, this->width, this->height, 0, -1, 1);

        orthFactorX = 1.0;
        orthFactorY = 1.0;

        glClearColor(engine->stage->color[0], engine->stage->color[1],
                     engine->stage->color[2], engine->stage->color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        Drawable::onDrawFrame();
        engine->stage->dirty = true;
    }

    FontDrawable::FontDrawable() {
        this->isBold   = false;
        this->isItalic = false;
    }

    FontDrawable::~FontDrawable() {

    }

    LiquidDrawable::LiquidDrawable() {
        this->textureCoords = NULL;
        this->segmentCoords = NULL;
        this->segmentCount  = 0;
    }

    LiquidDrawable::~LiquidDrawable() {
        if (this->textureCoords != NULL) free(this->textureCoords);
        if (this->segmentCoords != NULL) free(this->segmentCoords);
    
        this->textureCoords = NULL;
        this->segmentCoords = NULL;
    }

    bool LiquidDrawable::bindVertex() {
        // set default segment count if empty
        int count = this->segmentCount <= 0 ? 18 : this->segmentCount;

        if (!this->updateSegmentCount(count)) return false;
        if (!Drawable::bindVertex()) return false;

        return true;
    }

    void LiquidDrawable::onDrawFrame() {
        if (!this->loaded) return;
        if (!this->hasBuffer) return;
        if (this->segmentCount <= 0) return;

        engine->updateUptime();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // update colors
        glColor4f(param_color[0], param_color[1], param_color[2], param_color[3]);

        if (this->hasTexture) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, this->texture->textureId);

            glTexCoordPointer(2, GL_FLOAT, 0, this->textureCoords);
        } else {
            glDisable(GL_TEXTURE_2D);
        }

        glVertexPointer(2, GL_FLOAT, 0,  this->segmentCoords);
        glDrawArrays(GL_TRIANGLE_FAN, 0, this->segmentCount);
    }

    bool LiquidDrawable::updateTextureCoords(int index, float tx, float ty) {
        if (index >= this->segmentCount) return false;

        int realIndex = index * 2;
        this->textureCoords[realIndex]     = tx;
        this->textureCoords[realIndex + 1] = ty;

        return true;
    }

    bool LiquidDrawable::updateSegmentCoords(int index, float sx, float sy) {
        if (index >= this->segmentCount) return false;
    
        int realIndex = index * 2;
        this->segmentCoords[realIndex]     = sx; 
        this->segmentCoords[realIndex + 1] = sy;

        return true;
    }

    bool LiquidDrawable::updateSegmentCount(GLsizei count) {

        if (count <= 0) return false;
        if (count == segmentCount) return true;
    
        this->segmentCount = count;
    
        if (this->textureCoords != NULL) free(this->textureCoords);
        if (this->segmentCoords != NULL) free(this->segmentCoords);
    
        this->textureCoords = (float*)malloc(sizeof(float) * this->segmentCount * 2);
        this->segmentCoords = (float*)malloc(sizeof(float) * this->segmentCount * 2);
    
        for (int i = 0; i < this->segmentCount * 2; i++) {
            this->textureCoords[i] = 0.0f;
            this->segmentCoords[i] = 0.0f;
        }

        return true;
    }

    PointDrawable::PointDrawable() {
        this->pointCoords = NULL;
        this->pointCount  = 0;

        this->updatePointCount(1);
    }

    PointDrawable::~PointDrawable() {
        if (this->pointCoords != NULL) free(this->pointCoords);
        this->pointCoords = NULL;
    }

    bool PointDrawable::bindVertex() {
        if (!this->updatePointCount(pointCount)) return false;
        if (!Drawable::bindVertex()) return false;

        return true;
    }

    void PointDrawable::onDrawFrame() {
        if (!this->loaded) return;
        if (!this->hasBuffer) return;
        if (this->pointCount <= 0) return;

        engine->updateUptime();

        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity ();
	
        // update colors
        glColor4f(param_color[0], param_color[1], param_color[2], param_color[3]);
    
        if (this->hasTexture) {
            glEnable(GL_POINT_SPRITE_OES);
            glEnable(GL_TEXTURE_2D);
            glTexEnvi(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D, texture->textureId);
        } else {
            glDisable(GL_TEXTURE_2D);
        }
    
        glPointSize(width);
        glVertexPointer(2, GL_FLOAT, 0, pointCoords);
        glTexCoordPointer(2, GL_FLOAT, 0, vertex_tex_coords);
        glDrawArrays(GL_POINTS, 0, pointCount);
    
        if (this->hasTexture) {
            glDisable(GL_POINT_SPRITE_OES);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

    }

    bool PointDrawable::updatePointCount(GLsizei count) {
        if (count <= 0) return false;
        if (count == pointCount) return true;

        this->pointCount = count;

        if (this->pointCoords != NULL) free(this->pointCoords);

        this->pointCoords = (float*)malloc(sizeof(float) * this->pointCount * 2);

        for (int i = 0; i < this->pointCount * 2; i++) {
            this->pointCoords[i] = 0.0f;
        }

        return true;
    }

    bool PointDrawable::updatePointCoords(int index, float px, float py) {
        if (index >= this->pointCount) return false;

        int realIndex = index * 2;
        this->pointCoords[realIndex]     = px;
        this->pointCoords[realIndex + 1] = py;

        return true;
    }

}
