/*
 * Physics example by Box2D with dynamic body and GearJoint.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("physics.nut");

local stage   = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 10), true);

const FPS = 60.0;

class Main {
    gear1 = emo.Sprite("gear.png");
    gear2 = emo.Sprite("gear@2x.png");
    box1    = emo.Rectangle();
    box2    = emo.Rectangle();
    
    /*
     * Called when this class is loaded
     */
    function onLoad() {
    
        // Below statements is an example of multiple screen density support.
        // (i.e. Retina vs non-Retina, cellular phone vs tablet device).
        if (stage.getWindowWidth() > 320) {
            // if the screen has large display, scale contents twice
            // that makes the stage size by half.
            // This examples shows how to display similar-scale images
            // on Retina and non-Retina display.
            stage.setContentScale(stage.getWindowWidth() / 320.0);
        }
    
        box1.setSize(10, 10);
        box1.moveCenter(stage.getCenterX(), stage.getCenterY());
        box1.color(0.83, 0.83, 0.83);
        
        gear1.moveCenter(box1.getCenterX(), box1.getCenterY());
        
        box2.setSize(20, 20);
        box2.moveCenter(stage.getCenterX(), gear1.getY() + gear2.getHeight());
        box2.color(0.83, 0.83, 0.83);
        
        gear2.moveCenter(box2.getCenterX(), box2.getCenterY());
        
        local fixture = emo.physics.FixtureDef();
        fixture.density  = 0.1;
        fixture.friction = 0.2;
        fixture.restitution = 0.1;
    
        local gear1Info = physics.createDynamicCircleSprite(
                            world, gear1, gear1.getWidth() * 0.5, fixture);
        local gear2Info = physics.createDynamicCircleSprite(
                            world, gear2, gear2.getWidth() * 0.5, fixture);
        local box1Info  = physics.createStaticSprite(world, box1, fixture);
        local box2Info  = physics.createStaticSprite(world, box2, fixture);
        
        local jointDef1 = emo.physics.RevoluteJointDef();
        jointDef1.initialize(
            box1Info.getBody(), gear1Info.getBody(),
            box1Info.getBody().getWorldCenter());
        jointDef1.enableMotor = true;
        jointDef1.maxMotorTorque = 10;
        jointDef1.motorSpeed = 1;
        
        local joint1 = world.createJoint(jointDef1);

        local jointDef2 = emo.physics.RevoluteJointDef();
        jointDef2.enableMotor = false;
        jointDef2.initialize(
            box2Info.getBody(), gear2Info.getBody(),
            box2Info.getBody().getWorldCenter());
        local joint2 = world.createJoint(jointDef2);
        
        local gearJointDef = emo.physics.GearJointDef();
        gearJointDef.bodyA  = gear1Info.getBody();
        gearJointDef.bodyB  = gear2Info.getBody();
        gearJointDef.joint1 = joint1;
        gearJointDef.joint2 = joint2;
        gearJointDef.ratio = 2;
        world.createJoint(gearJointDef);
    
        box1.setZ(0);
        box2.setZ(0);
        gear1.setZ(1);
        gear2.setZ(1);

        // load the sprites
        gear1.load();
        gear2.load();
        box1.load();
        box2.load();
    
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
        world.step(dt / 1000.0, 6, 2);
        world.clearForces();
    }
    
    /*
     * Called when the class ends
     */
    function onDispose() {
        gear1.remove();
        gear2.remove();
        box1.remove();
        box2.remove();
    }
}
function emo::onLoad() {
    stage.load(Main());
}
