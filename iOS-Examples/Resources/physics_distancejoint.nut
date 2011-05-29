/*
 * Physics example by Box2D with dynamic body and DistanceJoint.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("physics.nut");

local stage   = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 10), true);

const FPS = 60.0;

class Main {
	ground = emo.Rectangle();
	box    = emo.Rectangle();
	axis   = emo.Rectangle();
	rope   = emo.Line();
	
	/*
	 * Called when this class is loaded
	 */
	function onLoad() {
		ground.setSize(stage.getWindowWidth(), 20);
		ground.move(0, stage.getWindowHeight() - ground.getHeight());
		physics.createStaticSprite(world, ground);
		ground.load();
		
		box.setSize(stage.getWindowWidth() / 4, 20);
		box.move(
			(stage.getWindowWidth()  - box.getWidth())  / 2,
			(stage.getWindowHeight() - box.getHeight()) / 2);
		box.color(0, 0, 1);
	
		axis.setSize(10, 10);
		axis.move(
			(stage.getWindowWidth()  - axis.getWidth())  / 2,
			box.getHeight());
		axis.color(1, 0, 0);
		
		local fixture = emo.physics.FixtureDef();
		fixture.density  = 0.1;
		fixture.friction = 0.2;
		fixture.restitution = 0.1;
		
		local boxInfo  = physics.createDynamicSprite(world, box, fixture);
		local axisInfo = physics.createStaticSprite(world, axis, fixture);
		
		local jointDef = emo.physics.DistanceJointDef();
		jointDef.initialize(
			boxInfo.getBody(), axisInfo.getBody(),
			boxInfo.getBody().getWorldCenter(), axisInfo.getBody().getWorldCenter());
		world.createJoint(jointDef);
	
		// draw the joint over the sprite
		rope.setZ(0);
		box.setZ(1);
		axis.setZ(2);

		// load the sprites
		box.load();
		axis.load();
		
		rope.setWidth(2);
		rope.load();
	
		// apply linear velocity to sprite to see how the DistanceJoint works.
		boxInfo.getBody().setLinearVelocity(emo.Vec2(5, 0));
	
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
		
		rope.move(
			axis.getCenterX(), axis.getCenterY(),
			box.getCenterX(),  box.getCenterY());
	}
	
	/*
	 * Called when the class ends
	 */
	function onDispose() {
		box.remove();
		axis.remove();
	}
	
	/*
	 * touch event
	 */
	function onMotionEvent(mevent) {
		if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
			local dropBox = emo.Rectangle();
			dropBox.setSize(20, 20);
			dropBox.move(mevent.getX(), mevent.getY());
			dropBox.color(0, 1, 0);
			
			local fixture = emo.physics.FixtureDef();
			fixture.density  = 0.1;
		
			physics.createDynamicSprite(world, dropBox, fixture);
		
			dropBox.load();
		}
	}
}
function emo::onLoad() {
    stage.load(Main());
}
