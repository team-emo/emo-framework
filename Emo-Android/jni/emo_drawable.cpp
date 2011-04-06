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

/*
 * update drawable graphic buffers
 */
void updateDrawableBuffers(struct Drawable* drawable) {
    glGenBuffers (3, drawable->vbo);
}

/*
 * initialize drawable instance
 */
void initDrawable(struct Drawable* drawable) {
    memset(drawable, 0, sizeof(drawable));

    drawable->hasTexture = false;
    drawable->loaded     = false;
    drawable->removed    = false;
    drawable->visible    = false;

    // color param RGBA
    drawable->param_color[0] = 1.0f;
    drawable->param_color[1] = 1.0f;
    drawable->param_color[2] = 1.0f;
    drawable->param_color[3] = 1.0f;

    // translate param x, y, z
    drawable->param_translate[0] = 0;
    drawable->param_translate[1] = 0;
    drawable->param_translate[2] = 0;

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

    updateDrawableBuffers(drawable);
}

/*
 * returns drawable key
 */
void updateDrawableKey(struct Drawable* drawable, char* key) {
    sprintf(key, "%d-%d-%d", drawable->vbo[0], drawable->vbo[1], drawable->vbo[2]);
}

/*
 * create drawable instance
 */
SQInteger emoDrawableCreateSprite(HSQUIRRELVM v) {

    struct Drawable *drawable = (Drawable *)malloc(sizeof(Drawable));

    initDrawable(drawable);

    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);

        drawable->name = name;
    }

    SQFloat x = 0;
    SQFloat y = 0;

    if (nargs >= 4) {
        sq_getfloat(v, 3, &x);
        sq_getfloat(v, 4, &y);
    }

    drawable->x    = x;
    drawable->y    = y;

    char key[DRAWABLE_KEY_LENGTH];
    updateDrawableKey(drawable, key);

    addDrawable(key, drawable, g_engine);

    sq_pushstring(v, key, DRAWABLE_KEY_LENGTH);

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
    if (loadPngFromAsset(drawable->name, imageInfo)) {
        drawable->texture    = imageInfo;
        drawable->hasTexture = true;
    } else {
        free(imageInfo);
        sq_pushinteger(v, ERR_ASSET_LOAD);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * move drawable
 */
SQInteger emoDrawableMove(HSQUIRRELVM v) {
    return 0;
}

/*
 * scale drawable
 */
SQInteger emoDrawableScale(HSQUIRRELVM v) {
    return 0;
}

/*
 * rotate drawable
 */
SQInteger emoDrawableRotate(HSQUIRRELVM v) {
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

