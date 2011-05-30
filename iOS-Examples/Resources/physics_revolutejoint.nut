/*
 * Physics example by Box2D with dynamic body and RevoluteJoint.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("physics.nut");

local stage   = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 10), true);

const FPS = 60.0;
const WALL_WIDTH = 10;

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
	wheel1 = emo.Sprite(getScaledImage("ball.png"));
	wheel2 = emo.Sprite(getScaledImage("ball.png"));
	axis   = emo.Rectangle();
	box    = emo.Rectangle();
	
	/*
	 * Called when this class is loaded
	 */
	function onLoad() {
	
		createWall();
	
		box.setSize(wheel1.getWidth() * 3, wheel1.getHeight());
		box.move(stage.getWindowWidth() - box.getWidth(), 0);
		box.color(0, 0, 1);
		
		wheel1.move(box.getX(), 
					box.getY() + (box.getHeight() / 4));
		wheel2.move(box.getX() + box.getWidth() - wheel2.getWidth(), 
					box.getY() + (box.getHeight() / 4));
		
		local fixture = emo.physics.FixtureDef();
		fixture.density  = 0.1;
		fixture.friction = 0.2;
		fixture.restitution = 0.1;
	
		local wheel1Info = physics.createDynamicCircleSprite(
							world, wheel1, wheel1.getWidth() * 0.5, fixture);
		local wheel2Info = physics.createDynamicCircleSprite(
							world, wheel2, wheel2.getWidth() * 0.5, fixture);
		local boxInfo    = physics.createDynamicSprite(world, box, fixture);
		
		local jointDef = emo.physics.RevoluteJointDef();
		jointDef.initialize(
			wheel1Info.getBody(), boxInfo.getBody(),
			wheel1Info.getBody().getWorldCenter());
		jointDef.enableMotor = true;
		jointDef.maxMotorTorque = 10;
		jointDef.motorSpeed = 1;
		world.createJoint(jointDef);

		jointDef.initialize(
			wheel2Info.getBody(), boxInfo.getBody(),
			wheel2Info.getBody().getWorldCenter());
		jointDef.enableMotor = true;
		jointDef.maxMotorTorque = 10;
		jointDef.motorSpeed = 1;
		world.createJoint(jointDef);
	
		box.setZ(0);
		wheel1.setZ(1);
		wheel2.setZ(1);

		// load the sprites
		wheel1.load();
		wheel2.load();
		box.load();
	
		// Set OnDrawCallback interval (millisecond)
		emo.Event.enableOnDrawCallback(1000.0 / FPS);
	}
	
	function createWall() {
		local wall1  = emo.Rectangle();
		local wall2  = emo.Rectangle();
		local wall3  = emo.Rectangle();
		local wall4  = emo.Rectangle();

		wall1.setSize(stage.getWindowWidth(), WALL_WIDTH);
		wall2.setSize(stage.getWindowWidth(), WALL_WIDTH);
		wall3.setSize(WALL_WIDTH, stage.getWindowHeight());
		wall4.setSize(WALL_WIDTH, stage.getWindowHeight());
		
		wall1.move(0, 0);
		wall2.move(0, stage.getWindowHeight() - wall2.getHeight());
		wall3.move(0, 0);
		wall4.move(stage.getWindowWidth() - wall4.getWidth(), 0);
		
		physics.createStaticSprite(world, wall1);
		physics.createStaticSprite(world, wall2);
		physics.createStaticSprite(world, wall3);
		physics.createStaticSprite(world, wall4);
		
		wall1.load();
		wall2.load();
		wall3.load();
		wall4.load();
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
		wheel1.remove();
		box.remove();
	}
}
function emo::onLoad() {
    stage.load(Main());
}
