#include <squirrel.h>


bool clearGLErrors(const char* msg);
bool printGLErrors(const char* msg);
bool loadStage(struct Stage* stage);
bool unloadStage(struct Stage* stage);
void unloadDrawables(struct engine* engine);
void loadDrawables(struct engine* engine);

void onDrawDrawables(struct engine* engine);
void onDrawStage(struct Stage* stage);

void clearDrawables(struct engine* engine);
bool removeDrawable(const char* key, struct engine* engine);
struct Drawable* getDrawable(const char* key, struct engine* engine);
void addDrawable(const char* key, struct Drawable* drawable, struct engine* engine);
void updateDrawableBuffers(struct Drawable* drawable);
void initDrawable(struct Drawable* drawable);
void updateDrawableKey(struct Drawable* drawable, char* key);

SQInteger emoDrawableCreateSprite(HSQUIRRELVM v);
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
