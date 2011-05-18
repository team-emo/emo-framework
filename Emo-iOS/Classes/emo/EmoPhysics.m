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
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_setAutoClearForces", emoPhysicsWorld_SetAutoClearForces);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_getAutoClearForces", emoPhysicsWorld_GetAutoClearForces);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "polygonShape_set",   emoPhysicsPolygonShape_Set);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "polygonShape_setAsBox",   emoPhysicsPolygonShape_SetAsBox);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "polygonShape_setAsEdge",   emoPhysicsPolygonShape_SetAsEdge);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "polygonShape_getVertex",   emoPhysicsPolygonShape_GetVertex);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "polygonShape_getVertexCount",   emoPhysicsPolygonShape_GetVertexCount);
}