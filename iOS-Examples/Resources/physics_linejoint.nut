/*
 * Physics example by Box2D with dynamic body and PrismaticJoint.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("physics.nut");

local stage   = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 10), true);

const FPS = 60.0;

class Main {
    gear = emo.Sprite("gear@2x.png");
    axis = emo.Rectangle();
    rope = emo.Line();
    
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
        
        axis.setSize(stage.getWindowWidth() / 4, 2);
        axis.moveCenter(stage.getCenterX(), stage.getCenterY());
        gear.moveCenter(axis.getX() + axis.getWidth(), stage.getCenterY());
        
        local fixture = emo.physics.FixtureDef();
        fixture.density  = 0.1;
        fixture.friction = 0.2;
        fixture.restitution = 0.1;
    
        local gearInfo = physics.createDynamicCircleSprite(
                            world, gear, gear.getWidth() * 0.5, fixture);
        local axisInfo = physics.createStaticSprite(world, axis);
        
        local jointDef = emo.physics.LineJointDef();
        jointDef.initialize(
            gearInfo.getBody(), axisInfo.getBody(),
            gearInfo.getBody().getWorldCenter(), emo.Vec2(1, 0));
            
        jointDef.enableMotor = true;
        jointDef.maxMotorForce = 1;
        jointDef.motorSpeed = 1;
        jointDef.enableLimit = true;
        jointDef.lowerTranslation = 0;
        jointDef.upperTranslation = axis.getWidth()  / world.getScale();
        
        world.createJoint(jointDef);

        axis.setZ(0);
        gear.setZ(1);

        // load the sprites
        axis.load();
        gear.load();
        
        rope.move(axis.getX() + axis.getWidth(), stage.getCenterY(), gear.getCenterX(), gear.getCenterY());
        rope.color(0.41, 0.41, 0.41);
        rope.setWidth(2);
        rope.load();
    
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
        
        rope.move(axis.getX() + axis.getWidth(), stage.getCenterY(), gear.getCenterX(), gear.getCenterY());
    }
    
    /*
     * Called when the class ends
     */
    function onDispose() {
        gear.remove();
        axis.remove();
    }
}
function emo::onLoad() {
    stage.load(Main());
}
