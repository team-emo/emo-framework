#include <android_native_app_glue.h>

#include <string.h>
#include <stdio.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <squirrel.h>
#include <common.h>
#include <main.h>
#include <sqfunc.h>
#include <emo_engine_func.h>
#include <emo_drawable_func.h>

extern struct engine *g_engine;

/*
 * clear all OpenGL errors
 */
void clearGLErrors(const char* msg) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        if (error != GL_NO_ERROR) {
            LOGI(msg);
            char str[128];
            sprintf(str, "err code=0x%x", error);
            LOGI(str);
        }
    }
}

bool printGLErrors(const char* msg) {
    bool result = true;
    for (GLint error = glGetError(); error; error = glGetError()) {
        if (error != GL_NO_ERROR) {
            LOGE(msg);
            char str[128];
            sprintf(str, "err code=0x%x", error);
            LOGE(str);
            result = false;
        }
    }
    return result;
}

static GLuint getCurrentDrawableTexBufferName(struct Drawable* drawable) {
    return drawable->frames_vbos[drawable->frame_index];
}

static bool isCurrentDrawableTexBufferLoaded(struct Drawable* drawable) {
    return drawable->frames_vbos[drawable->frame_index] > 0;
}

static bool deleteDrawableVBOAtFrame(struct Drawable* drawable, int index) {
    const GLuint value = drawable->frames_vbos[index];
    if (value > 0) {
        glDeleteBuffers(1, &value);
        drawable->frames_vbos[index] = 0;
        return true;
    }
    return false;
}

static int clearDrawableVBOs(struct Drawable* drawable) {
    int count = 0;
    for (int i = 0; i < drawable->frameCount; i++) {
        if (drawable->frames_vbos[i] > 0) {
            glDeleteBuffers(1, &drawable->frames_vbos[i]);
            drawable->frames_vbos[i] = 0;
            count++;
        }
    }
    return count;
}

static void deleteDrawableBuffer(struct Drawable* drawable) {
    if (!drawable->hasBuffer) return;
    if (drawable->hasTexture) {
        glDeleteTextures(1, &drawable->texture->textureId);
    }

    clearDrawableVBOs(drawable);
    drawable->hasBuffer = false;

    drawable->texture->textureId = 0;
}

void deleteDrawableBuffers(struct engine* engine) {
    drawables_t::iterator iter;
    for(iter = engine->drawables->begin(); iter != engine->drawables->end(); iter++) {
        struct Drawable* drawable = iter->second;
        deleteDrawableBuffer(drawable);
    }
}

void deleteStageBuffer(struct Stage* stage) {
    if (!stage->loaded) return;

    glDeleteBuffers(2, stage->vbo);
    stage->loaded = false;

    stage->vbo[0] = 0;
    stage->vbo[1] = 0;
}

/*
 * free drawable memory with given key.
 * returns false if the drawable is not found.
 */
bool freeDrawable(const char* key, struct engine* engine) {
    drawables_t::iterator iter = engine->drawables->find(key);
    if (iter != engine->drawables->end()) {
        struct Drawable* drawable = iter->second;
        deleteDrawableBuffer(drawable);
        char* ckey = (char*)iter->first;
        if (engine->drawables->erase(iter->first)){
            if (drawable->hasTexture) {
                free(drawable->texture->data);
                free(drawable->texture);
            }
            free(drawable->frames_vbos);
            free(ckey);
            free(drawable);
        }
        return true;
    }
    return false;
}

static void onDrawDrawable(struct Stage* stage, struct Drawable* drawable) {
    if (!drawable->loaded) return;

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity (); 

    // update colors
    glColor4f(drawable->param_color[0], drawable->param_color[1], drawable->param_color[2], drawable->param_color[3]);

    // update position
    glTranslatef(drawable->x, drawable->y, drawable->z);

    // update width and height
    glScalef(drawable->width, drawable->height, 1);

    // rotate
    glTranslatef(drawable->param_rotate[1], drawable->param_rotate[2], 0);
    if (drawable->param_rotate[3] == AXIS_X) {
        glRotatef(drawable->param_rotate[0], 1, 0, 0);
    } else if (drawable->param_rotate[3] == AXIS_Y) {
        glRotatef(drawable->param_rotate[0], 0, 1, 0);
    } else {
        glRotatef(drawable->param_rotate[0], 0, 0, 1);
    }
    glTranslatef(-drawable->param_rotate[1], -drawable->param_rotate[2], 0);

    // scale
    glTranslatef(drawable->param_scale[2], drawable->param_scale[3], 0);
    glScalef(drawable->param_scale[0], drawable->param_scale[1], 1);
    glTranslatef(-drawable->param_scale[2], -drawable->param_scale[3], 0);

    // bind vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, stage->vbo[0]);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    if (drawable->hasTexture && isCurrentDrawableTexBufferLoaded(drawable)) {
        // bind a texture
        glBindTexture(GL_TEXTURE_2D, drawable->texture->textureId);

        // bind texture coords
        glBindBuffer(GL_ARRAY_BUFFER, getCurrentDrawableTexBufferName(drawable));
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
    }

    // bind indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stage->vbo[1]);

    // draw sprite
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/*
 * draw stage
 */
void onDrawStage(struct Stage* stage) {
    if (stage->firstDraw) {
        glViewport(0, 0, stage->viewport_width, stage->viewport_height); 
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrthof(0, stage->width, stage->height, 0, -1, 1);
        stage->firstDraw = false;
    }

    glClearColor(0, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

/*
 * draw drawables
 */
void onDrawDrawables(struct engine* engine) {

    drawables_t::iterator iter;
    for(iter = engine->drawables->begin(); iter != engine->drawables->end(); iter++) {
        struct Drawable* drawable = iter->second;
        if (drawable->loaded) {
            onDrawDrawable(engine->stage, drawable);
        }
    }

    if (engine->drawablesToRemove->size() > 0) {
        for(iter = engine->drawablesToRemove->begin(); iter != engine->drawablesToRemove->end(); iter++) {
            freeDrawable(iter->first, engine);
            free((char*)iter->first);
        }
        engine->drawablesToRemove->clear();
    }
}

/*
 * load drawable (single sprite)
 */
void loadDrawable(struct Drawable* drawable) {
    if (drawable->hasBuffer) return;

    for (int i = 0; i < drawable->frameCount; i++) {
        drawable->frames_vbos[i] = 0;
    }

    // generate buffer for current frame index
    glGenBuffers(1, &drawable->frames_vbos[drawable->frame_index]);

    if (drawable->hasTexture) {
        glGenTextures(1, &drawable->texture->textureId);
    }
    drawable->hasBuffer = true;
}


void loadDrawables(struct engine* engine) {
    drawables_t::iterator iter;
    for(iter = engine->drawables->begin(); iter != engine->drawables->end(); iter++) {
        struct Drawable* drawable = iter->second;
        loadDrawable(drawable);
    }
}

/*
 * free and clear all drawables
 */
void unloadDrawables(struct engine* engine) {
    drawables_t::iterator iter;
    for(iter = engine->drawables->begin(); iter != engine->drawables->end(); iter++) {
        struct Drawable* drawable = iter->second;
        deleteDrawableBuffer(drawable);
        if (drawable->hasTexture) {
            free(drawable->texture->data);
            free(drawable->texture);
        }
        free((char*)iter->first);
        free(iter->second);
    }
    engine->drawables->clear();
}

void addDrawableToRemove(const char* _key, struct Drawable* drawable, struct engine* engine) {
    const char* key = strdup(_key);
    engine->drawablesToRemove->insert(std::make_pair(key, drawable)); 
}

/*
 * set remove flag for a drawable with given key.
 * returns false if the drawable is not found.
 */
bool removeDrawable(const char* key, struct engine* engine) {
    drawables_t::iterator iter = engine->drawables->find(key);
    if (iter != engine->drawables->end()) {
        struct Drawable* drawable = iter->second;
        addDrawableToRemove(iter->first, iter->second, engine);
        return true;
    }
    return false;
}

/*
 * get drawable with given key
 */
struct Drawable* getDrawable(const char* key, struct engine* engine) {
    drawables_t::iterator iter = engine->drawables->find(key);
    if (iter != engine->drawables->end()) {
        return iter->second;
    }
    return NULL;
}

/*
 * add drawable to the engine
 */
void addDrawable(const char* _key, struct Drawable* drawable, struct engine* engine) {
    const char* key = strdup(_key);
    engine->drawables->insert(std::make_pair(key, drawable)); 
}

bool loadStage(struct Stage* stage) {
    memset(stage, 0, sizeof(stage));

    stage->loaded    = false;
    stage->firstDraw = true;

    stage->width  = ANativeWindow_getWidth(g_engine->app->window);
    stage->height = ANativeWindow_getHeight(g_engine->app->window);

    stage->viewport_width  = stage->width;
    stage->viewport_height = stage->height;

    stage->indices[0] = 0;
    stage->indices[1] = 1;
    stage->indices[2] = 2;
    stage->indices[3] = 3;

    stage->positions[0] = 0;
    stage->positions[1] = 0;
    stage->positions[2] = 0;

    stage->positions[3] = 0;
    stage->positions[4] = 1;
    stage->positions[5] = 0;

    stage->positions[6] = 1;
    stage->positions[7] = 1;
    stage->positions[8] = 0;

    stage->positions[9]  = 1;
    stage->positions[10] = 0;
    stage->positions[11] = 0;

    stage->vbo[0] = 0;
    stage->vbo[1] = 0;

    clearGLErrors("loadStage");

    glGenBuffers(2, stage->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, stage->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 , stage->positions, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stage->vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * 4, stage->indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    printGLErrors("Could not create OpenGL buffers");

    stage->loaded = true;

    return true;
}

/*
 * initialize drawable instance
 */
void initDrawable(struct Drawable* drawable) {
    memset(drawable, 0, sizeof(drawable));

    drawable->name       = NULL;
    drawable->hasTexture = false;
    drawable->hasBuffer  = false;
    drawable->loaded     = false;
    drawable->hasSheet   = false;

    // color param RGBA
    drawable->param_color[0] = 1.0f;
    drawable->param_color[1] = 1.0f;
    drawable->param_color[2] = 1.0f;
    drawable->param_color[3] = 1.0f;

    // rotate angle, center x, center y, axis
    drawable->param_rotate[0] = 0;
    drawable->param_rotate[1] = 0;
    drawable->param_rotate[2] = 0;
    drawable->param_rotate[3] = AXIS_Z;

    // scale param x, y, center x, center y
    drawable->param_scale[0] = 1;
    drawable->param_scale[1] = 1;
    drawable->param_scale[2] = 0;
    drawable->param_scale[3] = 0;

    drawable->x = 0;
    drawable->y = 0;
    drawable->z = 0;

    drawable->width  = 0;
    drawable->height = 0;

    drawable->frameCount  = 1;
    drawable->frame_index = 0;
    drawable->border      = 0;
    drawable->margin      = 0;
}


static int tex_coord_frame_startX(struct Drawable* drawable) {
    int xindex = drawable->frame_index % (int)round((drawable->texture->width - (drawable->margin * 2) + drawable->border) / (float)(drawable->width  + drawable->border));
    return ((drawable->border + drawable->width) * xindex) + drawable->margin;
}

static int tex_coord_frame_startY(struct Drawable* drawable) {
    int ycount = (int)round((drawable->texture->height - (drawable->margin * 2) + drawable->border) / (float)(drawable->height + drawable->border));
    int yindex = ycount - 1 - ((drawable->frame_index + 1) / (int)round((drawable->texture->width - (drawable->margin * 2) + drawable->border) / (float)(drawable->width  + drawable->border)));
    return ((drawable->border + drawable->height) * yindex) + drawable->margin;
}

float getTexCoordStartX(struct Drawable* drawable) {
    if (drawable->hasSheet) {
        return tex_coord_frame_startX(drawable) / (float)drawable->texture->glWidth;
    } else {
        return 0;
    }
}

float getTexCoordEndX(struct Drawable* drawable) {
    if (drawable->hasSheet) {
        return (float)(tex_coord_frame_startX(drawable) + drawable->width) / (float)drawable->texture->glWidth;
    } else {
        return (float)drawable->texture->width / (float)drawable->texture->glWidth;
    }
}

float getTexCoordStartY(struct Drawable* drawable) {
    if (drawable->hasSheet) {
        return (float)(tex_coord_frame_startY(drawable) + drawable->height) / (float)drawable->texture->glHeight;
    } else {
        return (float)drawable->texture->height / (float)drawable->texture->glHeight;
    }
}

float getTexCoordEndY(struct Drawable* drawable) {
    if (drawable->hasSheet) {
        return tex_coord_frame_startY(drawable) / (float)drawable->texture->glHeight;
    } else {
        return 0;
    }
}

bool bindDrawableVertex(struct Drawable* drawable) {
    clearGLErrors("bindDrawableVertex");

    drawable->vertex_tex_coords[0] = getTexCoordStartX(drawable);
    drawable->vertex_tex_coords[1] = getTexCoordStartY(drawable);

    drawable->vertex_tex_coords[2] = getTexCoordStartX(drawable);
    drawable->vertex_tex_coords[3] = getTexCoordEndY(drawable);

    drawable->vertex_tex_coords[4] = getTexCoordEndX(drawable);
    drawable->vertex_tex_coords[5] = getTexCoordEndY(drawable);

    drawable->vertex_tex_coords[6] = getTexCoordEndX(drawable);
    drawable->vertex_tex_coords[7] = getTexCoordStartY(drawable);

    // generate buffer on demand
    if (drawable->frames_vbos[drawable->frame_index] == 0) {
        glGenBuffers(1, &drawable->frames_vbos[drawable->frame_index]);
    }

    glBindBuffer(GL_ARRAY_BUFFER, getCurrentDrawableTexBufferName(drawable));
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, drawable->vertex_tex_coords, GL_STATIC_DRAW);

    printGLErrors("Could not create OpenGL vertex");

    if (drawable->hasTexture && !drawable->texture->loaded) {
        glBindTexture   (GL_TEXTURE_2D, drawable->texture->textureId);

        glPixelStorei   (GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (drawable->texture->hasAlpha) {
            GLubyte* holder = (GLubyte*)malloc(sizeof(GLubyte) * drawable->texture->glWidth * drawable->texture->glHeight * 4);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, drawable->texture->glWidth, drawable->texture->glHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, holder);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 
                  0, 0, drawable->texture->width, drawable->texture->height, GL_RGBA, GL_UNSIGNED_BYTE, drawable->texture->data);
            free(holder);
        } else {
            GLubyte* holder = (GLubyte*)malloc(sizeof(GLubyte) * drawable->texture->glWidth * drawable->texture->glHeight * 3);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, drawable->texture->glWidth, drawable->texture->glHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, holder);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 
                   0, 0, drawable->texture->width, drawable->texture->height, GL_RGB, GL_UNSIGNED_BYTE, drawable->texture->data);
            free(holder);
        }
        drawable->texture->loaded = true;
        printGLErrors("Could not bind OpenGL textures");
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    drawable->loaded = true;

    return true;
}

void rebindStageBuffers(struct Stage* stage) {
    glGenBuffers(2, stage->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, stage->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 , stage->positions, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stage->vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * 4, stage->indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    printGLErrors("Could not create OpenGL buffers");

    stage->firstDraw = true;
    stage->loaded = true;
}

void rebindDrawableBuffers(struct engine* engine) {
    drawables_t::iterator iter;
    for(iter = engine->drawables->begin(); iter != engine->drawables->end(); iter++) {
        struct Drawable* drawable = iter->second;
        loadDrawable(drawable);
        bindDrawableVertex(drawable);
    }
}
