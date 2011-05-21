/*
 * Physics example by Box2D with one dynamic circle body and static bodies.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("physics.nut");

local stage   = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 10), true);

local fps = 60.0;

class Main {
	ground  = emo.Rectangle();
	blockL  = emo.Rectangle();
	blockR  = emo.Rectangle();
	sprite  = emo.Sprite("block.png");
	
	function onLoad() {
		ground.setSize(stage.getWindowWidth(), 20);
		ground.move(0, stage.getWindowHeight() - ground.getHeight());
		
		blockL.setSize(10, 20);
		blockR.setSize(10, 20);
		
		blockL.move(0, stage.getWindowHeight() - ground.getHeight() - blockL.getHeight());
		blockR.move(stage.getWindowWidth()  - blockR.getWidth(), 
					stage.getWindowHeight() - ground.getHeight() - blockR.getHeight());
	
		sprite.move(0, 0);

		physics.createStaticSprite(world, ground);
		physics.createStaticSprite(world, blockL);
		physics.createStaticSprite(world, blockR);
		
		local fixture = emo.physics.FixtureDef();
		fixture.density  = 1.0;
		fixture.friction = 0.3;
		fixture.restitution = 0.2;
		physics.createDynamicCircleSprite(world, sprite, sprite.getWidth() * 0.5, fixture);
	
		ground.load();
		sprite.load();
		blockL.load();
		blockR.load();
	
		// set OnDrawCallback interval (millisecond)
		emo.Event.enableOnDrawCallback(1000.0 / fps);
	}
	
    function onDrawFrame(dt) {
		// world step interval(second)
		world.step(1.0 / fps, 6, 2);
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
