#import "EmoPhysics.h"
#import "Physics_glue.h"
#import "VmFunc.h"
#import "EmoEngine.h"

extern EmoEngine* engine;

void initPhysicsFunctions() {
    registerClass(engine.sqvm,     EMO_PHYSICS_CLASS);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "newWorld",      emoPhysicsNewWorld);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "newShape",      emoPhysicsNewShape);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "createBody",    emoPhysicsCreateBody);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "destroyBody",   emoPhysicsDestroyBody);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "createJoint",   emoPhysicsCreateJoint);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "destroyJoint",  emoPhysicsDestroyJoint);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_step",    emoPhysicsWorld_Step);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_clearForces", emoPhysicsWorld_ClearForces);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "createFixture", emoPhysicsCreateFixture);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "destroyFixture",emoPhysicsDestroyFixture);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "newJointDef",   emoPhysicsNewJointDef);
}