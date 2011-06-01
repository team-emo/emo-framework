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

#include "Physics_contact.h"
#include "Physics_util.h"
#include "Constants.h"
	
extern void LOGI(const char* msg);
extern void LOGW(const char* msg);
extern void LOGE(const char* msg);

namespace emo {
	/*
	 * invoke contact event
	 */
	static SQBool invokeContactEvent(HSQUIRRELVM v, ContactPoint cp) {
		SQBool result = false;
		SQInteger top = sq_gettop(v);
		sq_pushroottable(v);
		sq_pushstring(v, "emo", -1);
		if (SQ_SUCCEEDED(sq_get(v, -2))) {
			sq_pushstring(v, "_onContact", -1);
			if(SQ_SUCCEEDED(sq_get(v, -2))) {
				sq_pushroottable(v);
				switch(cp.state) {
					case b2_addState:
						sq_pushinteger(v, PHYSICS_STATE_ADD);
						break;
					case b2_persistState:
						sq_pushinteger(v, PHYSICS_STATE_PERSIST);
						break;
					case b2_removeState:
						sq_pushinteger(v, PHYSICS_STATE_REMOVE);
						break;
					default:
						sq_pushinteger(v, PHYSICS_STATE_NULL);
				}
				sq_pushuserpointer(v, cp.fixtureA);
				sq_pushuserpointer(v, cp.fixtureB);
				sq_pushuserpointer(v, cp.fixtureA->GetBody());
				sq_pushuserpointer(v, cp.fixtureB->GetBody());
				pushVec2(v, cp.position);
				pushVec2(v, cp.normal);
				sq_pushfloat(v, cp.normalImpulse);
				sq_pushfloat(v, cp.tangentImpulse);
				
				result = SQ_SUCCEEDED(sq_call(v, 10, SQFalse, SQTrue));
			}
		}
		sq_settop(v,top);
		
		return result;
	}
	
	EmoPhysicsContactListener::EmoPhysicsContactListener(HSQUIRRELVM v) {
		this->sqvm = v;
		this->enableNullEvent    = false;
		this->enableAddEvent     = true;
		this->enablePersistEvent = true;
		this->enableRemoveEvent  = true;
	}

	EmoPhysicsContactListener::~EmoPhysicsContactListener() {
		
	}
	
	void EmoPhysicsContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
		const b2Manifold* manifold = contact->GetManifold();
		
		if (manifold->pointCount == 0)
		{
			return;
		}
		
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();
		
		b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
		b2GetPointStates(state1, state2, oldManifold, manifold);
		
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);
		
		for (int32 i = 0; i < manifold->pointCount; ++i)
		{
			if (!this->enableNullEvent    && state2[i] == b2_nullState)    continue;
			if (!this->enableAddEvent     && state2[i] == b2_addState)     continue;
			if (!this->enablePersistEvent && state2[i] == b2_persistState) continue;
			if (!this->enableRemoveEvent  && state2[i] == b2_removeState)  continue;
			
			ContactPoint cp;
			cp.fixtureA = fixtureA;
			cp.fixtureB = fixtureB;
			cp.position = worldManifold.points[i];
			cp.normal = worldManifold.normal;
			cp.normalImpulse  = manifold->points[i].normalImpulse;
			cp.tangentImpulse = manifold->points[i].tangentImpulse;
			cp.state = state2[i];
			
			invokeContactEvent(sqvm, cp);
		}
		
	}
}
#if __cplusplus
}   // Extern C
#endif
