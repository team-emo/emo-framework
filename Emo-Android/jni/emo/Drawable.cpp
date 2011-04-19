#include "stdlib.h"
#include "Drawable.h"

namespace emo {

    Drawable::Drawable() {

    }

    Drawable::~Drawable() {
        free(this->frames_vbos);
    }

    void Drawable::init() {

    }

    void Drawable::load() {
        this->frames_vbos = (GLuint *)malloc(sizeof(GLuint) * this->frameCount);

    }

    void Drawable::genTextures() {

        glGenTextures(1, &this->texture->textureId);

    }

    bool Drawable::bindVertex() {
        return false;
    }

    void Drawable::onDrawFrame() {

    }

    void Drawable::setFrameCount(int count) {
        this->frameCount = count;
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

}