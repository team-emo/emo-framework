emo.Runtime.import("physics.nut");

function emo::onLoad() {
	local gravity = emo.Vec2(0, 10);
	local world = emo.physics.World(gravity, EMO_YES);
	world.close();
}
