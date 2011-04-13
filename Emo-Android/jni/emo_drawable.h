#include <squirrel.h>


bool clearGLErrors(const char* msg);
bool printGLErrors(const char* msg);
bool loadStage(struct Stage* stage);
void loadDrawables(struct engine* engine);

void rebindStageBuffers(struct Stage* stage);
void rebindDrawableBuffers(struct engine* engine);

void unloadDrawables(struct engine* engine);
bool deleteStageBuffer(struct Stage* stage);
bool deleteDrawableBuffers(struct engine* engine);

void onDrawDrawables(struct engine* engine);
void onDrawStage(struct Stage* stage);

struct Drawable* getDrawable(const char* key, struct engine* engine);

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
