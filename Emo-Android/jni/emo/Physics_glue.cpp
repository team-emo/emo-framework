#include "Box2D/Box2D.h"
#include "squirrel.h"
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
	
static SQInteger b2ShapeReleaseHook(SQUserPointer ptr, SQInteger size) {
	delete reinterpret_cast<b2Shape*>(ptr);
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
	
	sq_pushuserpointer(v, world);
	return 1;
}
SQInteger emoPhysicsDeleteObj(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	
	if (nargs >= 2 && sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	SQUserPointer* obj;
	sq_getuserpointer(v, 2, (SQUserPointer*)&obj);
	delete obj;
	
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
