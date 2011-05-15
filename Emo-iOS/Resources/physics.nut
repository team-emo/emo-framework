class emo.Vec2 {
	x = null;
	y = null;
	function constructor(_x, _y) {
		x = _x;
		y = _y;
	}
	
	function set(_x, _y) {
		x = _x;
		y = _y;
	}
}

emo.physics <- {};

class emo.physics.World {
	id     = null;
	alive = null;
	function constructor(gravity, doSleep) {
		id = emo.Physics.newWorld(gravity, doSleep);
		alive = true;
	}
	
	function close() {
		emo.Physics.deleteWorld(id);
		alive = false;
	}
}

class emo.physics.AABB {
	lowerBound = null;
	upperBound = null;
}

class emo.physics.Body {
	id = null;
}

class emo.physics.BodyDef {
	type            = null;
	position        = null;
	angle           = null;
	linearVelocity  = null;
	angularVelocity = null;
	linearDamping   = null;
	angularDamping  = null;
	allowSleep      = null;
	awake           = null;
	fixedRotation   = null;
	bullet          = null;
	active          = null;
	userData        = null;
	inertiaScale    = null;
}

class emo.physics.Fixture {
	id = null;
}

class emo.physics.FixtureDef {
	id          = null;
	shape       = null;
	friction    = null;
	restitution = null;
	density     = null;
	isSensor    = null;
	filter      = null;
}