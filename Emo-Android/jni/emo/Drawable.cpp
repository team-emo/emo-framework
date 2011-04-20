#include "stdlib.h"
#include "math.h"

#include "Constants.h"
#include "Runtime.h"
#include "Engine.h"

extern emo::Engine* engine;

namespace emo {

    AnimationFrame::AnimationFrame() {
        this->start = 0;
        this->frameCount = 1;
        this->interval   = 0;
        this->loop       = 0;
    }

    AnimationFrame::~AnimationFrame() {
    }

    Drawable::Drawable() {
        this->hasTexture = false;
        this->hasBuffer  = false;
        this->loaded     = false;
        this->hasSheet   = false;
        this->animating  = false;
        this->frameCountLoaded = false;

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

        this->frameCount  = 1;
        this->frame_index = 0;
        this->border      = 0;
        this->margin      = 0;

        this->animations = new animations_t();
    }

    Drawable::~Drawable() {
        this->deleteAnimations();
        this->deleteBuffer();
        if (this->hasTexture) {
            delete this->texture;
        }
        if (this->frameCountLoaded) {
            delete[] this->frames_vbos;
        }
        delete this->animations;
    }

    void Drawable::load() {
        if (this->hasBuffer) return;

        // generate buffer for current frame index
        glGenBuffers(1, &this->frames_vbos[this->frame_index]);

        if (this->hasTexture) {
            this->texture->genTextures();
        }
        this->hasBuffer = true;
    }

    bool Drawable::isCurrentTexBufferLoaded() {
        return this->frames_vbos[this->frame_index] > 0;
    }

    void Drawable::deleteBuffer() {
        if (!this->hasBuffer) return;
        if (this->hasTexture) {
            glDeleteTextures(1, &this->texture->textureId);
        }

        for (int i = 0; i < this->frameCount; i++) {
            if (this->frames_vbos[i] > 0) {
                glDeleteBuffers(1, &this->frames_vbos[i]);
                this->frames_vbos[i] = 0;
            }
        }

        this->hasBuffer = false;

        if (this->hasTexture) {
            this->texture->textureId = 0;
            this->texture->loaded    = false;
        }
    }

    int Drawable::tex_coord_frame_startX() {
        int xindex = this->frame_index % (int)round((this->texture->width - (this->margin * 2) + this->border) / (float)(this->width  + this->border));
        return ((this->border + this->width) * xindex) + this->margin;
    }

    int Drawable::tex_coord_frame_startY() {
        int ycount = (int)round((this->texture->height - (this->margin * 2) + this->border) / (float)(this->height + this->border));
        int yindex = ycount - 1 - ((this->frame_index + 1) / (int)round((this->texture->width - (this->margin * 2) + this->border) / (float)(this->width  + this->border)));
        return ((this->border + this->height) * yindex) + this->margin;
    }

    float Drawable::getTexCoordStartX() {
        if (this->hasSheet) {
            return this->tex_coord_frame_startX() / (float)this->texture->glWidth;
        } else {
            return 0;
        }
    }

    float Drawable::getTexCoordEndX() {
        if (!this->hasTexture) {
            return 1;
        } else if (this->hasSheet) {
            return (float)(this->tex_coord_frame_startX() + this->width) / (float)this->texture->glWidth;
        } else {
            return (float)this->texture->width / (float)this->texture->glWidth;
        }
    }

    float Drawable::getTexCoordStartY() {
        if (!this->hasTexture) {
            return 1;
        } else if (this->hasSheet) {
            return (float)(this->tex_coord_frame_startY() + this->height) / (float)this->texture->glHeight;
        } else {
            return (float)this->texture->height / (float)this->texture->glHeight;
        }
    }

    float Drawable::getTexCoordEndY() {
        if (this->hasSheet) {
            return this->tex_coord_frame_startY() / (float)this->texture->glHeight;
        } else {
            return 0;
        }
    }

    bool Drawable::bindVertex() {
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
            glGenBuffers(1, &this->frames_vbos[this->frame_index]);
        }

        glBindBuffer(GL_ARRAY_BUFFER, this->getCurrentBufferId());
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, this->vertex_tex_coords, GL_STATIC_DRAW);

        printGLErrors("Could not create OpenGL vertex");

        if (this->hasTexture && !this->texture->loaded) {
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

        this->loaded = true;

        return true;
    }

    void Drawable::onDrawFrame() {
        if (!this->loaded) return;

        if (this->frames_vbos[frame_index] <= 0) {
            this->bindVertex();
        }

        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity (); 

        // update colors
        glColor4f(this->param_color[0], this->param_color[1], this->param_color[2], this->param_color[3]);

        // update position
        glTranslatef(this->x, this->y, this->z);

        // update width and height
        glScalef(this->width, this->height, 1);

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

        // bind vertex positions
        glBindBuffer(GL_ARRAY_BUFFER, engine->stage->vbo[0]);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        // bind a texture
        if (this->hasTexture) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, this->texture->textureId);
        } else {
            glDisable(GL_TEXTURE_2D);
        }

        // bind texture coords
        glBindBuffer(GL_ARRAY_BUFFER, this->getCurrentBufferId());
        glTexCoordPointer(2, GL_FLOAT, 0, 0);

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

    void Drawable::setFrameIndex(int index) {
        this->frame_index = index;
    }

    GLuint Drawable::getCurrentBufferId() {
        return this->frames_vbos[this->frame_index];
    }

    void Drawable::setTexture(Image* image) {
        this->texture = image;
    }

    void Drawable::addAnimation(AnimationFrame* animation) {

        this->deleteAnimation(animation->name);
        this->animations->insert(std::make_pair(animation->name, animation)); 
    }

    void Drawable::setAnimation(std::string name) {
        this->animationName = name;
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
}