emo.Runtime.import("physics.nut");

function emo::onLoad() {
	local gravity = emo.Vec2(0, -10);
	local world = emo.physics.World(gravity, true);
	
	local groundBodyDef = emo.physics.BodyDef();
	groundBodyDef.position = emo.Vec2(0, -10);
	
	local groundBody = world.createBody(groundBodyDef);
	
	local groundBox = emo.physics.PolygonShape();
	groundBox.setAsBox(50, 10);
	groundBody.createFixture(groundBox, 0);

	local bodyDef = emo.physics.BodyDef();
	bodyDef.type = PHYSICS_BODY_TYPE_DYNAMIC;
	bodyDef.position = emo.Vec2(0, 4);
	local body = world.createBody(bodyDef);
	
	local dynamicBox = emo.physics.PolygonShape();
	dynamicBox.setAsBox(1, 1);
	
	local fixtureDef = emo.physics.FixtureDef();
	fixtureDef.shape = dynamicBox;
	fixtureDef.density  = 1;
	fixtureDef.friction = 0.3;
	body.createFixture(fixtureDef);
	
	local timeStep = 1.0 / 60.0;
	local velocityIterations = 6;
	local positionIterations  = 2;
	
	for (local i = 0; i < 60; ++i) {
		world.step(timeStep, velocityIterations, positionIterations);
		world.clearForces();
		
		local position = body.getPosition();
		local angle = emo.toDegree(body.getAngle());
		
		print(position.x + " " + position.y + " " + angle);
	}
}
