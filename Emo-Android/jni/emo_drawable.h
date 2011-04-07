#include <squirrel.h>

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
SQInteger emoDrawableUnload(HSQUIRRELVM v);

