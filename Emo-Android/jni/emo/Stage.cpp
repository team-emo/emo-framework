#include "Engine.h"
#include "Runtime.h"

extern emo::Engine* engine;

namespace emo {
    Stage::Stage() {

    }

    Stage::~Stage() {

    }

    bool Stage::onLoad() {

        this->loaded    = false;
        this->dirty = true;

        this->width  = ANativeWindow_getWidth(engine->app->window);
        this->height = ANativeWindow_getHeight(engine->app->window);

        this->viewport_width  = this->width;
        this->viewport_height = this->height;

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

    void Stage::onDrawFrame() {
        if (this->dirty) {
            glViewport(0, 0, this->viewport_width, this->viewport_height); 
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrthof(0, this->width, this->height, 0, -1, 1);
            this->dirty = false;
        }

        glClearColor(0, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Stage::deleteBuffer() {
        if (!this->loaded) return;

        glDeleteBuffers(2, this->vbo);
        this->loaded = false;

        this->vbo[0] = 0;
        this->vbo[1] = 0;
    }

    void Stage::rebindBuffer() {
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


}
