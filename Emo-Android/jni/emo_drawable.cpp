#include <stdio.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <squirrel.h>
#include <common.h>
#include <main.h>
#include <sqfunc.h>

extern struct engine *g_engine;

/*
 * free and clear all drawables
 */
void clearDrawables(struct engine* engine) {
    std::hash_map <const char *, Drawable *>::iterator iter;
    for(iter = engine->drawables.begin(); iter != engine->drawables.end(); iter++) {
        free(iter->second);
    }
    engine->drawables.clear();
}

/*
 * remove a drawable with given key.
 * returns false if the drawable is not found.
 */
bool removeDrawable(const char* key, struct engine* engine) {
    std::hash_map <const char *, Drawable *>::iterator iter = engine->drawables.find(key);
    if (iter != engine->drawables.end()) {
        free(iter->second);
        engine->drawables.erase(key);
        return true;
    }
    return false;
}

/*
 * get drawable with given key
 */
struct Drawable* getDrawable(const char* key, struct engine* engine) {
    std::hash_map <const char *, Drawable *>::iterator iter = engine->drawables.find(key);
    if (iter != engine->drawables.end()) {
        return iter->second;
    }
    return NULL;
}

/*
 * add drawable to the engine
 */
void addDrawable(const char* key, struct Drawable* drawable, struct engine* engine) {
    engine->drawables.insert(std::make_pair(key, drawable)); 
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
    memset(&drawable, 0, sizeof(drawable));
    drawable->hasTexture = false;
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
SQInteger emoDrawableCreate(HSQUIRRELVM v) {

    return 0;
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

