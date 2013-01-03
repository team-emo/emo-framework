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
#include "Physics.h"
#include "VmFunc.h"
#include "Engine.h"
#include "Physics_glue.h"

extern emo::Engine* engine;

void initPhysicsFunctions() {
    registerTable(engine->sqvm, EMO_PHYSICS_TABLE);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "newWorld",      emoPhysicsNewWorld);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "newShape",      emoPhysicsNewShape);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "createBody",    emoPhysicsCreateBody);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "createGroundBody",    emoPhysicsCreateGroundBody);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "destroyBody",   emoPhysicsDestroyBody);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "createJoint",   emoPhysicsCreateJoint);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "destroyJoint",  emoPhysicsDestroyJoint);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "world_step",    emoPhysicsWorld_Step);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "world_enableContactListener", emoPhysicsWorld_EnableContactListener);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "world_enableContactState",    emoPhysicsWorld_EnableContactState);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "world_clearForces", emoPhysicsWorld_ClearForces);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "createFixture", emoPhysicsCreateFixture);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "destroyFixture",emoPhysicsDestroyFixture);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "newJointDef",   emoPhysicsNewJointDef);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "world_setAutoClearForces", emoPhysicsWorld_SetAutoClearForces);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "world_getAutoClearForces", emoPhysicsWorld_GetAutoClearForces);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "world_setGravity",   emoPhysicsWorld_SetGravity);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "world_getGravity",   emoPhysicsWorld_GetGravity);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "polygonShape_set",   emoPhysicsPolygonShape_Set);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "polygonShape_setAsBox",   emoPhysicsPolygonShape_SetAsBox);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "polygonShape_setAsEdge",   emoPhysicsPolygonShape_SetAsEdge);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "polygonShape_getVertex",   emoPhysicsPolygonShape_GetVertex);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "polygonShape_getVertexCount",   emoPhysicsPolygonShape_GetVertexCount);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "polygonShape_radius",    emoPhysicsPolygonShape_radius);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "circleShape_radius",     emoPhysicsCircleShape_radius);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "circleShape_position",   emoPhysicsCircleShape_position);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setTransform",   emoPhysicsBody_SetTransform);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getPosition",   emoPhysicsBody_GetPosition);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getAngle",   emoPhysicsBody_GetAngle);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getWorldCenter",   emoPhysicsBody_GetWorldCenter);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getLocalCenter",   emoPhysicsBody_GetLocalCenter);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setLinearVelocity",   emoPhysicsBody_SetLinearVelocity);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getLinearVelocity",   emoPhysicsBody_GetLinearVelocity);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setAngularVelocity",   emoPhysicsBody_SetAngularVelocity);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getAngularVelocity",   emoPhysicsBody_GetAngularVelocity);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_applyForce",   emoPhysicsBody_ApplyForce);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_applyTorque",   emoPhysicsBody_ApplyTorque);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_applyLinearImpulse",   emoPhysicsBody_ApplyLinearImpulse);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_applyAngularImpulse",   emoPhysicsBody_ApplyAngularImpulse);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getMass",   emoPhysicsBody_GetMass);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setCenterOfMass",   emoPhysicsBody_SetCenterOfMass);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getInertia",   emoPhysicsBody_GetInertia);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getWorldPoint",   emoPhysicsBody_GetWorldPoint);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getWorldVector",   emoPhysicsBody_GetWorldVector);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getLocalPoint",   emoPhysicsBody_GetLocalPoint);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getLocalVector",   emoPhysicsBody_GetLocalVector);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getLinearVelocityFromWorldPoint",   emoPhysicsBody_GetLinearVelocityFromWorldPoint);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getLinearVelocityFromLocalPoint",   emoPhysicsBody_GetLinearVelocityFromLocalPoint);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getLinearDamping",   emoPhysicsBody_GetLinearDamping);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setLinearDamping",   emoPhysicsBody_SetLinearDamping);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getAngularDamping",   emoPhysicsBody_GetAngularDamping);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setAngularDamping",   emoPhysicsBody_SetAngularDamping);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setType",   emoPhysicsBody_SetType);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_getType",   emoPhysicsBody_GetType);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setBullet",   emoPhysicsBody_SetBullet);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_isBullet",   emoPhysicsBody_IsBullet);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setSleepingAllowed",   emoPhysicsBody_SetSleepingAllowed);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_isSleepingAllowed",   emoPhysicsBody_IsSleepingAllowed);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setAwake",   emoPhysicsBody_SetAwake);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_isAwake",   emoPhysicsBody_IsAwake);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setActive",   emoPhysicsBody_SetActive);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_isActive",   emoPhysicsBody_IsActive);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setFixedRotation",   emoPhysicsBody_SetFixedRotation);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_isFixedRotation",   emoPhysicsBody_IsFixedRotation);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "body_setFilter",   emoPhysicsBody_SetFilter);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getAnchorA",   emoPhysicsJoint_GetAnchorA);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getAnchorB",   emoPhysicsJoint_GetAnchorB);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getReactionForce",   emoPhysicsJoint_GetReactionForce);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getReactionTorque",   emoPhysicsJoint_GetReactionTorque);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_setLength",   emoPhysicsJoint_SetLength);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getLength",   emoPhysicsJoint_GetLength);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_setFrequency",   emoPhysicsJoint_SetFrequency);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getFrequency",   emoPhysicsJoint_GetFrequency);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_setDampingRatio",   emoPhysicsJoint_SetDampingRatio);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getDampingRatio",   emoPhysicsJoint_GetDampingRatio);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_setMaxForce",   emoPhysicsJoint_SetMaxForce);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getMaxForce",   emoPhysicsJoint_GetMaxForce);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_setMaxTorque",   emoPhysicsJoint_SetMaxTorque);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getMaxTorque",   emoPhysicsJoint_GetMaxTorque);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_setRatio",   emoPhysicsJoint_SetRatio);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getRatio",   emoPhysicsJoint_GetRatio);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getJointTranslation",   emoPhysicsJoint_GetJointTranslation);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getJointSpeed",   emoPhysicsJoint_GetJointSpeed);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getJointAngle",   emoPhysicsJoint_GetJointAngle);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_isLimitedEnabled",   emoPhysicsJoint_IsLimitedEnabled);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_enableLimit",   emoPhysicsJoint_EnableLimit);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getLowerLimit",   emoPhysicsJoint_GetLowerLimit);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getUpperLimit",   emoPhysicsJoint_GetUpperLimit);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_setLimits",   emoPhysicsJoint_SetLimits);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_isMotorEnabled",   emoPhysicsJoint_IsMotorEnabled);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_enableMotor",   emoPhysicsJoint_EnableMotor);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_setMotorSpeed",   emoPhysicsJoint_SetMotorSpeed);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_setMaxMotorTorque",   emoPhysicsJoint_SetMaxMotorTorque);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getMotorForce",   emoPhysicsJoint_GetMotorForce);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_setMaxMotorForce",   emoPhysicsJoint_SetMaxMotorForce);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getMotorTorque",   emoPhysicsJoint_GetMotorTorque);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getGroundAnchorA",   emoPhysicsJoint_GetGroundAnchorA);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getGroundAnchorB",   emoPhysicsJoint_GetGroundAnchorB);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getLength1",   emoPhysicsJoint_GetLength1);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getLength2",   emoPhysicsJoint_GetLength2);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_setTarget",    emoPhysicsJoint_SetTarget);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "joint_getTarget",    emoPhysicsJoint_GetTarget);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "updateJointDef",     emoPhysicsJointDef_Update);
	registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "initDistanceJointDef",     emoPhysicsInitDistanceJointDef);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "initFrictionJointDef",     emoPhysicsInitFrictionJointDef);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "initLineJointDef",         emoPhysicsInitLineJointDef);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "initPrismaticJointDef",    emoPhysicsInitPrismaticJointDef);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "initPulleyJointDef",       emoPhysicsInitPulleyJointDef);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "initRevoluteJointDef",     emoPhysicsInitRevoluteJointDef);
    registerMemberFunc(engine->sqvm, EMO_PHYSICS_TABLE, "initWeldJointDef",         emoPhysicsInitWeldJointDef);
}
