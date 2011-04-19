#include <android_native_app_glue.h>

#include <squirrel.h>

#include "Constants.h"
#include "Engine.h"

extern emo::Engine* engine;

void initDrawableFunctions() {
    engine->registerClass(engine->sqvm, EMO_STAGE_CLASS);

    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createSprite",   emoDrawableCreateSprite);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createSpriteSheet",   emoDrawableCreateSpriteSheet);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "loadSprite",     emoDrawableLoad);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "move",           emoDrawableMove);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "scale",          emoDrawableScale);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "rotate",         emoDrawableRotate);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "color",          emoDrawableColor);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "remove",         emoDrawableRemove);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "interval",       emoSetOnDrawInterval);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "viewport",       emoSetViewport);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "ortho",          emoSetStageSize);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "windowWidth",    emoGetWindowWidth);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "windowHeight",   emoGetWindowHeight);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "show",           emoDrawableShow);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "hide",           emoDrawableHide);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "red",            emoDrawableColorRed);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "green",          emoDrawableColorGreen);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "blue",           emoDrawableColorBlue);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "alpha",          emoDrawableColorAlpha);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "pauseAt",        emoDrawablePauseAt);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "pause",          emoDrawablePause);
    engine->registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "stop",           emoDrawableStop);
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
    sprintf(key, "%d%d-%d", 
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
    }

    SQInteger frameIndex = 0;
    SQInteger frameWidth, frameHeight;
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

    if (name != NULL && !loadPngSizeFromAsset(name, &width, &height) || 
          width <= 0 || height <= 0 || frameWidth <= 0 || frameHeight <= 0) {
        free(drawable);
        sq_pushinteger(v, -1);
        return 1;
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
    sprintf(key, "%d%d-%d", 
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

    if (drawable->name != NULL) {
        emo::Image* image = new emo::Image();
        if (loadPngFromAsset(drawable->name, image)) {

            // calculate the size of power of two
            image->glWidth  = nextPowerOfTwo(image->width);
            image->glHeight = nextPowerOfTwo(image->height);
            image->loaded   = false;

            drawable->setTexture(image);
            drawable->hasTexture = true;

            if (!drawable->hasSheet) {
                drawable->width  = image->width;
                drawable->height = image->height;
            }

            image->genTextures();
        } else {
            delete image;
            sq_pushinteger(v, ERR_ASSET_LOAD);
            return 1;
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
	
    if (sq_gettype(v, 2) == OT_INTEGER) {
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

    if (sq_gettype(v, 2) == OT_INTEGER && sq_gettype(v, 3) == OT_INTEGER) {
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

    if (sq_gettype(v, 2) == OT_INTEGER && sq_gettype(v, 3) == OT_INTEGER) {
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
    sq_pushinteger(v, ANativeWindow_getWidth(engine->app->window));
    return 1;
}

SQInteger emoGetWindowHeight(HSQUIRRELVM v) {
    sq_pushinteger(v, ANativeWindow_getHeight(engine->app->window));
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
    } else {

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
    } else {

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
    } else {

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
    } else {

    }

    sq_pushinteger(v, drawable->param_color[3]);
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
    drawable->animating   = false;
    
    if (drawable->loaded) {
        drawable->bindVertex();
    }

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

    drawable->animating = false;

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
    
    drawable->animating   = false;
    drawable->setFrameIndex(0);
    
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}
