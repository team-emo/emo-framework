#include "Box2D/Box2D.h"

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

static void updateVec2(HSQUIRRELVM v, int idx, b2Vec2* vec2) {
	getInstanceMemberAsFloat(v, idx, "x", &vec2->x);
	getInstanceMemberAsFloat(v, idx, "y", &vec2->y);
}

SQInteger emoPhysicsNewWorld(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	
	b2Vec2 gravity;
	if (nargs >= 2 && sq_gettype(v, 2) == OT_INSTANCE) {
		updateVec2(v, 2, &gravity);
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
SQInteger emoPhysicsDeleteWorld(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
	
	if (nargs >= 2 && sq_gettype(v, 2) != OT_USERPOINTER) {
		sq_pushinteger(v, ERR_INVALID_PARAM);
		return 1;
	}
	b2World* world;
	sq_getuserpointer(v, 2, (SQUserPointer*)&world);
	delete world;
	
	sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}
#if __cplusplus
}   // Extern C
#endif
