/*
 * Box2D Contact Listener
 */
namespace emo {
	class EmoPhysicsContactListener : public b2ContactListener {
	public:
		EmoPhysicsContactListener(HSQUIRRELVM v);
		~EmoPhysicsContactListener();
		
		virtual void BeginContact(b2Contact* contact) { B2_NOT_USED(contact); };
		virtual void EndContact(b2Contact* contact) { B2_NOT_USED(contact); };
		virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
		virtual void PostSolve(const b2Contact* contact, const b2ContactImpulse* impulse) {
			B2_NOT_USED(contact);
			B2_NOT_USED(impulse);
		};
		HSQUIRRELVM sqvm;
	};

	struct ContactPoint
	{
		b2Fixture* fixtureA;
		b2Fixture* fixtureB;
		b2Vec2 normal;
		b2Vec2 position;
		b2PointState state;
		float32 normalImpulse;
		float32 tangentImpulse;
	};
}