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
SQInteger emoPhysicsWorld_SetAutoClearForces(HSQUIRRELVM v);
SQInteger emoPhysicsWorld_GetAutoClearForces(HSQUIRRELVM v);
SQInteger emoPhysicsPolygonShape_Set(HSQUIRRELVM v);
SQInteger emoPhysicsPolygonShape_SetAsBox(HSQUIRRELVM v);
SQInteger emoPhysicsPolygonShape_SetAsEdge(HSQUIRRELVM v);
SQInteger emoPhysicsPolygonShape_GetVertex(HSQUIRRELVM v);
SQInteger emoPhysicsPolygonShape_GetVertexCount(HSQUIRRELVM v);
