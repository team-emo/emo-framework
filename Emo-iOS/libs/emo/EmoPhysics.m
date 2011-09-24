// Copyright (c) 2011 emo-framework project
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the project nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
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
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "createGroundBody", emoPhysicsCreateGroundBody);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "destroyBody",   emoPhysicsDestroyBody);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "createJoint",   emoPhysicsCreateJoint);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "destroyJoint",  emoPhysicsDestroyJoint);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_step",    emoPhysicsWorld_Step);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_enableContactListener", emoPhysicsWorld_EnableContactListener);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_enableContactState",    emoPhysicsWorld_EnableContactState);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_clearForces", emoPhysicsWorld_ClearForces);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "createFixture", emoPhysicsCreateFixture);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "destroyFixture",emoPhysicsDestroyFixture);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "newJointDef",   emoPhysicsNewJointDef);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_setAutoClearForces", emoPhysicsWorld_SetAutoClearForces);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_getAutoClearForces", emoPhysicsWorld_GetAutoClearForces);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_setGravity",   emoPhysicsWorld_SetGravity);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "world_getGravity",   emoPhysicsWorld_GetGravity);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "polygonShape_set",   emoPhysicsPolygonShape_Set);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "polygonShape_setAsBox",   emoPhysicsPolygonShape_SetAsBox);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "polygonShape_setAsEdge",   emoPhysicsPolygonShape_SetAsEdge);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "polygonShape_getVertex",   emoPhysicsPolygonShape_GetVertex);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "polygonShape_getVertexCount",   emoPhysicsPolygonShape_GetVertexCount);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "polygonShape_radius",    emoPhysicsPolygonShape_radius);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "circleShape_radius",     emoPhysicsCircleShape_radius);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "circleShape_position",   emoPhysicsCircleShape_position);
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
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getAnchorA",   emoPhysicsJoint_GetAnchorA);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getAnchorB",   emoPhysicsJoint_GetAnchorB);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getReactionForce",   emoPhysicsJoint_GetReactionForce);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getReactionTorque",   emoPhysicsJoint_GetReactionTorque);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_setLength",   emoPhysicsJoint_SetLength);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getLength",   emoPhysicsJoint_GetLength);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_setFrequency",   emoPhysicsJoint_SetFrequency);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getFrequency",   emoPhysicsJoint_GetFrequency);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_setDampingRatio",   emoPhysicsJoint_SetDampingRatio);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getDampingRatio",   emoPhysicsJoint_GetDampingRatio);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_setMaxForce",   emoPhysicsJoint_SetMaxForce);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getMaxForce",   emoPhysicsJoint_GetMaxForce);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_setMaxTorque",   emoPhysicsJoint_SetMaxTorque);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getMaxTorque",   emoPhysicsJoint_GetMaxTorque);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_setRatio",   emoPhysicsJoint_SetRatio);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getRatio",   emoPhysicsJoint_GetRatio);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getJointTranslation",   emoPhysicsJoint_GetJointTranslation);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getJointSpeed",   emoPhysicsJoint_GetJointSpeed);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getJointAngle",   emoPhysicsJoint_GetJointAngle);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_isLimitedEnabled",   emoPhysicsJoint_IsLimitedEnabled);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_enableLimit",   emoPhysicsJoint_EnableLimit);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getLowerLimit",   emoPhysicsJoint_GetLowerLimit);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getUpperLimit",   emoPhysicsJoint_GetUpperLimit);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_setLimits",   emoPhysicsJoint_SetLimits);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_isMotorEnabled",   emoPhysicsJoint_IsMotorEnabled);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_enableMotor",   emoPhysicsJoint_EnableMotor);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_setMotorSpeed",   emoPhysicsJoint_SetMotorSpeed);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_setMaxMotorTorque",   emoPhysicsJoint_SetMaxMotorTorque);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getMotorForce",   emoPhysicsJoint_GetMotorForce);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_setMaxMotorForce",   emoPhysicsJoint_SetMaxMotorForce);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getMotorTorque",   emoPhysicsJoint_GetMotorTorque);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getGroundAnchorA",   emoPhysicsJoint_GetGroundAnchorA);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getGroundAnchorB",   emoPhysicsJoint_GetGroundAnchorB);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getLength1",   emoPhysicsJoint_GetLength1);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getLength2",   emoPhysicsJoint_GetLength2);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_setTarget",    emoPhysicsJoint_SetTarget);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "joint_getTarget",    emoPhysicsJoint_GetTarget);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "updateJointDef",     emoPhysicsJointDef_Update);
	registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "initDistanceJointDef",     emoPhysicsInitDistanceJointDef);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "initFrictionJointDef",     emoPhysicsInitFrictionJointDef);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "initLineJointDef",         emoPhysicsInitLineJointDef);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "initPrismaticJointDef",    emoPhysicsInitPrismaticJointDef);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "initPulleyJointDef",       emoPhysicsInitPulleyJointDef);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "initRevoluteJointDef",     emoPhysicsInitRevoluteJointDef);
    registerClassFunc(engine.sqvm, EMO_PHYSICS_CLASS, "initWeldJointDef",         emoPhysicsInitWeldJointDef);
}