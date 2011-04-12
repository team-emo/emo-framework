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

static void deleteDrawableBuffer(struct Drawable* drawable) {
    if (!drawable->hasBuffer) return;
    if (drawable->hasTexture) {
        glDeleteTextures(1, &drawable->texture->textureId);
    }
    glDeleteBuffers(1, drawable->vbo);
    drawable->hasBuffer = false;

    drawable->vbo[0] = 0;
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

            free(ckey);
            free(drawable);
        }
        return true;
    }
    return false;
}

void printDrawableInfo(struct Stage* stage, struct Drawable* drawable) {
    char str[1024];
    sprintf(str, "%d-%d-%d %d", stage->vbo[0], stage->vbo[1], drawable->vbo[0], drawable->texture->textureId);
    LOGI(str);
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

    if (drawable->hasTexture) {
        // bind a texture
        glBindTexture(GL_TEXTURE_2D, drawable->texture->textureId);

        // bind texture coords
        glBindBuffer(GL_ARRAY_BUFFER, drawable->vbo[0]);
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

void loadDrawable(struct Drawable* drawable) {
    if (drawable->hasBuffer) return;
    glGenBuffers (1, drawable->vbo);

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

    drawable->vbo[0] = 0;
}

float getTexCoordStartX(struct Drawable* drawable) {
    return 0;
}

float getTexCoordStartY(struct Drawable* drawable) {
    return (float)drawable->texture->height / (float)drawable->texture->glHeight;
}

float getTexCoordEndX(struct Drawable* drawable) {
    return (float)drawable->texture->width / (float)drawable->texture->glWidth;
}

float getTexCoordEndY(struct Drawable* drawable) {
    return 0;
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

    glBindBuffer(GL_ARRAY_BUFFER, drawable->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, drawable->vertex_tex_coords, GL_STATIC_DRAW);

    printGLErrors("Could not create OpenGL vertex");

    if (drawable->hasTexture) {
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

/*
 * create drawable instance
 */
SQInteger emoDrawableCreateSprite(HSQUIRRELVM v) {

    struct Drawable *drawable = (Drawable *)malloc(sizeof(Drawable));

    initDrawable(drawable);
    loadDrawable(drawable);

    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);

        if (strlen(name) > 0) {
            drawable->name = name;
        }
    }

    SQFloat x = 0;
    SQFloat y = 0;
    SQFloat z = 0;

    if (nargs >= 3) sq_getfloat(v, 3, &x);
    if (nargs >= 4) sq_getfloat(v, 4, &y);
    if (nargs >= 5) sq_getfloat(v, 5, &z);

    drawable->x    = x;
    drawable->y    = y;
    drawable->z    = z;

    char key[DRAWABLE_KEY_LENGTH];
    sprintf(key, "%d%d%d", 
                g_engine->uptime.time, g_engine->uptime.millitm, drawable->vbo[0]);

    addDrawable(key, drawable, g_engine);

    sq_pushstring(v, key, strlen(key));

    return 1;
}

SQInteger emoDrawableLoad(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    struct Drawable* drawable = getDrawable(id, g_engine);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    struct ImageInfo* imageInfo = (ImageInfo *)malloc(sizeof(ImageInfo));
    if (drawable->name != NULL && loadPngFromAsset(drawable->name, imageInfo)) {

        // calculate the size of power of two
        imageInfo->glWidth  = nextPowerOfTwo(imageInfo->width);
        imageInfo->glHeight = nextPowerOfTwo(imageInfo->height);

        drawable->texture    = imageInfo;
        drawable->hasTexture = true;
        drawable->width  = imageInfo->width;
        drawable->height = imageInfo->height;

        glGenTextures(1, &drawable->texture->textureId);
    } else {
        free(imageInfo);
        sq_pushinteger(v, ERR_ASSET_LOAD);
        return 1;
    }

    // drawable x
    if (nargs >= 3) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        drawable->x = x;
    }

    // drawable y
    if (nargs >= 4) {
        SQFloat y;
        sq_getfloat(v, 4, &y);
        drawable->y = y;
    }

    // drawable width
    if (nargs >= 5) {
        SQFloat width;
        sq_getfloat(v, 5, &width);
        drawable->width = width;
    }

    // drawable height
    if (nargs >= 6) {
        SQFloat height;
        sq_getfloat(v, 6, &height);
        drawable->height = height;
    }

    if (bindDrawableVertex(drawable)) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_CREATE_VERTEX);
    }

    return 1;
}

/*
 * move drawable
 */
SQInteger emoDrawableMove(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    struct Drawable* drawable = getDrawable(id, g_engine);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (nargs >= 3) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        drawable->x = x;
    }

    if (nargs >= 4) {
        SQFloat y;
        sq_getfloat(v, 4, &y);
        drawable->y = y;
    }

    if (nargs >= 5) {
        SQFloat z;
        sq_getfloat(v, 5, &z);
        drawable->z = z;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * update color of the drawable
 */
SQInteger emoDrawableColor(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    struct Drawable* drawable = getDrawable(id, g_engine);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (nargs >= 3) {
        SQFloat r;
        sq_getfloat(v, 3, &r);
        drawable->param_color[0] = r;
    }

    if (nargs >= 4) {
        SQFloat g;
        sq_getfloat(v, 4, &g);
        drawable->param_color[1] = g;
    }

    if (nargs >= 5) {
        SQFloat b;
        sq_getfloat(v, 5, &b);
        drawable->param_color[2] = b;
    }

    if (nargs >= 6) {
        SQFloat a;
        sq_getfloat(v, 6, &a);
        drawable->param_color[3] = a;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * scale drawable
 */
SQInteger emoDrawableScale(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    struct Drawable* drawable = getDrawable(id, g_engine);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    // scale x
    if (nargs >= 3) {
        SQFloat f;
        sq_getfloat(v, 3, &f);
        drawable->param_scale[0] = f;
    }

    // scale y
    if (nargs >= 4) {
        SQFloat f;
        sq_getfloat(v, 4, &f);
        drawable->param_scale[1] = f;
    }

    // center x
    if (nargs >= 5) {
        SQFloat f;
        sq_getfloat(v, 5, &f);
        drawable->param_scale[2] = f;
    } else {
        drawable->param_scale[2] = drawable->width * 0.5f;
    }

    // center y
    if (nargs >= 6) {
        SQFloat f;
        sq_getfloat(v, 6, &f);
        drawable->param_scale[3] = f;
    } else {
        drawable->param_scale[3] = drawable->height * 0.5f;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * rotate drawable
 */
SQInteger emoDrawableRotate(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    struct Drawable* drawable = getDrawable(id, g_engine);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    // angle
    if (nargs >= 3) {
        SQFloat f;
        sq_getfloat(v, 3, &f);
        drawable->param_rotate[0] = f;
    }

    // center x
    if (nargs >= 4) {
        SQFloat f;
        sq_getfloat(v, 4, &f);
        drawable->param_rotate[1] = f;
    } else {
        drawable->param_rotate[1] = drawable->width * 0.5f;
    }

    // center y
    if (nargs >= 5) {
        SQFloat f;
        sq_getfloat(v, 5, &f);
        drawable->param_rotate[2] = f;
    } else {
        drawable->param_rotate[2] = drawable->height * 0.5f;
    }

    // rotate axis
    if (nargs >= 6) {
        SQFloat f;
        sq_getfloat(v, 6, &f);
        drawable->param_rotate[3] = f;
    } else {
        drawable->param_rotate[3] = AXIS_Z;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * remove drawable
 */
SQInteger emoDrawableRemove(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    struct Drawable* drawable = getDrawable(id, g_engine);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (removeDrawable(id, g_engine)) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_ASSET_UNLOAD);
    }

    return 1;
}

/*
 * set onDraw interval
 */
SQInteger emoSetOnDrawInterval(HSQUIRRELVM v) {
    SQInteger oldInterval = g_engine->onDrawDrawablesInterval;
	
    if (sq_gettype(v, 2) == OT_INTEGER) {
        SQInteger interval;
        sq_getinteger(v, 2, &interval);
        g_engine->onDrawDrawablesInterval = interval;
    }
    sq_pushinteger(v, oldInterval);
	
	return 1;
}

SQInteger emoSetViewport(HSQUIRRELVM v) {

    SQInteger width  = g_engine->stage->viewport_width;
    SQInteger height = g_engine->stage->viewport_height;

    if (sq_gettype(v, 2) == OT_INTEGER && sq_gettype(v, 3) == OT_INTEGER) {
        sq_getinteger(v, 2, &width);
        sq_getinteger(v, 3, &height);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }

    g_engine->stage->viewport_width  = width;
    g_engine->stage->viewport_height = height;
    g_engine->stage->firstDraw = true;

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoSetStageSize(HSQUIRRELVM v) {
    SQInteger width  = g_engine->stage->width;
    SQInteger height = g_engine->stage->height;

    if (sq_gettype(v, 2) == OT_INTEGER && sq_gettype(v, 3) == OT_INTEGER) {
        sq_getinteger(v, 2, &width);
        sq_getinteger(v, 3, &height);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }

    g_engine->stage->width  = width;
    g_engine->stage->height = height;
    g_engine->stage->firstDraw = true;

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoGetWindowWidth(HSQUIRRELVM v) {
    sq_pushinteger(v, ANativeWindow_getWidth(g_engine->app->window));
    return 1;
}

SQInteger emoGetWindowHeight(HSQUIRRELVM v) {
    sq_pushinteger(v, ANativeWindow_getHeight(g_engine->app->window));
    return 1;
}
