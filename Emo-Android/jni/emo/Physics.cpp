#include "Physics.h"
#include "VmFunc.h"
#include "Engine.h"
#include "Physics_glue.h"

extern emo::Engine* engine;

void initPhysicsFunctions() {
    registerClass(engine->sqvm, EMO_PHYSICS_CLASS);
    registerClassFunc(engine->sqvm, EMO_PHYSICS_CLASS, "newWorld",  emoPhysicsNewWorld);
    registerClassFunc(engine->sqvm, EMO_PHYSICS_CLASS, "deleteWorld",  emoPhysicsDeleteObj);
    registerClassFunc(engine->sqvm, EMO_PHYSICS_CLASS, "newShape",  emoPhysicsNewShape);
}
