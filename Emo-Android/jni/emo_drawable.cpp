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
bool clearGLErrors() {
    for (GLint error = glGetError(); error; error = glGetError()) {
        // do nothing
    }
}

void loadDrawable(struct Drawable* drawable) {
    if (drawable->hasBuffer) return;
    glGenBuffers (3, drawable->vbo);

    if (drawable->hasTexture) {
        glGenTextures(1, &drawable->texture->textureId);
    }
    drawable->hasBuffer = true;
}

void unloadDrawable(struct Drawable* drawable) {
    if (!drawable->hasBuffer) return;
    if (drawable->hasTexture) {
        glDeleteTextures(1, &drawable->texture->textureId);
    }
    glDeleteBuffers(3, drawable->vbo);
    drawable->hasBuffer = false;
}

void unloadDrawables(struct engine* engine) {
    drawables_t::iterator iter;
    for(iter = engine->drawables->begin(); iter != engine->drawables->end(); iter++) {
        struct Drawable* drawable = iter->second;
        unloadDrawable(drawable);
    }
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
void clearDrawables(struct engine* engine) {
    drawables_t::iterator iter;
    for(iter = engine->drawables->begin(); iter != engine->drawables->end(); iter++) {
        struct Drawable* drawable = iter->second;
        if (drawable->hasTexture) {
            free(drawable->texture->data);
            free(drawable->texture);
        }
        free((char*)iter->first);
        free(iter->second);
    }
    engine->drawables->clear();
}

/*
 * remove a drawable with given key.
 * returns false if the drawable is not found.
 */
bool removeDrawable(const char* key, struct engine* engine) {
    drawables_t::iterator iter = engine->drawables->find(key);
    if (iter != engine->drawables->end()) {
        struct Drawable* drawable = iter->second;
        unloadDrawable(drawable);
        if (drawable->hasTexture) {
            free(drawable->texture->data);
            free(drawable->texture);
        }
        free((char*)iter->first);
        free(iter->second);
        engine->drawables->erase(key);
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

    stage->loaded = false;

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

    clearGLErrors();

    glGenBuffers(2, stage->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, stage->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(stage->positions), stage->positions, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stage->vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(stage->indices), stage->indices, GL_STATIC_DRAW);

    GLint error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        char str[128];
        sprintf(str, "Could not create OpenGL buffers: code=0x%x", error);
        LOGE(str);
        return false;
    }

    stage->loaded = true;

    return true;
}

void unloadStage(struct Stage* stage) {
    if (!stage->loaded) return;

    glDeleteBuffers(2, stage->vbo);
    stage->loaded = false;
}

/*
 * initialize drawable instance
 */
void initDrawable(struct Drawable* drawable) {
    memset(drawable, 0, sizeof(drawable));

    drawable->name       = NULL;
    drawable->hasTexture = false;
    drawable->hasBuffer  = false;

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
}

float getTexCoordStartX(struct Drawable* drawable) {
    return 0;
}

float getTexCoordStartY(struct Drawable* drawable) {
    return 0;
}

float getTexCoordEndX(struct Drawable* drawable) {
    return drawable->texture->width / drawable->texture->glWidth;
}

float getTexCoordEndY(struct Drawable* drawable) {
    return drawable->texture->height / drawable->texture->glHeight;
}

bool bindDrawableVertex(struct Drawable* drawable) {
    clearGLErrors();

    drawable->vertex_tex_coords[0] = getTexCoordStartX(drawable);
    drawable->vertex_tex_coords[1] = getTexCoordStartY(drawable);

    drawable->vertex_tex_coords[2] = getTexCoordStartX(drawable);
    drawable->vertex_tex_coords[3] = getTexCoordEndY(drawable);

    drawable->vertex_tex_coords[4] = getTexCoordEndX(drawable);
    drawable->vertex_tex_coords[5] = getTexCoordEndY(drawable);

    drawable->vertex_tex_coords[6] = getTexCoordEndX(drawable);
    drawable->vertex_tex_coords[7] = getTexCoordStartY(drawable);

    glBindBuffer(GL_ARRAY_BUFFER, drawable->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(drawable->vertex_tex_coords), drawable->vertex_tex_coords, GL_STATIC_DRAW);

    GLint error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        char str[128];
        sprintf(str, "Could not create OpenGL buffers: code=0x%x", error);
        LOGE(str);
        return false;
    }

    return true;
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
    sprintf(key, "%d%d%d%d%d", 
                g_engine->uptime.time, g_engine->uptime.millitm,
                drawable->vbo[0], drawable->vbo[1], drawable->vbo[2]);

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
    return 0;
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
    return 0;
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

    return 0;
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

    return 0;
}

/*
 * unload drawable
 */
SQInteger emoDrawableUnload(HSQUIRRELVM v) {
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

