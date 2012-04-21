// Copyright (c) 2011 emo-framework project
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the project nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
#include <../native_app_glue.h>

#include <squirrel.h>

#include "Constants.h"
#include "Engine.h"
#include "Drawable_glue.h"
#include "Runtime.h"
#include "VmFunc.h"
#include <rapidxml/rapidxml.hpp>

using namespace std;

extern emo::Engine* engine;

void initDrawableFunctions() {
    registerClass(engine->sqvm, EMO_STAGE_CLASS);

    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createSprite",     emoDrawableCreateSprite);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createFontSprite", emoDrawableCreateFontSprite);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createLine",       emoDrawableCreateLine);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createSpriteSheet",   emoDrawableCreateSpriteSheet);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createMapSprite",     emoDrawableCreateMapSprite);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createLiquidSprite",  emoDrawableCreateLiquidSprite);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createPointSprite",  emoDrawableCreatePointSprite);

    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "loadSprite",       emoDrawableLoad);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "loadMapSprite",    emoDrawableLoadMapSprite);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "addTileRow",       emoDrawableAddTileRow);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "clearTiles",       emoDrawableClearTiles);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "setTileAt",        emoDrawableSetTileAt);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getTileAt",        emoDrawableGetTileAt);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getTileIndexAtCoord",    emoDrawableGetTileIndexAtCoord);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getTilePositionAtCoord", emoDrawableGetTilePositionAtCoord);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "useMeshMapSprite", emoDrawableUseMeshMapSprite);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getMapData",       emoDrawableGetMapDataFromTmx);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "setFontSpriteParam",  emoDrawableSetFontSpriteParam);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "reloadFontSprite",  emoDrawableReloadFontSprite);

    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "createSnapshot",   emoDrawableCreateSnapshot);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "loadSnapshot",     emoDrawableLoadSnapshot);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "stopSnapshot",     emoDrawableDisableSnapshot);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "removeSnapshot",   emoDrawableRemoveSnapshot);

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
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "bgcolor",        emoSetStageColor);
    
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
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "isAnimationFinished", emoDrawableIsAnimationFinished);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getFrameIndex",  emoDrawableGetFrameIndex);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getFrameCount",  emoDrawableGetFrameCount);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "setLine",        emoDrawableSetLinePosition);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "selectFrame",    emoDrawableSelectFrame);

    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "updateLiquidTextureCoords",  emoDrawableUpdateLiquidTextureCoords);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "updateLiquidSegmentCoords",  emoDrawableUpdateLiquidSegmentCoords);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "updateLiquidSegmentCount",   emoDrawableUpdateLiquidSegmentCount);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getLiquidSegmentCount",      emoDrawableGetLiquidSegmentCount);

    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "updatePointDrawablePointCoords",  emoPointDrawableUpdatePointCoords);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "updatePointDrawablePointCount",   emoPointDrawableUpdatePointCount);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "getPointDrawablePointCount",      emoPointDrawableGetPointCount);
    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "isOffscreenSupported",            emoStageIsOffscreenSupported);

    registerClassFunc(engine->sqvm, EMO_STAGE_CLASS,    "blendFunc",      emoDrawableBlendFunc);
}

/*
 * create drawable instance (single sprite)
 * 
 * @param image file name (only png can be used)
 * @return drawable id
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
            drawable->needTexture = true;
        }
    } else {
        name = NULL;
    }

    int width  = 0;
    int height = 0;
    if (name != NULL && strlen(name) > 0) {
        if (!loadPngSizeFromAsset(name, &width, &height)) {
            delete drawable;
            return 0;
        }
    }
    drawable->width  = width;
    drawable->height = height;
    drawable->frameWidth  = width;
    drawable->frameHeight = height;

    char key[DRAWABLE_KEY_LENGTH];
    sprintf(key, "%ld%d-%d", 
                engine->uptime.time, engine->uptime.millitm, drawable->getCurrentBufferId());

    engine->addDrawable(key, drawable);

    sq_pushstring(v, key, strlen(key));

    return 1;
}

/*
 * create font drawable instance (single sprite)
 * 
 * @param property name that indicates the text string
 * @return drawable id
 */
SQInteger emoDrawableCreateFontSprite(HSQUIRRELVM v) {

    emo::FontDrawable* drawable = new emo::FontDrawable();
    drawable->setFrameCount(1);
    drawable->load();

    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);

        if (strlen(name) > 0) {
            drawable->name = name;
            drawable->needTexture = true;
        }
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    int fontSize = 0;
    
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        sq_getinteger(v, 3, &fontSize);
    }
    if (nargs >= 4 && sq_gettype(v, 4) == OT_STRING) {
        const SQChar* fontFace;
        sq_tostring(v, 4);
        sq_getstring(v, -1, &fontFace);

        if (strlen(fontFace) > 0) {
            drawable->fontFace = fontFace;
        }
    }
    if (nargs >= 5 && sq_gettype(v, 5) == OT_BOOL) {
        sq_getbool(v, 5, &drawable->isBold);
    }
    if (nargs >= 6 && sq_gettype(v, 6) == OT_BOOL) {
        sq_getbool(v, 6, &drawable->isItalic);
    }

    drawable->fontSize = fontSize;
    drawable->useFont  = true;

    char key[DRAWABLE_KEY_LENGTH];
    sprintf(key, "%ld%d-%d", 
                engine->uptime.time, engine->uptime.millitm, drawable->getCurrentBufferId());

    engine->addDrawable(key, drawable);

    sq_pushstring(v, key, strlen(key));

    return 1;
}

SQInteger emoDrawableCreateSnapshot(HSQUIRRELVM v) { 
    emo::SnapshotDrawable* drawable = new emo::SnapshotDrawable();
    
    drawable->setFrameCount(1);
    drawable->load();
    
    char key[DRAWABLE_KEY_LENGTH];
    sprintf(key, "%ld%d-%d", 
                engine->uptime.time, engine->uptime.millitm, drawable->getCurrentBufferId());

    engine->addDrawable(key, drawable);

    sq_pushstring(v, key, strlen(key));

    return 1;
}

/*
 * create liquid drawable instance (single sprite)
 * 
 * @param image file name (only png can be used)
 * @return drawable id
 */
SQInteger emoDrawableCreateLiquidSprite(HSQUIRRELVM v) {

    emo::LiquidDrawable* drawable = new emo::LiquidDrawable();

    drawable->setFrameCount(1);
    drawable->load();

    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);

        if (strlen(name) > 0) {
            drawable->name = name;
            drawable->needTexture = true;
        }
    } else {
        name = NULL;
    }

    int width  = 0;
    int height = 0;
    if (name != NULL && strlen(name) > 0) {
        if (!loadPngSizeFromAsset(name, &width, &height)) {
            delete drawable;
            return 0;
        }
    }
    drawable->width  = width;
    drawable->height = height;
    drawable->frameWidth  = width;
    drawable->frameHeight = height;

    char key[DRAWABLE_KEY_LENGTH];
    sprintf(key, "%ld%d-%d", 
                engine->uptime.time, engine->uptime.millitm, drawable->getCurrentBufferId());

    engine->addDrawable(key, drawable);

    sq_pushstring(v, key, strlen(key));

    return 1;
}

/*
 * create point drawable instance (single sprite)
 * 
 * @param image file name (only png can be used)
 * @return drawable id
 */
SQInteger emoDrawableCreatePointSprite(HSQUIRRELVM v) {

    emo::PointDrawable* drawable = new emo::PointDrawable();

    drawable->setFrameCount(1);
    drawable->load();

    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);

        if (strlen(name) > 0) {
            drawable->name = name;
            drawable->needTexture = true;
        }
    } else {
        name = NULL;
    }

    int width  = 0;
    int height = 0;
    if (name != NULL && strlen(name) > 0) {
        if (!loadPngSizeFromAsset(name, &width, &height)) {
            delete drawable;
            return 0;
        }
    }
    drawable->width  = width;
    drawable->height = height;
    drawable->frameWidth  = width;
    drawable->frameHeight = height;

    char key[DRAWABLE_KEY_LENGTH];
    sprintf(key, "%ld%d-%d", 
                engine->uptime.time, engine->uptime.millitm, drawable->getCurrentBufferId());

    engine->addDrawable(key, drawable);

    sq_pushstring(v, key, strlen(key));

    return 1;
}

/*
 * load snapshot drawable
 */
SQInteger emoDrawableLoadSnapshot(HSQUIRRELVM v) {
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
   
    drawable->width  = engine->stage->bufferWidth;
    drawable->height = engine->stage->bufferHeight;

    engine->enableOffscreen();
    engine->bindOffscreenFramebuffer();

    if (drawable->bindVertex()) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_CREATE_VERTEX);
    }
   
    return 1;
}

/*
 * remove snapshot drawable
 */
SQInteger emoDrawableRemoveSnapshot(HSQUIRRELVM v) { 
    engine->stopOffscreenRequested = false;
    engine->disableOffscreen();
    engine->stage->dirty = true;
    return emoDrawableRemove(v);
}

/*
 * disable snapshot
 */
SQInteger emoDrawableDisableSnapshot(HSQUIRRELVM v) { 
    if (!engine->useOffscreen || engine->stopOffscreenRequested) {
        sq_pushinteger(v, EMO_ERROR);
        return 1;
    }    
    
    engine->stopOffscreenRequested = true;
    
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * create line instance
 *
 * @param x1 start x
 * @param y1 start y
 * @param x2 end x
 * @param y2 end y
 * @return drawable id
 */
SQInteger emoDrawableCreateLine(HSQUIRRELVM v) {
	SQInteger nargs = sq_gettop(v);
	if (nargs < 5) {
		return 0;
	}
	
	emo::LineDrawable* drawable = new emo::LineDrawable();
	
    drawable->setFrameCount(1);
    drawable->load();
	
	SQFloat x1, y1, x2, y2;
	sq_getfloat(v, 2, &x1);
	sq_getfloat(v, 3, &y1);
	sq_getfloat(v, 4, &x2);
	sq_getfloat(v, 5, &y2);
	
	drawable->x = x1;
	drawable->y = y1;
	drawable->x2 = x2;
	drawable->y2 = y2;
	
	drawable->width  = 1;
	drawable->height = abs(y1 - y2);
	
    char key[DRAWABLE_KEY_LENGTH];
    sprintf(key, "%ld%d-%d", 
                engine->uptime.time, engine->uptime.millitm, drawable->getCurrentBufferId());

    engine->addDrawable(key, drawable);

    sq_pushstring(v, key, strlen(key));
	
    return 1;
}


/*
 * create drawable instance (sprite sheet)
 *
 * @param image file name
 * @param frame index
 * @param frame width
 * @param frame height
 * @param frame border
 * @param frame margin
 * @return drawable id
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
            drawable->needTexture = true;

            // if the filename ends with .xml, assumes texture is packed as atlas
            if (endsWith(name, ".xml")) {
                drawable->isPackedAtlas = true;
            }
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

    if (drawable->isPackedAtlas) {
        if (!drawable->loadPackedAtlasXml(frameIndex)) {
            delete drawable;
            return 0;
        }

        // retrieve image filename to load
        name = drawable->name.c_str();
    }

    int width  = 0;
    int height = 0;

    if (name != NULL && strlen(name) > 0) {
         if (!loadPngSizeFromAsset(name, &width, &height)) {
            delete drawable;
            return 0;
        }
    }

    drawable->hasSheet = true;

    if (!drawable->isPackedAtlas) {
        drawable->width  = frameWidth;
        drawable->height = frameHeight;
        drawable->border = border;
        drawable->frameWidth  = frameWidth;
        drawable->frameHeight = frameHeight;

        if (margin == 0 && border != 0) {
            drawable->margin = border;
        } else {
            drawable->margin = margin;
        }

        drawable->setFrameCount((int)floor(width / (float)(frameWidth  + border)) 
                                       * floor(height /(float)(frameHeight + border)));

    }
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

/*
 * Set parameters of FontSprite
 */
SQInteger emoDrawableSetFontSpriteParam(HSQUIRRELVM v) {
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

    emo::FontDrawable* fontDrawable = reinterpret_cast<emo::FontDrawable*>(drawable);

    if (nargs >= 3 && sq_gettype(v, 3) == OT_STRING) {
        const SQChar* param;
        sq_tostring(v, 3);
        sq_getstring(v, -1, &param);
        fontDrawable->param1 = param;
    }
    if (nargs >= 4 && sq_gettype(v, 4) == OT_STRING) {
        const SQChar* param;
        sq_tostring(v, 4);
        sq_getstring(v, -1, &param);
        fontDrawable->param2 = param;
    }
    if (nargs >= 5 && sq_gettype(v, 5) == OT_STRING) {
        const SQChar* param;
        sq_tostring(v, 5);
        sq_getstring(v, -1, &param);
        fontDrawable->param3 = param;
        sq_poptop(v);
    }
    if (nargs >= 6 && sq_gettype(v, 6) == OT_STRING) {
        const SQChar* param;
        sq_tostring(v, 6);
        sq_getstring(v, -1, &param);
        fontDrawable->param4 = param;
        sq_poptop(v);
    }
    if (nargs >= 7 && sq_gettype(v, 7) == OT_STRING) {
        const SQChar* param;
        sq_tostring(v, 7);
        sq_getstring(v, -1, &param);
        fontDrawable->param5 = param;
        sq_poptop(v);
    }
    if (nargs >= 8 && sq_gettype(v, 8) == OT_STRING) {
        const SQChar* param;
        sq_tostring(v, 8);
        sq_getstring(v, -1, &param);
        fontDrawable->param6 = param;
        sq_poptop(v);
    }

    sq_pushinteger(v, EMO_NO_ERROR);

    return 1;
}

/*
 * Reload parameter of FontSprite
 */
SQInteger emoDrawableReloadFontSprite(HSQUIRRELVM v) {
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

    const SQChar* name;
    if (nargs >= 3 && sq_gettype(v, 3) == OT_STRING) {
        sq_tostring(v, 3);
        sq_getstring(v, -1, &name);

        engine->removeCachedImage(drawable->name);
        engine->addCachedImage(name, drawable->getTexture());

        drawable->name = name;
    }

    drawable->deleteBuffer(true);
    drawable->getTexture()->clearTexture();

    engine->javaGlue->loadTextBitmap(drawable, drawable->getTexture(), true);

    drawable->width  = drawable->getTexture()->width;
    drawable->height = drawable->getTexture()->height;
    drawable->frameWidth  = drawable->width;
    drawable->frameHeight = drawable->height;

    // calculate the size of power of two
    drawable->getTexture()->glWidth  = nextPowerOfTwo(drawable->width);
    drawable->getTexture()->glHeight = nextPowerOfTwo(drawable->height);

    drawable->getTexture()->genTextures();

    drawable->reload();
    drawable->bindVertex();

    sq_pushinteger(v, EMO_NO_ERROR);

    return 1;
}

/*
 * load drawable
 *
 * @param drawable id
 * @param drawable x
 * @param drawable y
 * @param drawable width
 * @param drawable height
 * @return EMO_NO_ERROR if loading succeeds, otherwise returns error code
 */
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
        } else if (drawable->useFont) {
            image = new emo::Image();
            if (engine->javaGlue->loadTextBitmap(drawable, image, true)) {

                drawable->width  = image->width;
                drawable->height = image->height;
                drawable->frameWidth  = image->width;
                drawable->frameHeight = image->height;

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
        } else {
            image = new emo::Image();
            if (loadPngFromAsset(drawable->name.c_str(), image, true)) {

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

/*
 * create map sprite
 *
 * @param sprite sheet id
 * @return map sprite id
 */
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

/*
 * load map sprite
 *
 * @param map sprite id 
 * @param map x
 * @param map y
 * @param map width
 * @param map height
 * @param EMO_NO_ERROR if loading scceeds, otherwise returns error code
 */
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
            if (loadPngFromAsset(drawable->name.c_str(), image, true)) {

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

    if (parent->width  <= 0) parent->width  = width;
    if (parent->height <= 0) parent->height = height;

    if (parent->bindVertex()) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_CREATE_VERTEX);
    }

    return 1;
}

/*
 * add tile row to map sprite
 *
 * @param map sprite id
 * @param map row to be added
 * @returns EMO_NO_ERROR if succeeds
 */
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

/*
 * clear tiles from map sprite
 *
 * @param map sprite id
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * set tile value at given index
 *
 * @param map sprite id
 * @param tile row
 * @param tile column
 * @param tile value
 * @param EMO_NO_ERROR if succeeds
 */
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

        if (!drawable->setTileAt(row, column, value)) {
            sq_pushinteger(v, ERR_INVALID_PARAM);
            return 1;
        }
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * get tile value from given index
 *
 * @param map sprite id
 * @param tile row
 * @param tile column
 * @return tile value from given index
 */
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

/*
 * get tile index at given coords
 *
 * @param map sprite id
 * @param coord x
 * @param coord y
 * @returns tile index array[x, y]
 */
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
    } else {
        return 0;
    }

    return 1;
}

/*
 * get tile position at given coords
 *
 * @param map sprite id
 * @param coord x
 * @param coord y
 * @returns tile position array[x, y]
 */
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
    } else {
        return 0;
    }

    return 1;
}

/*
 * use mesh map sprite
 * mesh map sprite draws map as mesh (which does not loop each tiles)
 *
 * @param map sprite id
 * @param use mesh or not
 */
SQInteger emoDrawableUseMeshMapSprite(HSQUIRRELVM v) {
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
    
    emo::Drawable* parent = engine->getDrawable(id);
    
    if (parent == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
    
    SQBool useMesh = false;
    
    if (getBool(v, 3, &useMesh)) {
        sq_pushinteger(v, EMO_NO_ERROR);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
    
    parent->useMesh = useMesh;
    
    return 1;
}

/*
 * get map data from ".tmx" file (XML).
 *
 * @param id tmx file name
 * @returns EMO_NO_ERROR if succeeds
 */
SQInteger emoDrawableGetMapDataFromTmx(HSQUIRRELVM v) {
    const char *delim = ",";
    const char *LF = "\n";

    const SQChar* id;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &id);
        sq_poptop(v);
    } else {
        return 0;
    }

    string fname = id;
    // check if the length is shorter than the length of ".xml"
    if (fname.length() <= 4) return false;
    std::string data = loadContentFromAsset(fname);

    rapidxml::xml_document<char> doc;
    doc.parse<0>((char*)data.c_str());

    // retrieving map node
    rapidxml::xml_node<> * mapnode = doc.first_node("map");

    rapidxml::xml_attribute<> * ch = NULL;

    sq_pushroottable(v);

    sq_pushstring(v, "mapdata", -1);
    sq_newtable(v);

    // create tileset array
    sq_pushstring(v, "tileset", -1); //2
    sq_newarray(v, 0);

    // retrieving tileset node
    for (rapidxml::xml_node<> * tilesetnode = mapnode->first_node("tileset");
        tilesetnode; tilesetnode = tilesetnode->next_sibling("tileset")) {

        sq_newtable(v);

        ch = tilesetnode->first_attribute("firstgid");
        sq_pushstring(v, "firstgid", -1);
        sq_pushstring(v, ch->value(), -1);
        sq_newslot(v, -3, SQFalse);

        ch = tilesetnode->first_attribute("name");
        sq_pushstring(v, "name", -1);
        sq_pushstring(v, ch->value(), -1);
        sq_newslot(v, -3, SQFalse);

        ch = tilesetnode->first_attribute("tilewidth");
        sq_pushstring(v, "tilewidth", -1);
        sq_pushinteger(v, atoi(ch->value()));
        sq_newslot(v, -3, SQFalse);

        ch = tilesetnode->first_attribute("tileheight");
        sq_pushstring(v, "tileheight", -1);
        sq_pushinteger(v, atoi(ch->value()));
        sq_newslot(v, -3, SQFalse);

        // retrieving image node
        rapidxml::xml_node<> * imagenode = tilesetnode->first_node("image");
        ch = imagenode->first_attribute("source");
        sq_pushstring(v, "source", -1);
        sq_pushstring(v, ch->value(), -1);
        sq_newslot(v, -3, SQFalse);

        ch = imagenode->first_attribute("trans");
        sq_pushstring(v, "trans", -1);
        sq_pushstring(v, ch->value(), -1);
        sq_newslot(v, -3, SQFalse);

        ch = imagenode->first_attribute("width");
        sq_pushstring(v, "width", -1);
        sq_pushinteger(v, atoi(ch->value()));
        sq_newslot(v, -3, SQFalse);

        ch = imagenode->first_attribute("height");
        sq_pushstring(v, "height", -1);
        sq_pushinteger(v, atoi(ch->value()));
        sq_newslot(v, -3, SQFalse);

        sq_arrayappend(v, -2);
    }
    sq_newslot(v, -3, SQFalse);

    // create layer array
    sq_pushstring(v, "layer", -1); //2
    sq_newarray(v, 0);

    // retrieving layer node
    for (rapidxml::xml_node<> * layernode = mapnode->first_node("layer");
                     layernode; layernode = layernode->next_sibling("layer")) {
        sq_newtable(v);

        ch = layernode->first_attribute("name");
        sq_pushstring(v, "name", -1);
        sq_pushstring(v, ch->value(), -1);
        sq_newslot(v, -3, SQFalse);

        ch = layernode->first_attribute("width");
        sq_pushstring(v, "width", -1);
        sq_pushinteger(v, atoi(ch->value()));
        sq_newslot(v, -3, SQFalse);

        ch = layernode->first_attribute("height");
        sq_pushstring(v, "height", -1);
        sq_pushinteger(v, atoi(ch->value()));
        sq_newslot(v, -3, SQFalse);

        // retrieving data node
        rapidxml::xml_node<> * datanode = layernode->first_node("data");

        sq_pushstring(v, "data", -1);
        sq_newarray(v, 0);

        vector <string> lines = split(datanode->value(), LF);
        for(unsigned int i = 0; i < lines.size() ; i++){
            vector <string> chNums = split(lines[i], delim);
            if(chNums.size() == 1) continue;// ignore empty lines

            sq_newarray(v, 0);
            for(unsigned int j = 0 ; j < chNums.size() ; j++){
                if(chNums[j].size() == 0) continue;
                sq_pushinteger(v, atoi(chNums[j].c_str()) - 1 ); // -1 for emo-framework
                sq_arrayappend(v, -2);
            }
            sq_arrayappend(v, -2);
        }
        sq_newslot(v, -3, SQFalse);

        sq_arrayappend(v, -2);
    }
    sq_newslot(v, -3, SQFalse);

    return 1;
}

/*
 * move drawable
 *
 * @param drawable id
 * @param x
 * @param y
 * @param z
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
 * 
 * @param drawable id
 * @param red    (0 to 1)
 * @param green  (0 to 1)
 * @param blue   (0 to 1)
 * @param alpha  (0 to 1)
 * @return EMO_NO_ERROR if succeeds
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
 * 
 * @param drawable id
 * @param scale x
 * @param scale y
 * @param center x
 * @param center y
 * @return EMO_NO_ERROR if succeeds
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
 *
 * @param drawable id
 * @param rotate angle
 * @param center x
 * @param center y
 * @param rotate axis
 * @return EMO_NO_ERROR if succeeds
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
 * 
 * @param drawable id
 * @return EMO_NO_ERROR is succeeds, otherwise returns error code
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
 *
 * @param onDraw interval (msec)
 * @return previous interval
 */
SQInteger emoSetOnDrawInterval(HSQUIRRELVM v) {
    SQInteger oldInterval = engine->onDrawDrawablesInterval;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        SQInteger interval;
        sq_getinteger(v, 2, &interval);
        engine->setOnDrawDrawablesInterval(interval);
    }
    sq_pushinteger(v, oldInterval);
	
	return 1;
}

/*
 * set viewport
 *
 * @param viewport width
 * @param viewport height
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * set stage size
 *
 * @param stage width
 * @param stage height
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * set stage color
 *
 * @param red   (0 to 1)
 * @param green (0 to 1)
 * @param blue  (0 to 1)
 * @param alpha (0 to 1)
 * @return EMO_NO_ERROR if succeeds
 */
SQInteger emoSetStageColor(HSQUIRRELVM v) {
    float red   = engine->stage->color[0];
    float green = engine->stage->color[1];
    float blue  = engine->stage->color[2];
    float alpha = engine->stage->color[3];

    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getfloat(v, 2, &red);
    }
    if (sq_gettype(v, 3) != OT_NULL) {
        sq_getfloat(v, 3, &green);
    }
    if (sq_gettype(v, 4) != OT_NULL) {
        sq_getfloat(v, 4, &blue);
    }
    if (sq_gettype(v, 5) != OT_NULL) {
        sq_getfloat(v, 5, &alpha);
    }

    engine->stage->color[0] = red;
    engine->stage->color[1] = green;
    engine->stage->color[2] = blue;
    engine->stage->color[3] = alpha;

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * returns stage width
 */
SQInteger emoGetWindowWidth(HSQUIRRELVM v) {
    sq_pushinteger(v, engine->stage->width);
    return 1;
}

/*
 * returns stage height
 */
SQInteger emoGetWindowHeight(HSQUIRRELVM v) {
    sq_pushinteger(v, engine->stage->height);
    return 1;
}

/*
 * show a drawable with given id
 * this function set alpha color to 1
 *
 * @param drawable id
 */
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

/*
 * hide a drawable with given id
 * this function set alpha color to 0
 *
 * @param drawable id
 */
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

/*
 * set drawable color (red)
 *
 * @param drawable id
 * @param red color (0 to 1)
 */
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

/*
 * set drawable color (green)
 *
 * @param drawable id
 * @param green color (0 to 1)
 */
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

/*
 * set drawable color (blue)
 *
 * @param drawable id
 * @param blue color (0 to 1)
 */
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

/*
 * set drawable alpha color
 *
 * @param drawable id
 * @param alpha color (0 to 1)
 */
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

/*
 * set drawable position x
 * 
 * @param drawable id
 * @param x
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * set drawable position y
 * 
 * @param drawable id
 * @param y
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * set drawable position z
 * 
 * @param drawable id
 * @param z
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * set drawable width
 * 
 * @param drawable id
 * @param width
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * set drawable height
 * 
 * @param drawable id
 * @param height
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * set drawable size
 * 
 * @param drawable id
 * @param width
 * @param height
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * pause spritesheet at given position
 *
 * @param drawable id
 * @param index
 * @return EMO_NO_ERROR if succeeds
 */
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

    if (!drawable->loaded) {
        LOGW("It is recommended not to use SpriteSheet#setFrame BEFORE the sprite has been loaded by load().");
    }

    drawable->setFrameIndex(index);
    drawable->enableAnimation(false);

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * select frame that has given name
 */
SQInteger emoDrawableSelectFrame(HSQUIRRELVM v) {

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
    
    const SQChar* frame_name;
    if (nargs >= 3 && sq_gettype(v, 3) == OT_STRING) {
        sq_tostring(v, 3);
        sq_getstring(v, -1, &frame_name);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    if (!drawable->loaded) {
        LOGW("It is recommended not to use SpriteSheet#selectFrame BEFORE the sprite has been loaded by load().");
    }

    if (!drawable->selectFrame(frame_name)) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * pause spritesheet at given position
 *
 * @param drawable id
 * @param index
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * stop sprite animation
 * animation frame index is set to 0
 * 
 * @param drawable id
 */
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

/*
 * animate sprite sheet
 * 
 * @param drawable id
 * @param frame start index
 * @param frame count from start index
 * @param animation interval (msec)
 * @param loop count
 * @return EMO_NO_ERROR if succeeds
 */
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
    
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL && sq_gettype(v, 3) != OT_ARRAY) {
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

    // if individual animation frames are specified, use these values
    if (nargs >= 3 && sq_gettype(v, 3) == OT_ARRAY) {
        count = sq_getsize(v, 3);

        animation->count = count;
        animation->initializeFrames();

        sq_pushnull(v);
        int idx = 0; 
        while(SQ_SUCCEEDED(sq_next(v, -nargs+1))) {
            if (idx >= count) break;
            if (sq_gettype(v, -1) == OT_INTEGER) {
                SQInteger value;
                sq_getinteger(v, -1, &value);
                animation->setFrame(idx, value);
            } else if(sq_gettype(v, -1) == OT_STRING) {
                const SQChar *value;
                sq_getstring(v, -1, &value);
                emo::ImagePackInfo* info = drawable->getImagePack(value);
                if (info == NULL) continue;
                animation->setFrame(idx, info->index);
            }
            idx++;
            sq_pop(v, 2);
        }    

        sq_pop(v, 1);

        if (animation->count > 0) animation->start = animation->frames[0];
    } 
    
    drawable->addAnimation(animation), 
    drawable->setAnimation(animation->name);
    drawable->enableAnimation(true);
    
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * returns whether animation is finished or not
 *
 * @param drawable id
 * @return animation is finished or not
 */
SQInteger emoDrawableIsAnimationFinished(HSQUIRRELVM v) {

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
    
    sq_pushbool(v, drawable->isAnimationFinished());
    return 1;
}

/*
 * returns drawable position x
 *
 * @param drawable id
 * @return drawable position x
 */
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

/*
 * returns drawable position y
 *
 * @param drawable id
 * @return drawable position y
 */
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

/*
 * returns drawable position z
 *
 * @param drawable id
 * @return drawable position z
 */
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

/*
 * returns drawable width
 *
 * @param drawable id
 * @return drawable width
 */
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

/*
 * returns drawable height
 *
 * @param drawable id
 * @return drawable height
 */
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

/*
 * returns drawable scale x
 *
 * @param drawable id
 * @return drawable scale x
 */
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

/*
 * returns drawable scale y
 *
 * @param drawable id
 * @return drawable scale y
 */
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

/*
 * returns drawable angle
 *
 * @param drawable id
 * @return drawable angle
 */
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

/*
 * returns spritesheet frame index
 *
 * @param drawable id
 * @return spritesheet frame index
 */
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

/*
 * returns spritesheet frame count
 *
 * @param drawable id
 * @return spritesheet frame count
 */
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

/*
 * set line position
 *
 * @param drawable id
 * @param x1 start x
 * @param y1 start y
 * @param x2 end x
 * @param y2 end y
 * @return EMO_NO_ERROR if succeeds
 */
SQInteger emoDrawableSetLinePosition(HSQUIRRELVM v) {
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
	
    emo::LineDrawable* drawable = (emo::LineDrawable*)engine->getDrawable(id);
	
    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (sq_gettype(v, 3) != OT_NULL) {
        SQFloat x1;
        sq_getfloat(v, 3, &x1);
        drawable->x = x1;
    }
    if (sq_gettype(v, 4) != OT_NULL) {
        SQFloat y1;
        sq_getfloat(v, 4, &y1);
        drawable->y = y1;
    }
    if (sq_gettype(v, 5) != OT_NULL) {
        SQFloat x2;
        sq_getfloat(v, 5, &x2);
        drawable->x2 = x2;
    }
    if (sq_gettype(v, 6) != OT_NULL) {
        SQFloat y2;
        sq_getfloat(v, 6, &y2);
        drawable->y2 = y2;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableUpdateLiquidSegmentCount(HSQUIRRELVM v) {
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
	
    emo::LiquidDrawable* drawable = reinterpret_cast<emo::LiquidDrawable*>(engine->getDrawable(id));
	
    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
    
    if (drawable->loaded) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQInteger count;
        sq_getinteger(v, 3, &count);
        drawable->updateSegmentCount(count);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableGetLiquidSegmentCount(HSQUIRRELVM v) {
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
	
    emo::LiquidDrawable* drawable = reinterpret_cast<emo::LiquidDrawable*>(engine->getDrawable(id));
	
    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
    
    sq_pushinteger(v, drawable->segmentCount);
    return 1;
}

SQInteger emoDrawableUpdateLiquidTextureCoords(HSQUIRRELVM v) {
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
	
    emo::LiquidDrawable* drawable = reinterpret_cast<emo::LiquidDrawable*>(engine->getDrawable(id));
	
    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) == OT_ARRAY) {
        int size = sq_getsize(v, 3);
        
        for (int i = 0; i < size; i++) {
            sq_pushinteger(v, i);
            sq_get(v, 3);
            
            float vx = 0;
            float vy = 0;
            getInstanceMemberAsFloat(v, sq_gettop(v), "x", &vx);
            getInstanceMemberAsFloat(v, sq_gettop(v), "y", &vy);
            
            drawable->updateTextureCoords(i, vx, vy);
            
            sq_pop(v, 1);
        }
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoDrawableUpdateLiquidSegmentCoords(HSQUIRRELVM v) {
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
	
    emo::LiquidDrawable* drawable = reinterpret_cast<emo::LiquidDrawable*>(engine->getDrawable(id));
	
    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) == OT_ARRAY) {
        int size = sq_getsize(v, 3);
        
        for (int i = 0; i < size; i++) {
            sq_pushinteger(v, i);
            sq_get(v, 3);
            
            float vx = 0;
            float vy = 0;
            getInstanceMemberAsFloat(v, sq_gettop(v), "x", &vx);
            getInstanceMemberAsFloat(v, sq_gettop(v), "y", &vy);
            
            drawable->updateSegmentCoords(i, vx, vy);

            sq_pop(v, 1);
        }
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoPointDrawableUpdatePointCount(HSQUIRRELVM v) {
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
	
    emo::PointDrawable* drawable = reinterpret_cast<emo::PointDrawable*>(engine->getDrawable(id));
	
    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
    
    if (drawable->loaded) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        SQInteger count;
        sq_getinteger(v, 3, &count);
        drawable->updatePointCount(count);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

SQInteger emoPointDrawableGetPointCount(HSQUIRRELVM v) {
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
	
    emo::PointDrawable* drawable = reinterpret_cast<emo::PointDrawable*>(engine->getDrawable(id));
	
    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
    
    sq_pushinteger(v, drawable->pointCount);
    return 1;
}

SQInteger emoPointDrawableUpdatePointCoords(HSQUIRRELVM v) {
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
	
    emo::PointDrawable* drawable = reinterpret_cast<emo::PointDrawable*>(engine->getDrawable(id));
	
    if (drawable == NULL) {
        sq_pushinteger(v, ERR_INVALID_ID);
        return 1;
    }
	
    if (nargs >= 3 && sq_gettype(v, 3) == OT_ARRAY) {
        int size = sq_getsize(v, 3);
        
        for (int i = 0; i < size; i++) {
            sq_pushinteger(v, i);
            sq_get(v, 3);
            
            float vx = 0;
            float vy = 0;
            getInstanceMemberAsFloat(v, sq_gettop(v), "x", &vx);
            getInstanceMemberAsFloat(v, sq_gettop(v), "y", &vy);
            
            drawable->updatePointCoords(i, vx, vy);
            
            sq_pop(v, 1);
        }
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * returns whether offscreen is supported or not.
 */
SQInteger emoStageIsOffscreenSupported(HSQUIRRELVM v) {
    sq_pushbool(v, engine->canUseOffscreen);
    return 1;
}

/*
 * Set blend function of the sprite
 */
SQInteger emoDrawableBlendFunc(HSQUIRRELVM v) {
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
    
    if (nargs >= 4 && sq_gettype(v, 3) != OT_NULL && sq_gettype(v, 4) != OT_NULL) {
        SQInteger src, dst;
        sq_getinteger(v, 3, &src);
        sq_getinteger(v, 4, &dst);
        
        drawable->srcBlendFactor = src;
        drawable->dstBlendFactor = dst;
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
    
    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

