#import "squirrel.h"

void initDrawableFunctions();

SQInteger emoDrawableCreateSprite(HSQUIRRELVM v);
SQInteger emoDrawableCreateSpriteSheet(HSQUIRRELVM v);
SQInteger emoDrawableLoad(HSQUIRRELVM v);
SQInteger emoDrawableMove(HSQUIRRELVM v);
SQInteger emoDrawableScale(HSQUIRRELVM v);
SQInteger emoDrawableRotate(HSQUIRRELVM v);
SQInteger emoDrawableColor(HSQUIRRELVM v);
SQInteger emoDrawableRemove(HSQUIRRELVM v);
SQInteger emoSetOnDrawInterval(HSQUIRRELVM v);
SQInteger emoDrawableShow(HSQUIRRELVM v);
SQInteger emoDrawableHide(HSQUIRRELVM v);
SQInteger emoDrawableColorRed(HSQUIRRELVM v);
SQInteger emoDrawableColorGreen(HSQUIRRELVM v);
SQInteger emoDrawableColorBlue(HSQUIRRELVM v);
SQInteger emoDrawableColorAlpha(HSQUIRRELVM v);

SQInteger emoSetStageSize(HSQUIRRELVM v);
SQInteger emoSetViewport(HSQUIRRELVM v);
SQInteger emoGetWindowWidth(HSQUIRRELVM v);
SQInteger emoGetWindowHeight(HSQUIRRELVM v);
SQInteger emoDrawablePauseAt(HSQUIRRELVM v);
SQInteger emoDrawablePause(HSQUIRRELVM v);
SQInteger emoDrawableStop(HSQUIRRELVM v);
SQInteger emoDrawableAnimate(HSQUIRRELVM v);