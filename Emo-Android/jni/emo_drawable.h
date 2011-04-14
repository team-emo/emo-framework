#include <squirrel.h>

bool clearGLErrors(const char* msg);
bool printGLErrors(const char* msg);
bool loadStage(struct Stage* stage);

void addDrawable(const char* _key, struct Drawable* drawable, struct engine* engine);
bool removeDrawable(const char* key, struct engine* engine);

void initDrawable(struct Drawable* drawable);
void loadDrawable(struct Drawable* drawable);
void loadDrawables(struct engine* engine);

bool bindDrawableVertex(struct Drawable* drawable);

void rebindStageBuffers(struct Stage* stage);
void rebindDrawableBuffers(struct engine* engine);

void unloadDrawables(struct engine* engine);
bool deleteStageBuffer(struct Stage* stage);
bool deleteDrawableBuffers(struct engine* engine);

void onDrawDrawables(struct engine* engine);
void onDrawStage(struct Stage* stage);

struct Drawable* getDrawable(const char* key, struct engine* engine);
