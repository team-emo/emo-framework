#include <squirrel.h>

bool clearGLErrors(const char* msg);
bool printGLErrors(const char* msg);
bool loadStage(Stage* stage);

void addDrawable(const char* _key, Drawable* drawable, engine* engine);
bool removeDrawable(const char* key, engine* engine);

void initDrawable(Drawable* drawable);
void loadDrawable(Drawable* drawable);
void loadDrawables(engine* engine);

bool bindDrawableVertex(Drawable* drawable);

void rebindStageBuffers(Stage* stage);
void rebindDrawableBuffers(engine* engine);

void unloadDrawables(engine* engine);
bool deleteStageBuffer(Stage* stage);
bool deleteDrawableBuffers(engine* engine);

void onDrawDrawables(engine* engine);
void onDrawStage(Stage* stage);

Drawable* getDrawable(const char* key, engine* engine);
