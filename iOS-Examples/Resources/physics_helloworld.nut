/*
 * Physics example by Box2D with one dynamic body and one static body.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("physics.nut");

local stage   = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 10), true);

const FPS = 60.0;

class Main {
	ground  = emo.Rectangle();
	sprite  = emo.Sprite("tv.png");
	
	function onLoad() {
		ground.setSize(stage.getWindowWidth(), 20);
		ground.move(0, stage.getWindowHeight() - ground.getHeight());
	
		sprite.move((stage.getWindowWidth() - sprite.getWidth()) / 2, 0);

		physics.createStaticSprite(world, ground);
		physics.createDynamicSprite(world, sprite);
	
		ground.load();
		sprite.load();
	
		// Set OnDrawCallback interval (millisecond)
		emo.Event.enableOnDrawCallback(1000.0 / FPS);
	}
	
    function onDrawFrame(dt) {
		// step the world (second)
		world.step(1.0 / FPS, 6, 2);
		world.clearForces();
	}
	
	function onDispose() {
		ground.remove();
		sprite.remove();
	}
}
function emo::onLoad() {
    stage.load(Main());
}
