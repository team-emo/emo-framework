#include "Box2D/Box2D.h"
#include "squirrel.h"

#if __cplusplus
extern "C" {
#endif
#include "Physics_glue.h"
#include "Constants.h"
#include "VmFunc.h"
#include "stdio.h"

extern void LOGI(const char* msg);
extern void LOGW(const char* msg);
extern void LOGE(const char* msg);

static void getVec2Instance(HSQUIRRELVM v, int idx, b2Vec2* vec2) {
	getInstanceMemberAsFloat(v, idx, "x", &vec2->x);
	getInstanceMemberAsFloat(v, idx, "y", &vec2->y);
}

static void getVec2InstanceFromMember(HSQUIRRELVM v, int idx, const char* member, b2Vec2* vec2) {
	getInstanceMemberAsTable(v, idx, member, "x", &vec2->x);
	getInstanceMemberAsTable(v, idx, member, "y", &vec2->x);
}
	
static void getBodyDefInstance(HSQUIRRELVM v, int idx, b2BodyDef* def) {
	getVec2InstanceFromMember(v, idx, "position", &def->position);
	getInstanceMemberAsFloat(v,  idx, "angle",    &def->angle);
	getVec2InstanceFromMember(v, idx, "linearVelocity",  &def->linearVelocity);
	getInstanceMemberAsFloat(v,  idx, "angularVelocity", &def->angularVelocity);
	getInstanceMemberAsFloat(v,  idx, "linearDamping",   &def->linearDamping);
	getInstanceMemberAsFloat(v,  idx, "angularDamping",  &def->angularDamping);
	getInstanceMemberAsBool(v,   idx, "allowSleep",      &def->allowSleep);
	getInstanceMemberAsBool(v,   idx, "awake",           &def->awake);
	getInstanceMemberAsBool(v,   idx, "fixedRotation",   &def->fixedRotation);
	getInstanceMemberAsBool(v,   idx, "bullet",          &def->bullet);
	getInstanceMemberAsBool(v,   idx, "active",          &def->active);
	getInstanceMemberAsFloat(v,  idx, "inertiaScale",    &def->inertiaScale);
}
	
static void getFixtureDefInstance(HSQUIRRELVM v, int idx, b2FixtureDef* def) {
	getInstanceMemberAsFloat(v,  idx, "friction",    &def->friction);
	getInstanceMemberAsFloat(v,  idx, "restitution", &def->restitution);
	getInstanceMemberAsFloat(v,  idx, "density",     &def->density);
	getInstanceMemberAsBool(v,   idx, "isSensor",    &def->isSensor);
	
	SQUserPointer ptr_shape;
	if (getInstanceMemberAsUserPointer(v, idx, "shape", "id", &ptr_shape)) {
		def->shape = reinterpret_cast<b2Shape*>(ptr_shape); 
	}
}
static SQInteger b2WorldReleaseHook(SQUserPointer ptr, SQInteger size) {
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
    SQInteger nargs = sq_gettop(v);
	
	b2Vec2 gravity;
	if (nargs >= 2 && sq_gettype(v, 2) == OT_INSTANCE) {
		getVec2Instance(v, 2, &gravity);
	} else {
		gravity.Set(0, SENSOR_STANDARD_GRAVITY);
	}
	SQBool doSleep = true;
	if (nargs >= 3 && sq_gettype(v, 3) == OT_INTEGER) {
		SQInteger val;
		sq_getinteger(v, 3, &val);
		doSleep = (val == EMO_YES);
	}
	
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
	if (sq_gettype(v, 2) != OT_USERPOINTER || sq_gettype(v, 3) != OT_INSTANCE) {
		return 0;
	}
	b2World* world = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&world);
	
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
	if (sq_gettype(v, 2) != OT_USERPOINTER || sq_gettype(v, 3) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&world);
	
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
	if (sq_gettype(v, 2) != OT_USERPOINTER || sq_gettype(v, 3) != OT_USERPOINTER) {
		return 0;
	}
	b2World* world = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&world);
	
	b2JointDef* def = NULL;
	sq_getuserpointer(v, 3, (SQUserPointer*)&def);
	
	sq_pushuserpointer(v, world->CreateJoint(def));
	
	return 1;
}
SQInteger emoPhysicsDestroyJoint(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	if (nargs < 3) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	if (sq_gettype(v, 2) != OT_USERPOINTER || sq_gettype(v, 3) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&world);
	
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
	if (sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&world);
	
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
SQInteger emoPhysicsWorld_ClearForces(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	if (nargs < 2 || sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world = NULL;
	sq_getuserpointer(v, 2, (SQUserPointer*)&world);
	
	world->ClearForces();
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
SQInteger emoPhysicsCreateFixture(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	if (nargs < 3) {
		return 0;
	}
	if (sq_gettype(v, 2) != OT_USERPOINTER && sq_gettype(v, 3) != OT_INSTANCE) {
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
	if (sq_gettype(v, 2) != OT_USERPOINTER && sq_gettype(v, 3) != OT_USERPOINTER) {
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
    SQInteger nargs = sq_gettop(v);
	SQInteger sType = PHYSICS_SHAPE_UNKNOWN;
	if (nargs >= 2 && sq_gettype(v, 2) == OT_INTEGER) {
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
#if __cplusplus
}   // Extern C
#endif
