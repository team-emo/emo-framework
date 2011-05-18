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
	
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_setTransform",   emoPhysicsBody_SetTransform);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getPosition",   emoPhysicsBody_GetPosition);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getAngle",   emoPhysicsBody_GetAngle);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getWorldCenter",   emoPhysicsBody_GetWorldCenter);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getLocalCenter",   emoPhysicsBody_GetLocalCenter);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_setLinearVelocity",   emoPhysicsBody_SetLinearVelocity);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getLinearVelocity",   emoPhysicsBody_GetLinearVelocity);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_setAngularVelocity",   emoPhysicsBody_SetAngularVelocity);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getAngularVelocity",   emoPhysicsBody_GetAngularVelocity);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_applyForce",   emoPhysicsBody_ApplyForce);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_applyTorque",   emoPhysicsBody_ApplyTorque);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_applyLinearImpulse",   emoPhysicsBody_ApplyLinearImpulse);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_applyAngularImpulse",   emoPhysicsBody_ApplyAngularImpulse);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getMass",   emoPhysicsBody_GetMass);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getInertia",   emoPhysicsBody_GetInertia);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getWorldPoint",   emoPhysicsBody_GetWorldPoint);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getWorldVector",   emoPhysicsBody_GetWorldVector);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getLocalPoint",   emoPhysicsBody_GetLocalPoint);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getLocalVector",   emoPhysicsBody_GetLocalVector);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getLinearVelocityFromWorldPoint",   emoPhysicsBody_GetLinearVelocityFromWorldPoint);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getLinearVelocityFromLocalPoint",   emoPhysicsBody_GetLinearVelocityFromLocalPoint);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getLinearDamping",   emoPhysicsBody_GetLinearDamping);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_setLinearDamping",   emoPhysicsBody_SetLinearDamping);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getAngularDamping",   emoPhysicsBody_GetAngularDamping);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_setAngularDamping",   emoPhysicsBody_SetAngularDamping);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_setType",   emoPhysicsBody_SetType);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_getType",   emoPhysicsBody_GetType);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_setBullet",   emoPhysicsBody_SetBullet);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_isBullet",   emoPhysicsBody_IsBullet);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_setSleepingAllowed",   emoPhysicsBody_SetSleepingAllowed);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_isSleepingAllowed",   emoPhysicsBody_IsSleepingAllowed);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_setAwake",   emoPhysicsBody_SetAwake);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_isAwake",   emoPhysicsBody_IsAwake);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_setActive",   emoPhysicsBody_SetActive);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_isActive",   emoPhysicsBody_IsActive);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_setFixedRotation",   emoPhysicsBody_SetFixedRotation);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "body_isFixedRotation",   emoPhysicsBody_IsFixedRotation);
}