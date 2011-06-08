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
#include "Box2D/Box2D.h"
#include "squirrel.h"

#if __cplusplus
extern "C" {
#endif
#include "Physics_glue.h"
#include "Constants.h"
#include "VmFunc.h"
#include "stdio.h"
#include "Physics_util.h"
#include "Physics_contact.h"

extern void LOGI(const char* msg);
extern void LOGW(const char* msg);
extern void LOGE(const char* msg);

emo::EmoPhysicsContactListener* emoPhysicsContactListener = NULL;	

static SQInteger b2WorldReleaseHook(SQUserPointer ptr, SQInteger size) {
	if (emoPhysicsContactListener != NULL) {
		delete emoPhysicsContactListener;
		emoPhysicsContactListener = NULL;
	}
	delete reinterpret_cast<b2World*>(ptr);
	return 0;
}

static SQInteger b2ShapeReleaseHook(SQUserPointer ptr, SQInteger size) {
	delete reinterpret_cast<b2Shape*>(ptr);
	return 0;
}

static SQInteger b2JointDefReleaseHook(SQUserPointer ptr, SQInteger size) {
	delete reinterpret_cast<b2JointDef*>(ptr);
	return 0;
}
	
SQInteger emoPhysicsNewWorld(HSQUIRRELVM v) {
	b2Vec2 gravity;
	if (sq_gettype(v, 2) == OT_INSTANCE) {
		getVec2Instance(v, 2, &gravity);
	} else {
		gravity.Set(0, SENSOR_STANDARD_GRAVITY);
	}
	SQBool doSleep = true;
	getBool(v, 3, &doSleep);
	
	b2World* world = new b2World(gravity, doSleep);
	
	SQInteger result = createSQObject(v, 
				"emo", "Instance", world, b2WorldReleaseHook);
	if (result == 0) {
		delete world;
		return 0;
	}
	return 1;
}

SQInteger emoPhysicsCreateBody(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	if (nargs < 3) {
		return 0;
	}
	
	if (sq_gettype(v, 2) != OT_INSTANCE || sq_gettype(v, 3) != OT_INSTANCE) {
		return 0;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	b2BodyDef def;
	getBodyDefInstance(v, 3, &def);
	
	sq_pushuserpointer(v, world->CreateBody(&def));
	
	return 1;
}
SQInteger emoPhysicsDestroyBody(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	if (nargs < 3) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	if (sq_gettype(v, 2) != OT_INSTANCE || sq_gettype(v, 3) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	b2Body* body = NULL;
	sq_getuserpointer(v, 3, (SQUserPointer*)&body);

	world->DestroyBody(body);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsNewJointDef(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	if (nargs < 2 || sq_gettype(v, 2) != OT_INTEGER) {
		return 0;
	}

	SQInteger jtype;
	sq_getinteger(v, 2, &jtype);
	
	b2JointDef* def = NULL;
	switch (jtype) {
		case JOINT_TYPE_REVOLUTE:
			def = new b2RevoluteJointDef();
			break;
		case JOINT_TYPE_PRISMATIC:
			def = new b2PrismaticJointDef();
			break;
		case JOINT_TYPE_DISTANCE:
			def = new b2DistanceJointDef();
			break;
		case JOINT_TYPE_PULLEY:
			def = new b2PulleyJointDef();
			break;
		case JOINT_TYPE_MOUSE:
			def = new b2MouseJointDef();
			break;
		case JOINT_TYPE_GEAR:
			def = new b2GearJointDef();
			break;
		case JOINT_TYPE_LINE:
			def = new b2LineJointDef();
			break;
		case JOINT_TYPE_WELD:
			def = new b2WeldJointDef();
			break;
		case JOINT_TYPE_FRICTION:
			def = new b2FrictionJointDef();
			break;
		default:
			return 0;
	}

	SQInteger result = createSQObject(v, 
				  "emo", "Instance", def, b2JointDefReleaseHook);
	
	if (result == 0) {
		delete def;
		return 0;
	}
	return 1;
}
SQInteger emoPhysicsCreateJoint(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	if (nargs < 3) {
		return 0;
	}
	if (sq_gettype(v, 2) != OT_INSTANCE || sq_gettype(v, 3) != OT_INSTANCE) {
		return 0;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	b2JointDef* def = NULL;
	sq_getinstanceup(v, 3, (SQUserPointer*)&def, 0);
	
	sq_pushuserpointer(v, world->CreateJoint(def));
	
	return 1;
}
SQInteger emoPhysicsDestroyJoint(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	if (nargs < 3) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	if (sq_gettype(v, 2) != OT_INSTANCE || sq_gettype(v, 3) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 3, (SQUserPointer*)&joint);
	
	world->DestroyJoint(joint);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsWorld_Step(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	if (nargs < 5) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	SQFloat timeStep;
	SQInteger velocityIter;
	SQInteger positionIter;
	
	sq_getfloat(v, 3, &timeStep);
	sq_getinteger(v, 4, &velocityIter);
	sq_getinteger(v, 5, &positionIter);
	
	world->Step(timeStep, velocityIter, positionIter);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsWorld_EnableContactListener(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	if (emoPhysicsContactListener == NULL) {
		emoPhysicsContactListener = new emo::EmoPhysicsContactListener(v);
	}
	
	world->SetContactListener(emoPhysicsContactListener);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsWorld_EnableContactState(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	if (emoPhysicsContactListener == NULL) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	
	if (sq_gettype(v, 3) != OT_INTEGER || sq_gettype(v, 4) != OT_BOOL) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}

	SQInteger eType;
	SQBool    state;
	
	sq_getinteger(v, 3, &eType);
	sq_getbool(v, 4, &state);
	
	switch(eType) {
		case PHYSICS_STATE_NULL:
			emoPhysicsContactListener->enableNullEvent    = state;
			break;
		case PHYSICS_STATE_ADD:
			emoPhysicsContactListener->enableAddEvent     = state;
			break;
		case PHYSICS_STATE_PERSIST:
			emoPhysicsContactListener->enablePersistEvent = state;
			break;
		case PHYSICS_STATE_REMOVE:
			emoPhysicsContactListener->enableRemoveEvent  = state;
			break;
	}
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsWorld_ClearForces(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	world->ClearForces();
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsWorld_SetAutoClearForces(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	SQBool flag;
	getBool(v, 3, &flag);
	
	world->SetAutoClearForces(flag);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsWorld_GetAutoClearForces(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
        sq_pushbool(v, false);
		return 1;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	sq_pushbool(v, world->GetAutoClearForces());
	return 1;
}
SQInteger emoPhysicsWorld_SetGravity(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	b2Vec2 gravity;
	getVec2Instance(v, 3, &gravity);
	
	world->SetGravity(gravity);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsWorld_GetGravity(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushbool(v, false);
		return 1;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	pushVec2(v, world->GetGravity());
	return 1;
}
SQInteger emoPhysicsCreateFixture(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	b2FixtureDef def;
	getFixtureDefInstance(v, 3, &def);
	
	sq_pushuserpointer(v, body->CreateFixture(&def));
	
	return 1;
}
SQInteger emoPhysicsDestroyFixture(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	if (nargs < 3) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	if (sq_gettype(v, 2) != OT_USERPOINTER || sq_gettype(v, 3) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	b2Fixture* fixture = NULL;
	sq_getuserpointer(v, 3, (SQUserPointer*)&fixture);
	
	body->DestroyFixture(fixture);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsNewShape(HSQUIRRELVM v) {
	SQInteger sType = PHYSICS_SHAPE_UNKNOWN;
	if (sq_gettype(v, 2) == OT_INTEGER) {
		sq_getinteger(v, 2, &sType);
	}
	b2Shape* shape;
	if (sType == PHYSICS_SHAPE_CIRCLE) {
		shape = new b2CircleShape();
	} else if (sType == PHYSICS_SHAPE_POLYGON) {
		shape = new b2PolygonShape();
	} else {
		return 0;
	}
	SQInteger result = createSQObject(v, 
			"emo", "Instance", shape, b2ShapeReleaseHook);
	
	if (result == 0) {
		delete shape;
		return 0;
	}
	
	return 1;
}
SQInteger emoPhysicsPolygonShape_Set(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE || sq_gettype(v, 3) != OT_ARRAY) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2PolygonShape* shape;
	sq_getinstanceup(v, 2, (SQUserPointer*)&shape, 0);

	int size = 2; // b2PolygonShape->Set accepts only 2 vertices
	b2Vec2* vec2 = new b2Vec2[size];
	
	for (int i = 0; i < size; i++)
	{
		sq_pushinteger(v, i);
		sq_get(v, 3);
		
		getVec2Instance(v, sq_gettop(v), &vec2[i]);
		
		sq_pop(v, 1);
	}
	
	shape->Set(vec2, size);
	
	delete vec2;
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsPolygonShape_SetAsBox(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE
			|| sq_gettype(v, 3) == OT_NULL || sq_gettype(v, 4) == OT_NULL) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2PolygonShape* shape;
	sq_getinstanceup(v, 2, (SQUserPointer*)&shape, 0);
	
	float32 hx, hy;
	sq_getfloat(v, 3, &hx);
	sq_getfloat(v, 4, &hy);
	
	if (sq_gettype(v, 5) == OT_INSTANCE && sq_gettype(v, 6) != OT_NULL) {
		b2Vec2 center;
		getVec2Instance(v, 5, &center);
		
		float32 angle;
		sq_getfloat(v, 6, &angle);
		
		shape->SetAsBox(hx, hy, center, angle);
	} else {
		shape->SetAsBox(hx, hy);
	}
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsPolygonShape_SetAsEdge(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE
		|| sq_gettype(v, 3) != OT_INSTANCE || sq_gettype(v, 4) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2PolygonShape* shape;
	sq_getinstanceup(v, 2, (SQUserPointer*)&shape, 0);
	
	b2Vec2 v1, v2;
	getVec2Instance(v, 3, &v1);
	getVec2Instance(v, 4, &v2);
	
	shape->SetAsEdge(v1, v2);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsPolygonShape_GetVertex(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE || sq_gettype(v, 3) == OT_NULL) {
		return 0;
	}
	b2PolygonShape* shape;
	sq_getinstanceup(v, 2, (SQUserPointer*)&shape, 0);
	
	SQInteger idx;
	sq_getinteger(v, 3, &idx);
	
	pushVec2(v, shape->GetVertex(idx));
	
	return 1;
}
SQInteger emoPhysicsPolygonShape_GetVertexCount(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		return 0;
	}
	b2PolygonShape* shape;
	sq_getinstanceup(v, 2, (SQUserPointer*)&shape, 0);
	
	sq_pushinteger(v, shape->GetVertexCount());
	return 1;
}
SQInteger emoPhysicsBody_SetTransform(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER
			&& sq_gettype(v, 3) != OT_INSTANCE) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	b2Vec2 position;
	getVec2Instance(v, 3, &position);
	
	SQFloat angle;
	sq_getfloat(v, 4, &angle);
	
	body->SetTransform(position, angle);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_GetPosition(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	pushVec2(v, body->GetPosition());
	
	return 1;
}
SQInteger emoPhysicsBody_GetAngle(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushfloat(v, body->GetAngle());
	
	return 1;
}
SQInteger emoPhysicsBody_GetWorldCenter(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	pushVec2(v, body->GetWorldCenter());
	
	return 1;
}
SQInteger emoPhysicsBody_GetLocalCenter(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	pushVec2(v, body->GetLocalCenter());
	
	return 1;
}
SQInteger emoPhysicsBody_SetLinearVelocity(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER
		&& sq_gettype(v, 3) != OT_INSTANCE) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	b2Vec2 value;
	getVec2Instance(v, 3, &value);
	
	body->SetLinearVelocity(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_GetLinearVelocity(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	pushVec2(v, body->GetLinearVelocity());
	
	return 1;
}
SQInteger emoPhysicsBody_SetAngularVelocity(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER
		&& sq_gettype(v, 3) != OT_INSTANCE) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	body->SetAngularVelocity(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_GetAngularVelocity(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushfloat(v, body->GetAngularVelocity());
	
	return 1;
}
SQInteger emoPhysicsBody_ApplyForce(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	b2Vec2 force;
	getVec2Instance(v, 3, &force);
	
	b2Vec2 point;
	getVec2Instance(v, 4, &point);
	
	body->ApplyForce(force, point);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_ApplyTorque(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	body->ApplyTorque(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_ApplyLinearImpulse(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	b2Vec2 value1;
	getVec2Instance(v, 3, &value1);
	
	b2Vec2 value2;
	getVec2Instance(v, 4, &value2);
	
	body->ApplyLinearImpulse(value1, value2);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_ApplyAngularImpulse(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	body->ApplyAngularImpulse(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_GetMass(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushfloat(v, body->GetMass());
	
	return 1;
}
SQInteger emoPhysicsBody_GetInertia(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushfloat(v, body->GetInertia());
	
	return 1;
}
SQInteger emoPhysicsBody_GetWorldPoint(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER
		&& sq_gettype(v, 3) != OT_INSTANCE) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	b2Vec2 value;
	getVec2Instance(v, 3, &value);
	
	pushVec2(v, body->GetWorldPoint(value));
	
	return 1;
}
SQInteger emoPhysicsBody_GetWorldVector(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER
		&& sq_gettype(v, 3) != OT_INSTANCE) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	b2Vec2 value;
	getVec2Instance(v, 3, &value);
	
	pushVec2(v, body->GetWorldVector(value));
	
	return 1;
}
SQInteger emoPhysicsBody_GetLocalPoint(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER
		&& sq_gettype(v, 3) != OT_INSTANCE) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	b2Vec2 value;
	getVec2Instance(v, 3, &value);
	
	pushVec2(v, body->GetLocalPoint(value));
	
	return 1;
}
SQInteger emoPhysicsBody_GetLocalVector(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER
		&& sq_gettype(v, 3) != OT_INSTANCE) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	b2Vec2 value;
	getVec2Instance(v, 3, &value);
	
	pushVec2(v, body->GetLocalVector(value));
	
	return 1;
}
SQInteger emoPhysicsBody_GetLinearVelocityFromWorldPoint(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER
		&& sq_gettype(v, 3) != OT_INSTANCE) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	b2Vec2 value;
	getVec2Instance(v, 3, &value);
	
	pushVec2(v, body->GetLinearVelocityFromWorldPoint(value));
	
	return 1;
}
SQInteger emoPhysicsBody_GetLinearVelocityFromLocalPoint(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER
		&& sq_gettype(v, 3) != OT_INSTANCE) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	b2Vec2 value;
	getVec2Instance(v, 3, &value);
	
	pushVec2(v, body->GetLinearVelocityFromLocalPoint(value));
	
	return 1;
}
SQInteger emoPhysicsBody_GetLinearDamping(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushfloat(v, body->GetLinearDamping());
	
	return 1;
}
SQInteger emoPhysicsBody_SetLinearDamping(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	body->SetLinearDamping(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_GetAngularDamping(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushfloat(v, body->GetAngularDamping());
	
	return 1;
}
SQInteger emoPhysicsBody_SetAngularDamping(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	body->SetAngularDamping(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_SetType(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	SQInteger value;
	sq_getinteger(v, 3, &value);
	
	switch(value) {
		case PHYSICS_BODY_STATIC:
			body->SetType(b2_staticBody);
			break;
		case PHYSICS_BODY_KINEMATIC:
			body->SetType(b2_kinematicBody);
			break;
		case PHYSICS_BODY_DYNAMIC:
			body->SetType(b2_dynamicBody);
			break;
	}
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_GetType(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);

	int bodyType = PHYSICS_BODY_STATIC;
	switch(body->GetType()) {
		case b2_staticBody:
			bodyType = PHYSICS_BODY_STATIC;
			break;
		case b2_kinematicBody:
			bodyType = PHYSICS_BODY_KINEMATIC;
			break;
		case b2_dynamicBody:
			bodyType = PHYSICS_BODY_DYNAMIC;
			break;
	}
	sq_pushinteger(v, bodyType);
	
	return 1;
}
SQInteger emoPhysicsBody_SetBullet(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	SQBool flag;
	getBool(v, 3, &flag);
	
	body->SetBullet(flag);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_IsBullet(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushbool(v, body->IsBullet());
	
	return 1;
}
SQInteger emoPhysicsBody_SetSleepingAllowed(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	SQBool flag;
	getBool(v, 3, &flag);
	
	body->SetSleepingAllowed(flag);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_IsSleepingAllowed(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushbool(v, body->IsSleepingAllowed());
	
	return 1;
}
SQInteger emoPhysicsBody_SetAwake(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	SQBool flag;
	getBool(v, 3, &flag);
	
	body->SetAwake(flag);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_IsAwake(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushbool(v, body->IsAwake());
	
	return 1;
}
SQInteger emoPhysicsBody_SetActive(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);

	SQBool flag;
	getBool(v, 3, &flag);
	
	body->SetActive(flag);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_IsActive(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushbool(v, body->IsActive());
	
	return 1;
}
SQInteger emoPhysicsBody_SetFixedRotation(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	SQBool flag;
	getBool(v, 3, &flag);
	
	body->SetFixedRotation(flag);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsBody_IsFixedRotation(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushbool(v, body->IsFixedRotation());
	
	return 1;
}
SQInteger emoPhysicsCircleShape_position(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2CircleShape* shape;
	sq_getinstanceup(v, 2, (SQUserPointer*)&shape, 0);

	b2Vec2 value;
	getVec2Instance(v, 3, &value);
	
	shape->m_p = value;
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsCircleShape_radius(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2CircleShape* shape;
	sq_getinstanceup(v, 2, (SQUserPointer*)&shape, 0);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	shape->m_radius = value;
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsPolygonShape_radius(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2PolygonShape* shape;
	sq_getinstanceup(v, 2, (SQUserPointer*)&shape, 0);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	shape->m_radius = value;
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_GetAnchorA(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	pushVec2(v, joint->GetAnchorA());
	
	return 1;
}
SQInteger emoPhysicsJoint_GetAnchorB(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	pushVec2(v, joint->GetAnchorB());
	
	return 1;
}
SQInteger emoPhysicsJoint_GetReactionForce(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);

	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	pushVec2(v, joint->GetReactionForce(value));
	
	return 1;
}
SQInteger emoPhysicsJoint_GetReactionTorque(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	sq_pushfloat(v, joint->GetReactionTorque(value));
	
	return 1;
}
SQInteger emoPhysicsJoint_SetLength(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2DistanceJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	joint->SetLength(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_GetLength(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2DistanceJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetLength());
	
	return 1;
}
SQInteger emoPhysicsJoint_SetFrequency(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2DistanceJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	joint->SetFrequency(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_GetFrequency(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2DistanceJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetFrequency());
	
	return 1;
}
SQInteger emoPhysicsJoint_SetDampingRatio(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2DistanceJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	joint->SetDampingRatio(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_GetDampingRatio(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2DistanceJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetDampingRatio());
	
	return 1;
}
SQInteger emoPhysicsJoint_SetMaxForce(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2FrictionJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	joint->SetMaxForce(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_GetMaxForce(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2FrictionJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetMaxForce());
	
	return 1;
}
SQInteger emoPhysicsJoint_SetMaxTorque(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2FrictionJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	joint->SetMaxTorque(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_GetMaxTorque(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2FrictionJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetMaxTorque());
	
	return 1;
}
SQInteger emoPhysicsJoint_SetRatio(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2GearJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	joint->SetRatio(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_GetRatio(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2GearJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetRatio());
	
	return 1;
}
SQInteger emoPhysicsJoint_GetJointTranslation(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	if (joint->GetType() == e_lineJoint) {
		b2LineJoint* _joint = reinterpret_cast<b2LineJoint*>(joint);
		sq_pushfloat(v, _joint->GetJointTranslation());
	} else if (joint->GetType() == e_prismaticJoint) {
		b2PrismaticJoint* _joint = reinterpret_cast<b2PrismaticJoint*>(joint);
		sq_pushfloat(v, _joint->GetJointTranslation());
	} else {
		return 0;
	}
	
	return 1;
}
SQInteger emoPhysicsJoint_GetJointSpeed(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	if (joint->GetType() == e_lineJoint) {
		b2LineJoint* _joint = reinterpret_cast<b2LineJoint*>(joint);
		sq_pushfloat(v, _joint->GetJointSpeed());
	} else if (joint->GetType() == e_prismaticJoint) {
		b2PrismaticJoint* _joint = reinterpret_cast<b2PrismaticJoint*>(joint);
		sq_pushfloat(v, _joint->GetJointSpeed());
	} else if (joint->GetType() == e_revoluteJoint) {
		b2RevoluteJoint* _joint = reinterpret_cast<b2RevoluteJoint*>(joint);
		sq_pushfloat(v, _joint->GetJointSpeed());
	} else {
		return 0;
	}
	
	return 1;
}
SQInteger emoPhysicsJoint_GetJointAngle(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2RevoluteJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetJointAngle());
	
	return 1;
}
SQInteger emoPhysicsJoint_IsLimitedEnabled(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	if (joint->GetType() == e_lineJoint) {
		b2LineJoint* _joint = reinterpret_cast<b2LineJoint*>(joint);
		sq_pushbool(v, _joint->IsLimitEnabled());
	} else if (joint->GetType() == e_prismaticJoint) {
		b2PrismaticJoint* _joint = reinterpret_cast<b2PrismaticJoint*>(joint);
		sq_pushbool(v, _joint->IsLimitEnabled());
	} else if (joint->GetType() == e_revoluteJoint) {
		b2RevoluteJoint* _joint = reinterpret_cast<b2RevoluteJoint*>(joint);
		sq_pushbool(v, _joint->IsLimitEnabled());
	} else {
		return 0;
	}
	
	return 1;
}
SQInteger emoPhysicsJoint_EnableLimit(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQBool flag;
	getBool(v, 3, &flag);
	
	if (joint->GetType() == e_lineJoint) {
		b2LineJoint* _joint = reinterpret_cast<b2LineJoint*>(joint);
		_joint->EnableLimit(flag);
	} else if (joint->GetType() == e_prismaticJoint) {
		b2PrismaticJoint* _joint = reinterpret_cast<b2PrismaticJoint*>(joint);
		_joint->EnableLimit(flag);
	} else if (joint->GetType() == e_revoluteJoint) {
		b2RevoluteJoint* _joint = reinterpret_cast<b2RevoluteJoint*>(joint);
		_joint->EnableLimit(flag);
	} else {
		return 0;
	}
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_GetLowerLimit(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	if (joint->GetType() == e_lineJoint) {
		b2LineJoint* _joint = reinterpret_cast<b2LineJoint*>(joint);
		sq_pushfloat(v, _joint->GetLowerLimit());
	} else if (joint->GetType() == e_prismaticJoint) {
		b2PrismaticJoint* _joint = reinterpret_cast<b2PrismaticJoint*>(joint);
		sq_pushfloat(v, _joint->GetLowerLimit());
	} else if (joint->GetType() == e_revoluteJoint) {
		b2RevoluteJoint* _joint = reinterpret_cast<b2RevoluteJoint*>(joint);
		sq_pushfloat(v, _joint->GetLowerLimit());
	} else {
		return 0;
	}
	
	return 1;
}
SQInteger emoPhysicsJoint_GetUpperLimit(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	if (joint->GetType() == e_lineJoint) {
		b2LineJoint* _joint = reinterpret_cast<b2LineJoint*>(joint);
		sq_pushfloat(v, _joint->GetUpperLimit());
	} else if (joint->GetType() == e_prismaticJoint) {
		b2PrismaticJoint* _joint = reinterpret_cast<b2PrismaticJoint*>(joint);
		sq_pushfloat(v, _joint->GetUpperLimit());
	} else if (joint->GetType() == e_revoluteJoint) {
		b2RevoluteJoint* _joint = reinterpret_cast<b2RevoluteJoint*>(joint);
		sq_pushfloat(v, _joint->GetUpperLimit());
	} else {
		return 0;
	}
	
	return 1;
}
SQInteger emoPhysicsJoint_SetLimits(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat lower, upper;
	sq_getfloat(v, 3, &lower);
	sq_getfloat(v, 4, &upper);
	
	if (joint->GetType() == e_lineJoint) {
		b2LineJoint* _joint = reinterpret_cast<b2LineJoint*>(joint);
		_joint->SetLimits(lower, upper);
	} else if (joint->GetType() == e_prismaticJoint) {
		b2PrismaticJoint* _joint = reinterpret_cast<b2PrismaticJoint*>(joint);
		_joint->SetLimits(lower, upper);
	} else if (joint->GetType() == e_revoluteJoint) {
		b2RevoluteJoint* _joint = reinterpret_cast<b2RevoluteJoint*>(joint);
		_joint->SetLimits(lower, upper);
	} else {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_IsMotorEnabled(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	if (joint->GetType() == e_prismaticJoint) {
		b2PrismaticJoint* _joint = reinterpret_cast<b2PrismaticJoint*>(joint);
		sq_pushbool(v, _joint->IsMotorEnabled());
	} else if (joint->GetType() == e_revoluteJoint) {
		b2RevoluteJoint* _joint = reinterpret_cast<b2RevoluteJoint*>(joint);
		sq_pushbool(v, _joint->IsMotorEnabled());
	} else {
		return 0;
	}
	
	return 1;
}
SQInteger emoPhysicsJoint_EnableMotor(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQBool flag;
	getBool(v, 3, &flag);
	
	if (joint->GetType() == e_lineJoint) {
		b2LineJoint* _joint = reinterpret_cast<b2LineJoint*>(joint);
		_joint->EnableMotor(flag);
	} else if (joint->GetType() == e_prismaticJoint) {
		b2PrismaticJoint* _joint = reinterpret_cast<b2PrismaticJoint*>(joint);
		_joint->EnableMotor(flag);
	} else if (joint->GetType() == e_revoluteJoint) {
		b2RevoluteJoint* _joint = reinterpret_cast<b2RevoluteJoint*>(joint);
		_joint->EnableMotor(flag);
	} else {
		return 0;
	}
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_SetMotorSpeed(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	if (joint->GetType() == e_lineJoint) {
		b2LineJoint* _joint = reinterpret_cast<b2LineJoint*>(joint);
		_joint->SetMotorSpeed(value);
	} else if (joint->GetType() == e_prismaticJoint) {
		b2PrismaticJoint* _joint = reinterpret_cast<b2PrismaticJoint*>(joint);
		_joint->SetMotorSpeed(value);
	} else if (joint->GetType() == e_revoluteJoint) {
		b2RevoluteJoint* _joint = reinterpret_cast<b2RevoluteJoint*>(joint);
		_joint->SetMotorSpeed(value);
	} else {
		return 0;
	}
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_SetMaxMotorForce(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	if (joint->GetType() == e_lineJoint) {
		b2LineJoint* _joint = reinterpret_cast<b2LineJoint*>(joint);
		_joint->SetMaxMotorForce(value);
	} else if (joint->GetType() == e_prismaticJoint) {
		b2PrismaticJoint* _joint = reinterpret_cast<b2PrismaticJoint*>(joint);
		_joint->SetMaxMotorForce(value);
	} else {
		return 0;
	}
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_GetMotorForce(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	if (joint->GetType() == e_lineJoint) {
		b2LineJoint* _joint = reinterpret_cast<b2LineJoint*>(joint);
		sq_pushfloat(v, _joint->GetMotorForce());
	} else if (joint->GetType() == e_prismaticJoint) {
		b2PrismaticJoint* _joint = reinterpret_cast<b2PrismaticJoint*>(joint);
		sq_pushfloat(v, _joint->GetMotorForce());
	} else {
		return 0;
	}
	
	return 1;
}
SQInteger emoPhysicsJoint_SetMaxMotorTorque(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 0;
	}
	b2RevoluteJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	joint->SetMaxMotorTorque(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsJoint_GetMotorTorque(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2RevoluteJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetMotorTorque());
	
	return 1;
}
SQInteger emoPhysicsJoint_GetGroundAnchorA(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2PulleyJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	pushVec2(v, joint->GetGroundAnchorA());
	
	return 1;
}
SQInteger emoPhysicsJoint_GetGroundAnchorB(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2PulleyJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	pushVec2(v, joint->GetGroundAnchorB());
	
	return 1;
}
SQInteger emoPhysicsJoint_GetLength1(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2PulleyJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetLength1());
	
	return 1;
}
SQInteger emoPhysicsJoint_GetLength2(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2PulleyJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetLength2());
	
	return 1;
}
SQInteger emoPhysicsJointDef_Update(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2JointDef* def = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&def, 0);

	switch(def->type) {
		case e_distanceJoint:
			getDistanceJointDef(v, 3, reinterpret_cast<b2DistanceJointDef*> (def));
			break;
		case e_frictionJoint:
			getFrictionJointDef(v, 3, reinterpret_cast<b2FrictionJointDef*> (def));
			break;
		case e_gearJoint:
			getGearJointDef(v, 3, reinterpret_cast<b2GearJointDef*> (def));
			break;
		case e_lineJoint:
			getLineJointDef(v, 3, reinterpret_cast<b2LineJointDef*> (def));
			break;
		case e_prismaticJoint:
			getPrismaticJointDef(v, 3, reinterpret_cast<b2PrismaticJointDef*> (def));
			break;
		case e_pulleyJoint:
			getPulleyJointDef(v, 3, reinterpret_cast<b2PulleyJointDef*> (def));
			break;
		case e_revoluteJoint:
			getRevoluteJointDef(v, 3, reinterpret_cast<b2RevoluteJointDef*> (def));
			break;
		case e_weldJoint:
			getWeldJointDef(v, 3, reinterpret_cast<b2WeldJointDef*> (def));
			break;
		default:
			sq_pushinteger(v, ERR_INVALID_PARAM);
			return 1;
	}
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsInitDistanceJointDef(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	if (sq_gettype(v, 3) != OT_USERPOINTER || sq_gettype(v, 4) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	
	b2DistanceJointDef* def = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&def, 0);
	
	b2Body* bodyA = NULL;
	sq_getuserpointer(v, 3, (SQUserPointer*)&bodyA);
	
	b2Body* bodyB = NULL;
	sq_getuserpointer(v, 4, (SQUserPointer*)&bodyB);
	
	b2Vec2 anchorA, anchorB;
	getVec2Instance(v, 5, &anchorA);
	getVec2Instance(v, 6, &anchorB);
	
	def->Initialize(bodyA, bodyB, anchorA, anchorB);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsInitFrictionJointDef(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	if (sq_gettype(v, 3) != OT_USERPOINTER || sq_gettype(v, 4) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	
	b2FrictionJointDef* def = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&def, 0);
	
	b2Body* bodyA = NULL;
	sq_getuserpointer(v, 3, (SQUserPointer*)&bodyA);
	
	b2Body* bodyB = NULL;
	sq_getuserpointer(v, 4, (SQUserPointer*)&bodyB);
	
	b2Vec2 anchor;
	getVec2Instance(v, 5, &anchor);
	
	def->Initialize(bodyA, bodyB, anchor);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsInitLineJointDef(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	if (sq_gettype(v, 3) != OT_USERPOINTER || sq_gettype(v, 4) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	
	b2LineJointDef* def = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&def, 0);
	
	b2Body* bodyA = NULL;
	sq_getuserpointer(v, 3, (SQUserPointer*)&bodyA);
	
	b2Body* bodyB = NULL;
	sq_getuserpointer(v, 4, (SQUserPointer*)&bodyB);
	
	b2Vec2 anchor, axis;
	getVec2Instance(v, 5, &anchor);
	getVec2Instance(v, 6, &axis);
	
	def->Initialize(bodyA, bodyB, anchor, axis);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsInitPrismaticJointDef(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	if (sq_gettype(v, 3) != OT_USERPOINTER || sq_gettype(v, 4) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	
	b2PrismaticJointDef* def = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&def, 0);
	
	b2Body* bodyA = NULL;
	sq_getuserpointer(v, 3, (SQUserPointer*)&bodyA);
	
	b2Body* bodyB = NULL;
	sq_getuserpointer(v, 4, (SQUserPointer*)&bodyB);
	
	b2Vec2 anchor, axis;
	getVec2Instance(v, 5, &anchor);
	getVec2Instance(v, 6, &axis);
	
	def->Initialize(bodyA, bodyB, anchor, axis);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsInitPulleyJointDef(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	if (sq_gettype(v, 3) != OT_USERPOINTER || sq_gettype(v, 4) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	
	b2PulleyJointDef* def = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&def, 0);
	
	b2Body* bodyA = NULL;
	sq_getuserpointer(v, 3, (SQUserPointer*)&bodyA);
	
	b2Body* bodyB = NULL;
	sq_getuserpointer(v, 4, (SQUserPointer*)&bodyB);
	
	b2Vec2 gAnchorA, gAnchorB, anchorA, anchorB;
	getVec2Instance(v, 5, &gAnchorA);
	getVec2Instance(v, 6, &gAnchorB);
	getVec2Instance(v, 7, &anchorA);
	getVec2Instance(v, 8, &anchorB);
	
	SQFloat ratio = 0;
	sq_getfloat(v, 9, &ratio);
	
	if (ratio <= 0) ratio = 1.0;
	
	def->Initialize(bodyA, bodyB, gAnchorA, gAnchorB, anchorA, anchorB, ratio);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsInitRevoluteJointDef(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	if (sq_gettype(v, 3) != OT_USERPOINTER || sq_gettype(v, 4) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	
	b2RevoluteJointDef* def = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&def, 0);
	
	b2Body* bodyA = NULL;
	sq_getuserpointer(v, 3, (SQUserPointer*)&bodyA);
	
	b2Body* bodyB = NULL;
	sq_getuserpointer(v, 4, (SQUserPointer*)&bodyB);
	
	b2Vec2 anchor;
	getVec2Instance(v, 5, &anchor);
	
	def->Initialize(bodyA, bodyB, anchor);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsInitWeldJointDef(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	if (sq_gettype(v, 3) != OT_USERPOINTER || sq_gettype(v, 4) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	
	b2WeldJointDef* def = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&def, 0);
	
	b2Body* bodyA = NULL;
	sq_getuserpointer(v, 3, (SQUserPointer*)&bodyA);
	
	b2Body* bodyB = NULL;
	sq_getuserpointer(v, 4, (SQUserPointer*)&bodyB);
	
	b2Vec2 anchor;
	getVec2Instance(v, 5, &anchor);
	
	def->Initialize(bodyA, bodyB, anchor);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
#if __cplusplus
}   // Extern C
#endif
