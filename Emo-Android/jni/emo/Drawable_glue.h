#ifndef EMO_DRAWABLE_GLUE_H
#define EMO_DRAWABLE_GLUE_H

#include <squirrel.h>

void initDrawableFunctions();

SQInteger emoDrawableCreateSprite(HSQUIRRELVM v);
SQInteger emoDrawableCreateSpriteSheet(HSQUIRRELVM v);
SQInteger emoDrawableCreateMapSprite(HSQUIRRELVM v);
SQInteger emoDrawableLoadMapSprite(HSQUIRRELVM v);
SQInteger emoDrawableAddTileRow(HSQUIRRELVM v);
SQInteger emoDrawableClearTiles(HSQUIRRELVM v);
SQInteger emoDrawableSetTileAt(HSQUIRRELVM v);
SQInteger emoDrawableGetTileAt(HSQUIRRELVM v);
SQInteger emoDrawableGetTileIndexAtCoord(HSQUIRRELVM v);
SQInteger emoDrawableGetTilePositionAtCoord(HSQUIRRELVM v);
SQInteger emoDrawableLoad(HSQUIRRELVM v);
SQInteger emoDrawableMove(HSQUIRRELVM v);
SQInteger emoDrawableScale(HSQUIRRELVM v);
SQInteger emoDrawableRotate(HSQUIRRELVM v);
SQInteger emoDrawableColor(HSQUIRRELVM v);
SQInteger emoDrawableRemove(HSQUIRRELVM v);
SQInteger emoSetOnDrawInterval(HSQUIRRELVM v);
SQInteger emoSetViewport(HSQUIRRELVM v);
SQInteger emoSetStageSize(HSQUIRRELVM v);
SQInteger emoGetWindowWidth(HSQUIRRELVM v);
SQInteger emoGetWindowHeight(HSQUIRRELVM v);
SQInteger emoDrawableShow(HSQUIRRELVM v);
SQInteger emoDrawableHide(HSQUIRRELVM v);

SQInteger emoDrawableColorRed(HSQUIRRELVM v);
SQInteger emoDrawableColorGreen(HSQUIRRELVM v);
SQInteger emoDrawableColorBlue(HSQUIRRELVM v);
SQInteger emoDrawableColorAlpha(HSQUIRRELVM v);

SQInteger emoDrawablePauseAt(HSQUIRRELVM v);
SQInteger emoDrawablePause(HSQUIRRELVM v);
SQInteger emoDrawableStop(HSQUIRRELVM v);
SQInteger emoDrawableAnimate(HSQUIRRELVM v);

SQInteger emoDrawableGetX(HSQUIRRELVM v);
SQInteger emoDrawableGetY(HSQUIRRELVM v);
SQInteger emoDrawableGetZ(HSQUIRRELVM v);
SQInteger emoDrawableGetWidth(HSQUIRRELVM v);
SQInteger emoDrawableGetHeight(HSQUIRRELVM v);

SQInteger emoDrawableGetScaleX(HSQUIRRELVM v);
SQInteger emoDrawableGetScaleY(HSQUIRRELVM v);
SQInteger emoDrawableGetAngle(HSQUIRRELVM v);

SQInteger emoDrawableSetX(HSQUIRRELVM v);
SQInteger emoDrawableSetY(HSQUIRRELVM v);
SQInteger emoDrawableSetZ(HSQUIRRELVM v);
SQInteger emoDrawableSetWidth(HSQUIRRELVM v);
SQInteger emoDrawableSetHeight(HSQUIRRELVM v);
SQInteger emoDrawableSetSize(HSQUIRRELVM v);
#endif
