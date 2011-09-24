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
	
/*
 * create new physics world
 *
 * @param gravity (vec2 instance)
 * @param sleep objects or not
 * @return b2World instance
 */
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

/*
 * create new physics body
 *
 * @param instance of physics world
 * @param bodydef instance
 * @return pointer of physics body
 */
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

/*
 * create new physics ground body
 *
 * @param instance of physics world
 * @return pointer of physics body
 */
SQInteger emoPhysicsCreateGroundBody(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	if (nargs < 2 || sq_gettype(v, 2) != OT_INSTANCE) {
		return 0;
	}
	b2World* world = NULL;
	sq_getinstanceup(v, 2, (SQUserPointer*)&world, 0);
	
	b2BodyDef def;
	sq_pushuserpointer(v, world->CreateBody(&def));
	
	return 1;
}

/*
 * destroy a physics body
 *
 * @param physics world instance
 * @param pointer of the body
 * @return EMO_NO_ERROR is succeeds
 */
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

/*
 * create new physics jointdef instance
 *
 * @param joint type
 * @return physics jointdef instance
 */
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

/*
 * create new physics joint
 *
 * @param physics world instance
 * @param physics jointdef instance
 * @return pointer of physics joint
 */
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

/*
 * destroy physics joint
 *
 * @param physics world instance
 * @param pointer of physics joint
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * step physics world
 *
 * @param physics world instance
 * @param time step
 * @param velocity iterations
 * @param position iterations
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * enable contact listener of physics world
 *
 * @param physics world instance
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * enable/disable contact state of physics world
 *
 * state is one of PHYSICS_STATE_NULL, PHYSICS_STATE_ADD,
 * PHYSICS_STATE_PERSIST or PHYSICS_STATE_REMOVE
 * 
 * @param physics world instance
 * @param contact state type
 * @param enable or not
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * clear forces of physics world
 *
 * @param physics world instance
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * Set flag to control automatic clearing of forces after each time step. 
 *
 * @param physics world instance
 * @param enable or not
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * Get the flag that controls automatic clearing of forces after each time step. 
 *
 * @param physics world instance
 * @return the flag that controls automatic clearing of forces after each time step. 
 */
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

/*
 * set gravity of physics world
 *
 * @param physics world instance
 * @param gravity (vec2 instance)
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * get gravity of physics world
 * 
 * @param physics world instance
 * @return gravity (vec2 instance)
 */
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

/*
 * create physics fixture
 *
 * @param b2Body pointer
 * @param fixturedef instance
 * @return pointer of b2Fixture
 */
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

/*
 * destroy physics fixture
 *
 * @param b2Body pointer
 * @param b2Fixture pointer
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * create new physics shape
 * physics shape type is one of PHYSICS_SHAPE_CIRCLE or PHYSICS_SHAPE_POLYGON
 *
 * @param type of physics shape
 * @return instance of physics shape
 */
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

/*
 * call b2PolygonShape->Set
 *
 * @param instance of physics polygon shape
 * @param array of vertices
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2PolygonShape->SetAsBox
 *
 * @param hx the half width
 * @param hy the half height
 * @param the center of the box in local coordinates
 * @param the rotation of the box in local coordinates
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2PolygonShape->SetAsEdge
 *
 * @param b2PolygonShape instance
 * @param v1 vec2 instance 
 * @param v2 vec2 instance
 * @return EMO_NO_ERROR if succeeds
 */
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
    
/*
 * call b2PolygonShape->GetVertex
 * 
 * @param b2PolygonShape instance
 * @param index of vertex
 * @return vertex
 */
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

/*
 * get vertex count of b2PolygonShape
 *
 * @param b2PolygonShape instance
 * @return vertex count
 */
SQInteger emoPhysicsPolygonShape_GetVertexCount(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_INSTANCE) {
		return 0;
	}
	b2PolygonShape* shape;
	sq_getinstanceup(v, 2, (SQUserPointer*)&shape, 0);
	
	sq_pushinteger(v, shape->GetVertexCount());
	return 1;
}

/*
 * call b2Body->SetTransform
 *
 * @param pointer of b2Body 
 * @param position (vec2 instance)
 * @param angle
 */
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

/*
 * get position of b2Body
 *
 * @param pointer of b2Body
 * @param position (vec2 instance)
 */
SQInteger emoPhysicsBody_GetPosition(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	pushVec2(v, body->GetPosition());
	
	return 1;
}

/*
 * get angle of b2Body
 *
 * @param pointer of b2Body
 * @return angle
 */
SQInteger emoPhysicsBody_GetAngle(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushfloat(v, body->GetAngle());
	
	return 1;
}

/*
 * call b2Body->GetWorldCenter
 *
 * @param pointer of b2Body
 * @return world position of the center of mass
 */
SQInteger emoPhysicsBody_GetWorldCenter(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	pushVec2(v, body->GetWorldCenter());
	
	return 1;
}

/*
 * call b2Body->GetLocalCenter
 *
 * @param pointer of b2Body
 * @return local position of the center of mass
 */
SQInteger emoPhysicsBody_GetLocalCenter(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	pushVec2(v, body->GetLocalCenter());
	
	return 1;
}
    
/*
 * Set the linear velocity of the center of mass
 * 
 * @param pointer of b2Body
 * @param linear velocity (vec2 instance)
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * Get the linear velocity of the center of mass
 *
 * @param pointer of b2Body
 * @return linear velocity of the center of mass
 */
SQInteger emoPhysicsBody_GetLinearVelocity(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	pushVec2(v, body->GetLinearVelocity());
	
	return 1;
}

/*
 * Set the angular velocity
 *
 * @param pointer of b2Body
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * Get the angular velocity
 *
 * @param pointer of b2Body
 * @return anglular velocity
 */
SQInteger emoPhysicsBody_GetAngularVelocity(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushfloat(v, body->GetAngularVelocity());
	
	return 1;
}

/*
 * call b2Body->ApplyForce
 *
 * @param pointer of b2Body
 * @param force (vec2 instance)
 * @param point (vec2 instance)
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2Body->ApplyTorque
 *
 * @param pointer of b2Body
 * @param torque
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2Body->ApplyLinearImpulse
 *
 * @param pointer of b2Body
 * @param value1 (vec2 instance)
 * @param value2 (vec2 instance)
 * @return EMO_NO_ERROR
 */
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

/*
 * call b2Body->ApplyAngularImpulse
 *
 * @param pointer of b2Body
 * @param angular impulse
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2Body->GetMass
 *
 * @param pointer of b2Body
 * @return total mass of the body
 */
SQInteger emoPhysicsBody_GetMass(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushfloat(v, body->GetMass());
	
	return 1;
}

/*
 * call b2Body->getInertia
 *
 * @param pointer of b2Body
 * @return central rotational inertia of the body
 */
SQInteger emoPhysicsBody_GetInertia(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushfloat(v, body->GetInertia());
	
	return 1;
}

/*
 * call b2Body->GetWorldPoint
 *
 * @param pointer of b2Body
 * @param point on the body measured relative the the body's origin (vec2 instance)
 * @return world coordinates of a point given the local coordinates
 */
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

/*
 * call b2Body->GetWorldVector
 * 
 * @param pointer of b2Body
 * @param a vector fixed in the body
 * @return world coordinates of a vector given the local coordinates
 */
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

/*
 * call b2Body->GetLocalPoint
 *
 * @param pointer of b2Body
 * @param point in world coordinates
 * @return a local point relative to the body's origin given a world point
 */
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

/*
 * call b2Body->GetLocalVector
 *
 * @param pointer of b2Body
 * @param vector in world coordinates
 * @return a local vector given a world vector
 */
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

/*
 * call b2Body->GetLinearVelocityFromWorldPoint
 *
 * @param pointer of b2Body
 * @param point in world coordinates
 * @return world linear velocity of a world point attached to this body
 */
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

/*
 * call b2Body->GetLinearVelocityFromLocalPoint
 *
 * @param pointer of b2Body
 * @param point in local coordinates
 * @return world velocity of a local point
 */
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

/*
 * call b2Body->GetLinearDamping
 *
 * @param pointer of b2Body
 * @return linear damping of the body
 */
SQInteger emoPhysicsBody_GetLinearDamping(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushfloat(v, body->GetLinearDamping());
	
	return 1;
}

/*
 * call b2Body->SetLinearDamping
 *
 * @param pointer of b2Body
 * @param linear damping of the body
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2Body->GetAngularDamping
 *
 * @param pointer of b2Body
 * @return angular damping
 */
SQInteger emoPhysicsBody_GetAngularDamping(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushfloat(v, body->GetAngularDamping());
	
	return 1;
}

/*
 * call b2Body->SetAngularDamping
 *
 * @param angular damping of the body
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * set type of the body
 * body type is one of PHYSICS_BODY_STATIC, PHYSICS_BODY_KINEMATIC,
 * or PHYSICS_BODY_DYNAMIC
 * 
 * @param pointer of b2Body
 * @param type of the body
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * get type of the body
 * body type is one of PHYSICS_BODY_STATIC, PHYSICS_BODY_KINEMATIC,
 * or PHYSICS_BODY_DYNAMIC
 *
 * @param pointer of the body
 * @return type of the body
 */
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

/*
 * Should this body be treated like a bullet for continuous collision detection? 
 *
 * @param pointer of b2Body
 * @param treat like a bullet or not
 */
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

/*
 * Is this body treated like a bullet for continuous collision detection? 
 *
 * @param pointer of the body
 * @return is bullet or not
 */
SQInteger emoPhysicsBody_IsBullet(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushbool(v, body->IsBullet());
	
	return 1;
}

/*
 * set whether this body allowed to sleep
 * 
 * @param pointer of b2Body
 * @param allowed to sleep or not
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * returns whether the body is allowd to sleep
 * 
 * @param pointer of b2Body
 * @return sleeping is allowed or noit
 */
SQInteger emoPhysicsBody_IsSleepingAllowed(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushbool(v, body->IsSleepingAllowed());
	
	return 1;
}

/*
 * set the body awake
 *
 * @param pointer of b2Body
 * @param awake or not
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * returns whether the body is awake or not
 *
 * @param pointer of b2Body
 * @return awake or not
 */
SQInteger emoPhysicsBody_IsAwake(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushbool(v, body->IsAwake());
	
	return 1;
}

/*
 * set the body active
 * 
 * @param pointer of b2Body
 * @param active or not
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * returns whether the body is active or not
 *
 * @param pointer of b2Body
 * @return active or not
 */
SQInteger emoPhysicsBody_IsActive(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushbool(v, body->IsActive());
	
	return 1;
}

/*
 * call b2Body->SetFixedRotation
 *
 * @param pointer of b2Body
 * @param use fixed rotation or not
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2Body->IsFixedRotation
 *
 * @param pointer of b2Body
 * @return is fixed rotation
 */
SQInteger emoPhysicsBody_IsFixedRotation(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Body* body = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&body);
	
	sq_pushbool(v, body->IsFixedRotation());
	
	return 1;
}

/*
 * set b2CircleShape->m_p
 *
 * @param b2CircleShape instance
 * @param m_p
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * set b2CircleShape->m_radius
 *
 * @param b2CircleShape instance
 * @param m_radius
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * set b2PolygonShape->m_radius
 *
 * @param b2PolygonShape instance
 * @param m_radius
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2Joint->GetAnchorA
 *
 * @param pointer of b2Joint
 * @return vector value of anchor A
 */
SQInteger emoPhysicsJoint_GetAnchorA(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	pushVec2(v, joint->GetAnchorA());
	
	return 1;
}

/*
 * call b2Joint->GetAnchorB
 *
 * @param pointer of b2Joint
 * @return vector value of anchor B
 */
SQInteger emoPhysicsJoint_GetAnchorB(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	pushVec2(v, joint->GetAnchorB());
	
	return 1;
}

/*
 * call b2Joint->GetReactionForce
 *
 * @param pointer of b2Joint
 * @param timestep
 * @return reaction force on body at the joint anchor in Newtons
 */
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

/*
 * call b2Joint->GetReactionTorque
 *
 * @param pointer of b2Joint
 * @param timestep
 * @param reaction torque
 */
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

/*
 * call b2Joint->SetLength
 *
 * @param pointer of b2DistanceJoint
 * @param length
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2DistanceJoint->GetLength
 * 
 * @param pointer of b2DistanceJoint
 * @return length
 */
SQInteger emoPhysicsJoint_GetLength(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2DistanceJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetLength());
	
	return 1;
}

/*
 * call b2DistanceJoint->SetFrequency
 *
 * @param pointer of b2DistanceJoint
 * @param frequency
 * @return EMO_NO_ERROR if succeeds
 */
SQInteger emoPhysicsJoint_SetFrequency(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	if (joint->GetType() == e_distanceJoint) {
		b2DistanceJoint* _joint = reinterpret_cast<b2DistanceJoint*>(joint);
        _joint->SetFrequency(value);
    } else if (joint->GetType() == e_mouseJoint) {
		b2MouseJoint* _joint = reinterpret_cast<b2MouseJoint*>(joint);
        _joint->SetFrequency(value);
    }
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}

/*
 * call b2DistanceJoint->GetFrequency
 *
 * @param pointer of b2DistanceJoint
 * @return frequency
 */
SQInteger emoPhysicsJoint_GetFrequency(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	if (joint->GetType() == e_distanceJoint) {
		b2DistanceJoint* _joint = reinterpret_cast<b2DistanceJoint*>(joint);
        sq_pushfloat(v, _joint->GetFrequency());
    } else if (joint->GetType() == e_mouseJoint) {
		b2MouseJoint* _joint = reinterpret_cast<b2MouseJoint*>(joint);
        sq_pushfloat(v, _joint->GetFrequency());
    }
	
	return 1;
}

/*
 * call b2DistanceJoint->SetDampingRatio
 *
 * @param pointer of b2DistanceJoint
 * @param damping ratio
 * @return EMO_NO_ERROR if succeeds
 */
SQInteger emoPhysicsJoint_SetDampingRatio(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	if (joint->GetType() == e_distanceJoint) {
		b2DistanceJoint* _joint = reinterpret_cast<b2DistanceJoint*>(joint);
        _joint->SetDampingRatio(value);
    } else if (joint->GetType() == e_mouseJoint) {
		b2MouseJoint* _joint = reinterpret_cast<b2MouseJoint*>(joint);
        _joint->SetDampingRatio(value);
    }
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}

/*
 * call b2DistanceJoint->GetDampingRatio
 *
 * @param poiinter of b2DistanceJoint
 * @return damping ratio
 */
SQInteger emoPhysicsJoint_GetDampingRatio(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	if (joint->GetType() == e_distanceJoint) {
		b2DistanceJoint* _joint = reinterpret_cast<b2DistanceJoint*>(joint);
        sq_pushfloat(v, _joint->GetDampingRatio());
    } else if (joint->GetType() == e_mouseJoint) {
		b2MouseJoint* _joint = reinterpret_cast<b2MouseJoint*>(joint);
        sq_pushfloat(v, _joint->GetDampingRatio());
    }
	
	return 1;
}

/*
 * call b2FrictionJoint->SetMaxForce
 *
 * @param pointer of b2FrictionJoint
 * @param max force
 * @return EMO_NO_ERROR if succeeds
 */
SQInteger emoPhysicsJoint_SetMaxForce(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	SQFloat value;
	sq_getfloat(v, 3, &value);
	
	if (joint->GetType() == e_frictionJoint) {
		b2FrictionJoint* _joint = reinterpret_cast<b2FrictionJoint*>(joint);
        _joint->SetMaxForce(value);
	} else if (joint->GetType() == e_mouseJoint) {
		b2MouseJoint* _joint = reinterpret_cast<b2MouseJoint*>(joint);
        _joint->SetMaxForce(value);
	}
    
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}

/*
 * call b2FrictionJoint->GetMaxForce
 *
 * @param pointer of b2FrictionJoint
 * @return max force
 */
SQInteger emoPhysicsJoint_GetMaxForce(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2Joint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	if (joint->GetType() == e_frictionJoint) {
		b2FrictionJoint* _joint = reinterpret_cast<b2FrictionJoint*>(joint);
        sq_pushfloat(v, _joint->GetMaxForce());
    } else 	if (joint->GetType() == e_mouseJoint) {
		b2MouseJoint* _joint = reinterpret_cast<b2MouseJoint*>(joint);
        sq_pushfloat(v, _joint->GetMaxForce());
    }
	
	return 1;
}

/*
 * call b2FrictionJoint->SetMaxTorque
 *
 * @param pointer of b2FrictionJoint
 * @param max torque
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2FrictionJoint->GetMaxTorque
 *
 * @param pointer of b2FrictionJoint
 * @return max torque
 */
SQInteger emoPhysicsJoint_GetMaxTorque(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2FrictionJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetMaxTorque());
	
	return 1;
}

/*
 * call b2GearJoint->SetRatio
 *
 * @param pointer of b2GearJoint
 * @param ratio
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2GearJoint->GetRatio
 *
 * @param pointer of b2GearJoint
 * @return ratio
 */
SQInteger emoPhysicsJoint_GetRatio(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2GearJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetRatio());
	
	return 1;
}

/*
 * call b2Joint->GetJointTranslation
 *
 * @param pointer of b2Joint
 * @return joint translation
 */
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

/*
 * call b2Joint->GetJointSpeed
 *
 * @param pointer of b2Joint
 * @return joint speed
 */
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

/*
 * call b2RevoluteJoint->GetJointAngle
 *
 * @param pointer of b2RevoluteJoint
 * @return joint angle
 */
SQInteger emoPhysicsJoint_GetJointAngle(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2RevoluteJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetJointAngle());
	
	return 1;
}

/*
 * call b2Joint->IsLimitedEnabled
 *
 * @param pointer of b2Joint
 * @return limited is enabled or not
 */
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

/*
 * call b2Joint->EnableLimit
 *
 * @param pointer of b2Joint
 * @param enable limit or not
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2Joint->GetLowerLimit
 *
 * @param pointer of b2Joint
 * @return lower limit
 */
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

/*
 * call b2Joint->GetUpperLimit
 *
 * @param pointer of b2Joint
 * @return upper limit
 */
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

/*
 * call b2Joint->SetLimits
 *
 * @param pointer of b2Joint
 * @param lower limits
 * @param upper limits
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2Joint->IsMotorEnabled
 * 
 * @param pointer of b2Joint
 * @return motor is enabled or not
 */
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

/*
 * call b2Joint->EnableMotor
 *
 * @param pointer of b2Joint
 * @param enable motor or not
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2Joint->SetMotorSpeed
 *
 * @param pointer of b2Joint
 * @param motor speed
 * @param EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2Joint->SetMaxMotorForce
 *
 * @param pointer of b2Joint
 * @param max motor force
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2Joint->GetMotorForce
 *
 * @param pointer of b2Joint
 * @return motor force
 */
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

/*
 * call b2RevoluteJoint->SetMaxMotorTorque
 *
 * @param pointer of b2RevoluteJoint
 * @param max motor torque
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * call b2RevoluteJoint->GetMotorTorque
 *
 * @param pointer of b2RevoluteJoint
 * @return motor torque
 */
SQInteger emoPhysicsJoint_GetMotorTorque(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2RevoluteJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetMotorTorque());
	
	return 1;
}

/*
 * call b2PulleyJoint->GetGroundAnchorA
 *
 * @param pointer of b2PulleyJoint
 * @return ground anchor A
 */
SQInteger emoPhysicsJoint_GetGroundAnchorA(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2PulleyJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	pushVec2(v, joint->GetGroundAnchorA());
	
	return 1;
}

/*
 * call b2PulleyJoint->GetGroundAnchorB
 *
 * @param pointer of b2PulleyJoint
 * @return ground anchor B
 */
SQInteger emoPhysicsJoint_GetGroundAnchorB(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2PulleyJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	pushVec2(v, joint->GetGroundAnchorB());
	
	return 1;
}

/*
 * call b2PulleyJoint->GetLength1
 *
 * @param pointer of b2PulleyJoint
 * @return length 1
 */
SQInteger emoPhysicsJoint_GetLength1(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2PulleyJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetLength1());
	
	return 1;
}

/*
 * call b2PulleyJoint->GetLength2
 *
 * @param pointer of b2PulleyJoint
 * @return length 2
 */
SQInteger emoPhysicsJoint_GetLength2(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2PulleyJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	sq_pushfloat(v, joint->GetLength2());
	
	return 1;
}

/*
 * call b2MouseJoint->SetTarget
 *
 * @param pointer of b2MouoseJoint
 * @param target vector
 * @return EMO_NO_ERROR if succeeds
 */
SQInteger emoPhysicsJoint_SetTarget(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2MouseJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	b2Vec2 value;
	getVec2Instance(v, 3, &value);
    
    joint->SetTarget(value);
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}

/*
 * call b2MouseJoint->GetTarget
 *
 * @param pointer of b2MouoseJoint
 * @return target vector
 */
SQInteger emoPhysicsJoint_GetTarget(HSQUIRRELVM v) {
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		return 0;
	}
	b2MouseJoint* joint = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&joint);
	
	pushVec2(v, joint->GetTarget());
	
	return 1;
}

/*
 * update jointdef properties
 *
 * @param instance of b2JointDef
 * @return EMO_NO_ERROR if succeeds
 */
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
		case e_mouseJoint:
			getMouseJointDef(v, 3, reinterpret_cast<b2MouseJointDef*> (def));
			break;
		default:
			sq_pushinteger(v, ERR_INVALID_PARAM);
			return 1;
	}
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}

/*
 * initialize distance jointdef
 * 
 * @param instance of b2DistanceJointDef
 * @param pointer of b2Body A
 * @param pointer of b2Body B
 * @param anchor A
 * @param anchor B
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * initialize friction jointdef
 *
 * @param instance of b2FrictinJointDef
 * @param pointer of b2Body A
 * @param pointer of b2Body B
 * @param anchor
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * initialize LineJointDef
 *
 * @param instance of b2LineJointDef
 * @param pointer of b2Body A
 * @param pointer of b2Body B
 * @param anchor
 * @param axis
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * initialize PrismaticJointDef
 * 
 * @param instance of b2PrismaticJointDef
 * @param pointer of b2Body A
 * @param pointer of b2Body B
 * @param anchor
 * @param axis
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * initialize PulleyJointDef
 *
 * @param instance of b2PulleyJointDef
 * @param pointer of b2Body A
 * @param pointer of b2Body B
 * @param ground anchor A
 * @param ground anchor B
 * @param anchor A
 * @param anchor B
 * @param ratio
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * initialize RevoluteJointDef
 *
 * @param instance of b2RevoluteJointDef
 * @param pointer of b2Body A
 * @param pointer of b2Body B
 * @param anchor
 * @return EMO_NO_ERROR if succeeds
 */
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

/*
 * initialize WeldJointDef
 * 
 * @param instance of b2WeldJointDef
 * @param pointer of b2Body A
 * @param pointer of b2Body B
 * @param anchor
 * @return EMO_NO_ERROR if succeeds
 */
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
