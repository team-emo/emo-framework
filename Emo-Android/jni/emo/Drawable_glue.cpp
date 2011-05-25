#include <android_native_app_glue.h>

#include <squirrel.h>

#include "Constants.h"
#include "Engine.h"
#include "Drawable_glue.h"
#include "Runtime.h"
#include "VmFunc.h"

extern emo::Engine* engine;

void initDrawableFunctions() {
    registerClass(engine->sqvm, EMO_STAGE_CLASS);

    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createSprite",     emoDrawableCreateSprite);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createSpriteSheet",   emoDrawableCreateSpriteSheet);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createMapSprite",     emoDrawableCreateMapSprite);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "loadSprite",       emoDrawableLoad);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "loadMapSprite",    emoDrawableLoadMapSprite);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "addTileRow",       emoDrawableAddTileRow);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "clearTiles",       emoDrawableClearTiles);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "setTileAt",        emoDrawableSetTileAt);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getTileAt",        emoDrawableGetTileAt);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getTileIndexAtCoord",    emoDrawableGetTileIndexAtCoord);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getTilePositionAtCoord", emoDrawableGetTilePositionAtCoord);

    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getX",           emoDrawableGetX);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getY",           emoDrawableGetY);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getZ",           emoDrawableGetZ);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getWidth",       emoDrawableGetWidth);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getHeight",      emoDrawableGetHeight);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getScaleX",      emoDrawableGetScaleX);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getScaleY",      emoDrawableGetScaleY);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getAngle",       emoDrawableGetAngle);

    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "setX",           emoDrawableSetX);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "setY",           emoDrawableSetY);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "setZ",           emoDrawableSetZ);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "setWidth",       emoDrawableSetWidth);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "setHeight",      emoDrawableSetHeight);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "setSize",        emoDrawableSetSize);
    
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "move",           emoDrawableMove);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "scale",          emoDrawableScale);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "rotate",         emoDrawableRotate);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "color",          emoDrawableColor);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "remove",         emoDrawableRemove);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "interval",       emoSetOnDrawInterval);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "viewport",       emoSetViewport);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "ortho",          emoSetStageSize);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "windowWidth",    emoGetWindowWidth);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "windowHeight",   emoGetWindowHeight);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "show",           emoDrawableShow);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "hide",           emoDrawableHide);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "red",            emoDrawableColorRed);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "green",          emoDrawableColorGreen);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "blue",           emoDrawableColorBlue);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "alpha",          emoDrawableColorAlpha);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "pauseAt",        emoDrawablePauseAt);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "pause",          emoDrawablePause);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "stop",           emoDrawableStop);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "animate",        emoDrawableAnimate);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getFrameIndex",  emoDrawableGetFrameIndex);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getFrameCount",  emoDrawableGetFrameCount);
}

/*
 * create drawable instance (single sprite)
 */
SQInteger emoDrawableCreateSprite(HSQUIRRELVM v) {

    emo::Drawable* drawable = new emo::Drawable();

    drawable->setFrameCount(1);
    drawable->load();

    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);

        if (strlen(name) > 0) {
            drawable->name = name;
        }
    } else {
        name = NULL;
    }

    int width  = 0;
    int height = 0;
    if (name != NULL && strlen(name) > 0) {
        if (!loadPngSizeFromAsset(name, &width, &height) || width <= 0 || height <= 0) {
            delete drawable;
            return 0;
        }
    }
    drawable->width  = width;
    drawable->height = height;

    char key[DRAWABLE_KEY_LENGTH];
    sprintf(key, "%ld%d-%d", 
                engine->uptime.time, engine->uptime.millitm, drawable->getCurrentBufferId());

    engine->addDrawable(key, drawable);

    sq_pushstring(v, key, strlen(key));

    return 1;
}

/*
 * create drawable instance (sprite sheet)
 */
SQInteger emoDrawableCreateSpriteSheet(HSQUIRRELVM v) {

    emo::Drawable* drawable = new emo::Drawable();

    const SQChar* name = NULL;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);

        if (strlen(name) > 0) {
            drawable->name = name;
        }
    } else {
        name = NULL;
    }

    SQInteger frameIndex = 0;
    SQInteger frameWidth = 0;
    SQInteger frameHeight = 0;
    SQInteger border = 0;
    SQInteger margin = 0;
    if (nargs >= 5 && sq_gettype(v, 3) != OT_NULL && sq_gettype(v, 4) != OT_NULL && sq_gettype(v, 5) != OT_NULL) {
        sq_getinteger(v, 3, &frameIndex);
        sq_getinteger(v, 4, &frameWidth);
        sq_getinteger(v, 5, &frameHeight);
    }

    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
        sq_getinteger(v, 6, &border);
    }

    if (nargs >= 7 && sq_gettype(v, 7) != OT_NULL) {
        sq_getinteger(v, 7, &margin);
    }

    int width  = 0;
    int height = 0;

    if (name != NULL && strlen(name) > 0) {
         if (!loadPngSizeFromAsset(name, &width, &height) || width <= 0 || height <= 0 || frameWidth <= 0 || frameHeight <= 0) {
            delete drawable;
            return 0;
        }
    }

    drawable->hasSheet = true;
    drawable->width  = frameWidth;
    drawable->height = frameHeight;
    drawable->border = border;

    if (margin == 0 && border != 0) {
        drawable->margin = border;
    } else {
        drawable->margin = margin;
    }

    drawable->setFrameCount((int)floor(width / (float)(frameWidth  + border)) 
                                       * floor(height /(float)(frameHeight + border)));
    if (drawable->getFrameCount() <= 0) drawable->setFrameCount(1);

    drawable->setFrameIndex(frameIndex);

    drawable->load();

    char key[DRAWABLE_KEY_LENGTH];
    sprintf(key, "%ld%d-%d", 
                engine->uptime.time, engine->uptime.millitm, drawable->getCurrentBufferId());

    engine->addDrawable(key, drawable);

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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (!drawable->name.empty()) {
         emo::Image* image = NULL;

        if (engine->hasCachedImage(drawable->name)) {
            image = engine->getCachedImage(drawable->name);
        } else {
            image = new emo::Image();
            if (loadPngFromAsset(drawable->name.c_str(), image)) {

                // calculate the size of power of two
                image->glWidth  = nextPowerOfTwo(image->width);
                image->glHeight = nextPowerOfTwo(image->height);
                image->loaded   = false;

                image->genTextures();

                engine->addCachedImage(drawable->name, image);
            } else {
                delete image;
                sq_pushinteger(v, ERR_ASSET_LOAD);
                return 1;
            }
        }

        if (image != NULL) {
            drawable->setTexture(image);
            drawable->hasTexture = true;

            if (!drawable->hasSheet) {
                if (drawable->width <= 0) {
                    drawable->width  = image->width;
                }
                if (drawable->height <= 0) {
                    drawable->height = image->height;
                }
            }

            image->referenceCount++;
        }
    }

    // drawable x
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        drawable->x = x;
    }

    // drawable y
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat y;
        sq_getfloat(v, 4, &y);
        drawable->y = y;
    }

    // drawable width
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        SQInteger width;
        sq_getinteger(v, 5, &width);
        drawable->width = width;
    }

    // drawable height
    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
        SQInteger height;
        sq_getinteger(v, 6, &height);
        drawable->height = height;
    }
    
    if (drawable->width  == 0) drawable->width  = 1;
    if (drawable->height == 0) drawable->height = 1;

    if (drawable->bindVertex()) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_CREATE_VERTEX);
    }

    return 1;
}

SQInteger emoDrawableCreateMapSprite(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, 0);
        return 1;
    }

    // create parent sprite
    emo::MapDrawable* parent = new emo::MapDrawable(drawable);
    drawable->independent = false;

    parent->setFrameCount(1);
    parent->load();

    char key[DRAWABLE_KEY_LENGTH];
    sprintf(key, "%ld%d-%d", 
                engine->uptime.time, engine->uptime.millitm, parent->getCurrentBufferId());
    engine->addDrawable(key, parent);

    sq_pushstring(v, key, strlen(key));

    return 1;
}

SQInteger emoDrawableLoadMapSprite(HSQUIRRELVM v) {
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

    emo::Drawable* parent   = engine->getDrawable(id);

    if (parent == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    emo::Drawable* drawable = parent->getChild();
    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    // load drawable texture
    if (!drawable->name.empty()) {
         emo::Image* image = NULL;

        if (engine->hasCachedImage(drawable->name)) {
            image = engine->getCachedImage(drawable->name);
        } else {
            image = new emo::Image();
            if (loadPngFromAsset(drawable->name.c_str(), image)) {

                // calculate the size of power of two
                image->glWidth  = nextPowerOfTwo(image->width);
                image->glHeight = nextPowerOfTwo(image->height);
                image->loaded   = false;

                image->genTextures();

                engine->addCachedImage(drawable->name, image);
            } else {
                delete image;
                sq_pushinteger(v, ERR_ASSET_LOAD);
                return 1;
            }
        }

        if (image != NULL) {
            drawable->setTexture(image);
            drawable->hasTexture = true;

            if (!drawable->hasSheet) {
                if (drawable->width <= 0) {
                    drawable->width  = image->width;
                }
                if (drawable->height <= 0) {
                    drawable->height = image->height;
                }
            }

            image->referenceCount++;
        }
    }

    // parent x
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        parent->x = x;
    }

    // parent y
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat y;
        sq_getfloat(v, 4, &y);
        parent->y = y;
    }

    // parent width
    SQInteger width = engine->stage->width;
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        sq_getinteger(v, 5, &width);
    }

    // parent height
    SQInteger height = engine->stage->height;
    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
        sq_getinteger(v, 6, &height);
    }

    parent->width  = width;
    parent->height = height;

    if (parent->bindVertex()) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_CREATE_VERTEX);
    }

    return 1;
}

SQInteger emoDrawableAddTileRow(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (nargs >= 3 && sq_gettype(v, 3) == OT_ARRAY) {
        SQInteger size = sq_getsize(v, 3);
        int* tile = new int[size];
        sq_pushnull(v);
        int idx = 0;
        while(SQ_SUCCEEDED(sq_next(v, -2))) {
            if (idx >= size) break;
            if (sq_gettype(v, -1) != OT_NULL) {
                SQInteger value;
                sq_getinteger(v, -1, &value);
                tile[idx] = value;
            } else {
                tile[idx] = 0;
            }
            idx++;
            sq_pop(v, 2);
        }

        drawable->addRow(tile, size);

        sq_pop(v, 1);
        delete tile;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableClearTiles(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (drawable->clearTiles()) {
        sq_pushinteger(v, EMO_NO_ERROR);
        return 1;
    } else {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
}

SQInteger emoDrawableSetTileAt(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (nargs >= 5 && sq_gettype(v, 3) != OT_NULL &&
                      sq_gettype(v, 4) != OT_NULL &&
                      sq_gettype(v, 5) != OT_NULL) {
        SQInteger row;
        SQInteger column;
        SQInteger value;
        sq_getinteger(v, 3, &row);
        sq_getinteger(v, 4, &column);
        sq_getinteger(v, 5, &value);

        drawable->setTileAt(row, column, value);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableGetTileAt(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    if (nargs >= 4 && sq_gettype(v, 3) != OT_NULL &&
                      sq_gettype(v, 4) != OT_NULL) {
        SQInteger row;
        SQInteger column;
        sq_getinteger(v, 3, &row);
        sq_getinteger(v, 4, &column);

        sq_pushinteger(v, drawable->getTileAt(row, column));
    } else {
        return 0;
    }

    return 1;
}

SQInteger emoDrawableGetTileIndexAtCoord(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    if (nargs >= 4 && sq_gettype(v, 3) != OT_NULL &&
                      sq_gettype(v, 4) != OT_NULL) {
        SQFloat x;
        SQFloat y;
        sq_getfloat(v, 3, &x);
        sq_getfloat(v, 4, &y);

        std::vector<int> index = drawable->getTileIndexAtCoord(x, y);

        if (index.size() < 2) return 0;

        sq_newarray(v, 0);

        sq_pushinteger(v, index.at(0));
        sq_arrayappend(v, -2);

        sq_pushinteger(v, index.at(1));
        sq_arrayappend(v, -2);

        sq_push(v, -1);
    } else {
        return 0;
    }

    return 1;
}

SQInteger emoDrawableGetTilePositionAtCoord(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    if (nargs >= 4 && sq_gettype(v, 3) != OT_NULL &&
                      sq_gettype(v, 4) != OT_NULL) {
        SQFloat x;
        SQFloat y;
        sq_getfloat(v, 3, &x);
        sq_getfloat(v, 4, &y);

        std::vector<float> position = drawable->getTilePositionAtCoord(x, y);

        if (position.size() < 2) return 0;

        sq_newarray(v, 0);

        sq_pushfloat(v, position.at(0));
        sq_arrayappend(v, -2);

        sq_pushfloat(v, position.at(1));
        sq_arrayappend(v, -2);

        sq_push(v, -1);
    } else {
        return 0;
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        drawable->x = x;
    }

    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat y;
        sq_getfloat(v, 4, &y);
        drawable->y = y;
    }

    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        SQFloat z;
        sq_getfloat(v, 5, &z);
        drawable->z = z;
        engine->sortOrderDirty = true;
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat r;
        sq_getfloat(v, 3, &r);
        drawable->param_color[0] = r;
    }

    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat g;
        sq_getfloat(v, 4, &g);
        drawable->param_color[1] = g;
    }

    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        SQFloat b;
        sq_getfloat(v, 5, &b);
        drawable->param_color[2] = b;
    }

    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    // scale x
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 3, &f);
        drawable->param_scale[0] = f;
    }

    // scale y
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 4, &f);
        drawable->param_scale[1] = f;
    }

    // center x
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 5, &f);
        drawable->param_scale[2] = f;
    } else {
        drawable->param_scale[2] = drawable->width * 0.5f;
    }

    // center y
    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    // angle
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 3, &f);

        // to avoid overflow
        if (f >= 360)  f = f - (360 * floor(f / 360));
  
        drawable->param_rotate[0] = f;
    }

    // center x
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 4, &f);
        drawable->param_rotate[1] = f;
    } else {
        drawable->param_rotate[1] = drawable->width * 0.5f;
    }

    // center y
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        SQFloat f;
        sq_getfloat(v, 5, &f);
        drawable->param_rotate[2] = f;
    } else {
        drawable->param_rotate[2] = drawable->height * 0.5f;
    }

    // rotate axis
    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (engine->removeDrawable(id)) {
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
    SQInteger oldInterval = engine->onDrawDrawablesInterval;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        SQInteger interval;
        sq_getinteger(v, 2, &interval);
        engine->onDrawDrawablesInterval = interval;
    }
    sq_pushinteger(v, oldInterval);
	
	return 1;
}

SQInteger emoSetViewport(HSQUIRRELVM v) {

    SQInteger width  = engine->stage->viewport_width;
    SQInteger height = engine->stage->viewport_height;

    if (sq_gettype(v, 2) != OT_NULL && sq_gettype(v, 3) != OT_NULL) {
        sq_getinteger(v, 2, &width);
        sq_getinteger(v, 3, &height);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }

    engine->stage->viewport_width  = width;
    engine->stage->viewport_height = height;
    engine->stage->dirty = true;

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoSetStageSize(HSQUIRRELVM v) {
    SQInteger width  = engine->stage->width;
    SQInteger height = engine->stage->height;

    if (sq_gettype(v, 2) != OT_NULL && sq_gettype(v, 3) != OT_NULL) {
        sq_getinteger(v, 2, &width);
        sq_getinteger(v, 3, &height);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }

    engine->stage->width  = width;
    engine->stage->height = height;
    engine->stage->dirty = true;

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoGetWindowWidth(HSQUIRRELVM v) {
    sq_pushinteger(v, engine->stage->width);
    return 1;
}

SQInteger emoGetWindowHeight(HSQUIRRELVM v) {
    sq_pushinteger(v, engine->stage->height);
    return 1;
}

SQInteger emoDrawableShow(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    drawable->param_color[3] = 1;

    sq_pushinteger(v, EMO_NO_ERROR);

    return 1;
}

SQInteger emoDrawableHide(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    drawable->param_color[3] = 0;

    sq_pushinteger(v, EMO_NO_ERROR);

    return 1;
}

SQInteger emoDrawableColorRed(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat color;
        sq_getfloat(v, 3, &color);
        drawable->param_color[0] = color;
    }

    sq_pushinteger(v, drawable->param_color[0]);
    return 1;
}

SQInteger emoDrawableColorGreen(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat color;
        sq_getfloat(v, 3, &color);
        drawable->param_color[1] = color;
    }

    sq_pushinteger(v, drawable->param_color[1]);
    return 1;
}

SQInteger emoDrawableColorBlue(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat color;
        sq_getfloat(v, 3, &color);
        drawable->param_color[2] = color;
    }

    sq_pushinteger(v, drawable->param_color[2]);
    return 1;
}

SQInteger emoDrawableColorAlpha(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat color;
        sq_getfloat(v, 3, &color);
        drawable->param_color[3] = color;
    }

    sq_pushinteger(v, drawable->param_color[3]);
    return 1;
}

SQInteger emoDrawableSetX(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat x;
        sq_getfloat(v, 3, &x);
        drawable->x = x;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableSetY(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat y;
        sq_getfloat(v, 3, &y);
        drawable->y = y;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableSetZ(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQFloat z;
        sq_getfloat(v, 3, &z);
        drawable->z = z;
        engine->sortOrderDirty = true;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableSetWidth(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQInteger w;
        sq_getinteger(v, 3, &w);
        drawable->width = w;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableSetHeight(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQInteger h;
        sq_getinteger(v, 3, &h);
        drawable->height = h;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableSetSize(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQInteger w;
        sq_getinteger(v, 3, &w);
        drawable->width = w;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        SQInteger h;
        sq_getinteger(v, 4, &h);
        drawable->height = h;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}


SQInteger emoDrawablePauseAt(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
    
    SQInteger index;
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        sq_getinteger(v, 3, &index);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
    
    if (drawable->getFrameCount() <= index) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    drawable->setFrameIndex(index);
    drawable->enableAnimation(false);

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawablePause(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }

    drawable->enableAnimation(false);

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableStop(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
    
    drawable->setFrameIndex(0);
    drawable->enableAnimation(false);
    
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableAnimate(HSQUIRRELVM v) {
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

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
    
    SQInteger start = 0;
    SQInteger count = 1;
    SQInteger interval = 0;
    SQInteger loop  = 0;
    
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        sq_getinteger(v, 3, &start);
    }
    if (nargs >= 4 && sq_gettype(v, 4) != OT_NULL) {
        sq_getinteger(v, 4, &count);
    }
    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        sq_getinteger(v, 5, &interval);
    }
    if (nargs >= 6 && sq_gettype(v, 6) != OT_NULL) {
        sq_getinteger(v, 6, &loop);
    }

    emo::AnimationFrame* animation = new emo::AnimationFrame();
    animation->name  = DEFAULT_ANIMATION_NAME;
    animation->start = start;
    animation->count = count;
    animation->interval   = interval;
    animation->loop       = loop;
    
    drawable->addAnimation(animation), 
    drawable->setAnimation(animation->name);
    drawable->enableAnimation(true);
    
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableGetX(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    sq_pushinteger(v, drawable->x);
    return 1;
}

SQInteger emoDrawableGetY(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    sq_pushinteger(v, drawable->y);
    return 1;
}

SQInteger emoDrawableGetZ(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    sq_pushinteger(v, drawable->z);
    return 1;
}

SQInteger emoDrawableGetWidth(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    sq_pushinteger(v, drawable->width);
    return 1;
}

SQInteger emoDrawableGetHeight(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    sq_pushinteger(v, drawable->height);
    return 1;
}

SQInteger emoDrawableGetScaleX(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    sq_pushfloat(v, drawable->param_scale[0]);
    return 1;
}

SQInteger emoDrawableGetScaleY(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    sq_pushfloat(v, drawable->param_scale[1]);
    return 1;
}

SQInteger emoDrawableGetAngle(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    emo::Drawable* drawable = engine->getDrawable(id);

    if (drawable == NULL) {
        return 0;
    }

    sq_pushfloat(v, drawable->param_rotate[0]);
    return 1;
}

SQInteger emoDrawableGetFrameIndex(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    emo::Drawable* drawable = engine->getDrawable(id);
	
    if (drawable == NULL) {
		return 0;
    }
	
    sq_pushinteger(v, drawable->getFrameIndex());
    return 1;
}

SQInteger emoDrawableGetFrameCount(HSQUIRRELVM v) {
    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
		return 0;
    }
	
    emo::Drawable* drawable = engine->getDrawable(id);
	
    if (drawable == NULL) {
		return 0;
    }
	
    sq_pushinteger(v, drawable->getFrameCount());
    return 1;
}
