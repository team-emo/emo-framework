emo.Runtime.import("physics.nut");

function emo::onLoad() {
	local gravity = emo.Vec2(0, -10);
	local world = emo.physics.World(gravity, true);
	
	local groundBodyDef = emo.physics.BodyDef();
	groundBodyDef.position = emo.Vec2(0, -10);
	
	local groundBody = world.createBody(groundBodyDef);
	local groundBox = emo.physics.PolygonShape();
	local fixture = groundBody.createFixture(groundBox, 0);
	
	print(groundBox.setAsBox(50, 10));
	print(groundBox.getVertexCount());
	
	local v = groundBox.getVertex(0);
	
	print(v.x);
	print(v.y);
}
