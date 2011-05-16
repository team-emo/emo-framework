emo.Runtime.import("physics.nut");

function emo::onLoad() {
	local gravity = emo.Vec2(0, 10);
	local world = emo.physics.World(gravity, EMO_YES);
	
	local shape = emo.physics.PolygonShape();
	world.close();
	print(shape);
}

function emo::onLostFocus() {
	print(emo.Runtime.gc());
}
