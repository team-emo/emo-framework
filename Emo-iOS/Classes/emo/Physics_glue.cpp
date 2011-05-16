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
	
static SQInteger b2WorldReleaseHook(SQUserPointer ptr, SQInteger size) {
	delete reinterpret_cast<b2World*>(ptr);
	return 0;
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
	
	SQInteger result = createSQObject(v, 
				"emo", "Instance", world, b2WorldReleaseHook);
	if (result == 0) {
		delete world;
		return 0;
	}
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
