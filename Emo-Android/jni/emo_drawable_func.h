#include <squirrel.h>

SQInteger emoDrawableCreateSprite(HSQUIRRELVM v);
SQInteger emoDrawableCreateSpriteSheet(HSQUIRRELVM v);
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
