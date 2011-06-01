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
#include "squirrel.h"

SQInteger emoPhysicsNewWorld(HSQUIRRELVM v);
SQInteger emoPhysicsNewShape(HSQUIRRELVM v);
SQInteger emoPhysicsCreateBody(HSQUIRRELVM v);
SQInteger emoPhysicsDestroyBody(HSQUIRRELVM v);
SQInteger emoPhysicsCreateJoint(HSQUIRRELVM v);
SQInteger emoPhysicsDestroyJoint(HSQUIRRELVM v);
SQInteger emoPhysicsWorld_Step(HSQUIRRELVM v);
SQInteger emoPhysicsWorld_ClearForces(HSQUIRRELVM v);
SQInteger emoPhysicsCreateFixture(HSQUIRRELVM v);
SQInteger emoPhysicsDestroyFixture(HSQUIRRELVM v);
SQInteger emoPhysicsNewJointDef(HSQUIRRELVM v);
SQInteger emoPhysicsWorld_EnableContactListener(HSQUIRRELVM v);
SQInteger emoPhysicsWorld_EnableContactState(HSQUIRRELVM v);
SQInteger emoPhysicsWorld_SetAutoClearForces(HSQUIRRELVM v);
SQInteger emoPhysicsWorld_GetAutoClearForces(HSQUIRRELVM v);
SQInteger emoPhysicsWorld_SetGravity(HSQUIRRELVM v);
SQInteger emoPhysicsWorld_GetGravity(HSQUIRRELVM v);
SQInteger emoPhysicsPolygonShape_Set(HSQUIRRELVM v);
SQInteger emoPhysicsPolygonShape_SetAsBox(HSQUIRRELVM v);
SQInteger emoPhysicsPolygonShape_SetAsEdge(HSQUIRRELVM v);
SQInteger emoPhysicsPolygonShape_GetVertex(HSQUIRRELVM v);
SQInteger emoPhysicsPolygonShape_GetVertexCount(HSQUIRRELVM v);
SQInteger emoPhysicsBody_SetTransform(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetPosition(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetAngle(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetWorldCenter(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetLocalCenter(HSQUIRRELVM v);
SQInteger emoPhysicsBody_SetLinearVelocity(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetLinearVelocity(HSQUIRRELVM v);
SQInteger emoPhysicsBody_SetAngularVelocity(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetAngularVelocity(HSQUIRRELVM v);
SQInteger emoPhysicsBody_ApplyForce(HSQUIRRELVM v);
SQInteger emoPhysicsBody_ApplyTorque(HSQUIRRELVM v);
SQInteger emoPhysicsBody_ApplyLinearImpulse(HSQUIRRELVM v);
SQInteger emoPhysicsBody_ApplyAngularImpulse(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetMass(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetInertia(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetWorldPoint(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetWorldVector(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetLocalPoint(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetLocalVector(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetLinearVelocityFromWorldPoint(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetLinearVelocityFromLocalPoint(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetLinearDamping(HSQUIRRELVM v);
SQInteger emoPhysicsBody_SetLinearDamping(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetAngularDamping(HSQUIRRELVM v);
SQInteger emoPhysicsBody_SetAngularDamping(HSQUIRRELVM v);
SQInteger emoPhysicsBody_SetType(HSQUIRRELVM v);
SQInteger emoPhysicsBody_GetType(HSQUIRRELVM v);
SQInteger emoPhysicsBody_SetBullet(HSQUIRRELVM v);
SQInteger emoPhysicsBody_IsBullet(HSQUIRRELVM v);
SQInteger emoPhysicsBody_SetSleepingAllowed(HSQUIRRELVM v);
SQInteger emoPhysicsBody_IsSleepingAllowed(HSQUIRRELVM v);
SQInteger emoPhysicsBody_SetAwake(HSQUIRRELVM v);
SQInteger emoPhysicsBody_IsAwake(HSQUIRRELVM v);
SQInteger emoPhysicsBody_SetActive(HSQUIRRELVM v);
SQInteger emoPhysicsBody_IsActive(HSQUIRRELVM v);
SQInteger emoPhysicsBody_SetFixedRotation(HSQUIRRELVM v);
SQInteger emoPhysicsBody_IsFixedRotation(HSQUIRRELVM v);
SQInteger emoPhysicsCircleShape_position(HSQUIRRELVM v);
SQInteger emoPhysicsCircleShape_radius(HSQUIRRELVM v);
SQInteger emoPhysicsPolygonShape_radius(HSQUIRRELVM v);

SQInteger emoPhysicsJoint_GetAnchorA(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetAnchorB(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetReactionForce(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetReactionTorque(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_SetLength(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetLength(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_SetFrequency(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetFrequency(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_SetDampingRatio(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetDampingRatio(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_SetMaxForce(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetMaxForce(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_SetMaxTorque(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetMaxTorque(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_SetRatio(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetRatio(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetJointTranslation(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetJointSpeed(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetJointAngle(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_IsLimitedEnabled(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_EnableLimit(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetLowerLimit(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetUpperLimit(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_SetLimits(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_IsMotorEnabled(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_EnableMotor(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_SetMotorSpeed(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_SetMaxMotorTorque(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetMotorForce(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_SetMaxMotorForce(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetMotorTorque(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetGroundAnchorA(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetGroundAnchorB(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetLength1(HSQUIRRELVM v);
SQInteger emoPhysicsJoint_GetLength2(HSQUIRRELVM v);
SQInteger emoPhysicsJointDef_Update(HSQUIRRELVM v);
SQInteger emoPhysicsInitDistanceJointDef(HSQUIRRELVM v);
SQInteger emoPhysicsInitFrictionJointDef(HSQUIRRELVM v);
SQInteger emoPhysicsInitLineJointDef(HSQUIRRELVM v);
SQInteger emoPhysicsInitPrismaticJointDef(HSQUIRRELVM v);
SQInteger emoPhysicsInitPulleyJointDef(HSQUIRRELVM v);
SQInteger emoPhysicsInitRevoluteJointDef(HSQUIRRELVM v);
SQInteger emoPhysicsInitWeldJointDef(HSQUIRRELVM v);
