#import "EmoPhysics.h"
#import "Physics_glue.h"
#import "VmFunc.h"
#import "EmoEngine.h"

extern EmoEngine* engine;

void initPhysicsFunctions() {
    registerClass(engine.sqvm,     EMO_PHYSICS_CLASS);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "newWorld",      emoPhysicsNewWorld);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "newShape",      emoPhysicsNewShape);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "createBody",    emoPhysicsNewShape);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "destroyBody",   emoPhysicsNewShape);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "createJoint",   emoPhysicsNewShape);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "destroyJoint",  emoPhysicsNewShape);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_step",    emoPhysicsNewShape);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_clearForces", emoPhysicsNewShape);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "createFixture", emoPhysicsNewShape);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "destroyFixture",emoPhysicsNewShape);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "newJointDef",   emoPhysicsNewShape);
}