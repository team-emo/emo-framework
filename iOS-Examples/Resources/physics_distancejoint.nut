/*
 * Physics example by Box2D with dynamic body and DistanceJoint.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("physics.nut");

local stage   = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 10), true);

const FPS = 60.0;

/*
 * This function is an example for retrieving
 * higher resolution image filename to support 
 * multiple screen size. (i.e. Retina and non-Retina).
 */
function getScaledImage(filename, baseWidth = 320) {
	local idx    = filename.find(".");
	local name   = filename.slice(0, idx);
	local suffix = filename.slice(idx);

	local scaling = (emo.Stage.getWindowWidth() / baseWidth).tointeger();
	
	if (scaling > 1) {
		name = name + "@" + scaling + "x";
	}
	
	return name + suffix;
}

class Main {
	sprite  = emo.Sprite(getScaledImage("tv.png"));
	jointAxis = emo.Rectangle();
	
	/*
	 * Called when this class is loaded
	 */
	function onLoad() {
		sprite.move(
			(stage.getWindowWidth()  - sprite.getWidth())  / 2,
			(stage.getWindowHeight() - sprite.getHeight()) / 2);

		local fixture = emo.physics.FixtureDef();
		fixture.density  = 0.1;
		fixture.friction = 0.2;
		fixture.restitution = 0.1;
		
		jointAxis.setSize(10, 10);
		jointAxis.move(
			(stage.getWindowWidth()  - jointAxis.getWidth())  / 2,
			sprite.getHeight());
		jointAxis.color(1, 0, 0);
	
		local spriteInfo = physics.createDynamicSprite(world, sprite, fixture);
		local axisInfo   = physics.createStaticSprite(world, jointAxis, fixture);
		
		local jointDef = emo.physics.DistanceJointDef();
		jointDef.initialize(
			spriteInfo.getBody(), axisInfo.getBody(),
			spriteInfo.getBody().getWorldCenter(), axisInfo.getBody().getWorldCenter());
		world.createJoint(jointDef);
	
		// draw the joint over the sprite
		sprite.setZ(0);
		jointAxis.setZ(1);

		// load the sprites
		sprite.load();
		jointAxis.load();
	
		// apply linear velocity to sprite to see how the DistanceJoint works.
		spriteInfo.getBody().setLinearVelocity(emo.Vec2(5, 0));
	
		// Set OnDrawCallback interval (millisecond)
		emo.Event.enableOnDrawCallback(1000.0 / FPS);
	}
	
    /*
     * Called when the onDraw callback is enabled by enableOnDrawCallback.
     * This callback is called before drawing the screen at interval of milliseconds set by enableOnDrawCallback.
     * dt is the actual delta time elapsed (milliseconds).
     */
    function onDrawFrame(dt) {
		// step the world (second)
		world.step(1.0 / FPS, 6, 2);
		world.clearForces();
	}
	
	/*
	 * Called when the class ends
	 */
	function onDispose() {
		sprite.remove();
		jointAxis.remove();
	}
}
function emo::onLoad() {
    stage.load(Main());
}
