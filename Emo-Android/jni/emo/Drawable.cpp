#include "stdlib.h"
#include "math.h"

#include "Drawable.h"
#include "Constants.h"
#include "Runtime.h"
#include "Engine.h"
#include "Database.h"
#include "Util.h"

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
    }

    AnimationFrame::~AnimationFrame() {

    }

    int32_t AnimationFrame::getLastOnAnimationDelta() {
        int32_t deltaSec  = engine->uptime.time - this->lastOnAnimationInterval.time;
        int32_t deltaMsec = engine->uptime.millitm - this->lastOnAnimationInterval.millitm;

        return (deltaSec * 1000) + deltaMsec;
    }

    int AnimationFrame::getNextIndex(int frameCount, int currentIndex) {

        if (this->currentLoopCount > this->loop) {
            return currentIndex;
        }

        this->currentCount++;

        if (currentCount >= this->count) {
            this->currentCount = 0;
            if (this->loop > 0) {
                this->currentLoopCount++;
            }
        }

        if (this->currentCount + this->start >= frameCount) {
            currentCount = 0;
        }

        int nextIndex = this->currentCount + this->start;

        return nextIndex;
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
        if (this->hasTexture && this->texture->referenceCount <= 1) {
            glDeleteTextures(1, &this->texture->textureId);
        }

        for (int i = 0; i < this->frameCount; i++) {
            if (this->frames_vbos[i] > 0) {
                glDeleteBuffers(1, &this->frames_vbos[i]);
                this->frames_vbos[i] = 0;
            }
        }

        this->hasBuffer = false;

        if (this->hasTexture && this->texture->referenceCount <= 1) {
            this->texture->textureId = 0;
            this->texture->loaded    = false;
        }
    }

    int Drawable::tex_coord_frame_startX() {
        int xcount = (int)round((this->texture->width - (this->margin * 2) + this->border) / (float)(this->width  + this->border));
        int xindex = this->frame_index % xcount;
        return ((this->border + this->width) * xindex) + this->margin;
    }

    int Drawable::tex_coord_frame_startY() {
        int xcount = (int)round((this->texture->width - (this->margin * 2) + this->border) / (float)(this->width  + this->border));
        int ycount = (int)round((this->texture->height - (this->margin * 2) + this->border) / (float)(this->height + this->border));
        int yindex = ycount - (this->frame_index / xcount) - 1;
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
        glTranslatef(this->x, this->y, 0);

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

    bool Drawable::setFrameIndex(int index) {
        if (index < 0 || this->frameCount <= index) return false;
        this->nextFrameIndex = index;
        this->frameIndexChanged = true;
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
            }
        }
        return true;
    }

    MapDrawable::MapDrawable(Drawable* drawable) : Drawable() {
        this->drawable = drawable;
        this->tiles = new std::vector<std::vector<int>*>;
        this->setChild(drawable);
    }

    MapDrawable::~MapDrawable() {
        for (unsigned int i = 0; i < this->tiles->size(); i++) {
            delete this->tiles->at(i);
        }
        delete this->tiles;
    }

    void MapDrawable::addRow(int rowdata[], int count) {
        std::vector<int>* row = new std::vector<int>;
        for (int i = 0; i < count; i++) {
            row->push_back(rowdata[i]);
        }
        this->tiles->push_back(row);

        this->columns = count;
        this->rows    = this->tiles->size();
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

        index.push_back(col);
        index.push_back(row);

        return index;
    }

    std::vector<float> MapDrawable::getTilePositionAtCoord(float x, float y) {
        std::vector<float> index;

        std::vector<int> tile = this->getTileIndexAtCoord(x, y);
        int col = tile.at(0);
        int row = tile.at(1);
        
        index.push_back((col * this->drawable->getScaledWidth())  + this->x);
        index.push_back((row * this->drawable->getScaledHeight()) + this->y);
        
        return index;
    }

    bool MapDrawable::clearTiles() {
        this->tiles->clear();
        return true;
    }

    void MapDrawable::load() {
        Drawable::load();
    }

    Drawable* MapDrawable::getChild() {
        return Drawable::getChild();
    }

    void MapDrawable::setChild(Drawable* child) {
        Drawable::setChild(child);
    }

    bool MapDrawable::bindVertex() {
        this->loaded = this->drawable->bindVertex();
        return this->loaded;
    }

    void MapDrawable::onDrawFrame() {

        int invertX = -this->x;
        int invertY = -this->y;

        if (this->x > 0 && this->x  > this->getScaledWidth())  return;
        if (this->y > 0 && this->y  > this->getScaledHeight()) return;
        if (this->x < 0 && invertX > this->drawable->getScaledWidth()  * this->columns) return;
        if (this->y < 0 && invertY > this->drawable->getScaledHeight() * this->rows) return;

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

    void MapDrawable::deleteBuffer() {
        this->deleteBuffer();
        this->drawable->deleteBuffer();
    }

}
