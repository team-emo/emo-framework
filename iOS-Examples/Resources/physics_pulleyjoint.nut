/*
 * Physics example by Box2D with dynamic body and PulleyJoint.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("physics.nut");

local stage   = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 10), true);

const FPS = 60.0;

class Main {

    // 16x16 text sprite with 2 pixel border and 1 pixel margin
    text = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
    
    box1  = emo.SpriteSheet("blocks.png", 32, 32, 2, 2);
    box2  = emo.SpriteSheet("blocks.png", 32, 32, 2, 2);
    axis1 = emo.Rectangle();
    axis2 = emo.Rectangle();
    rope1 = emo.Line();
    rope2 = emo.Line();
    rope3 = emo.Line();
    
    box1Info = null;
    
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
        
        box1.move(stage.getWindowWidth() / 4, stage.getWindowHeight() - box1.getHeight());
    
        box2.setSize(box1.getWidth() * 1.1, box1.getHeight() * 1.1);
        box2.move(stage.getWindowWidth() / 4 * 2, box1.getHeight());
        
        axis1.setSize(10, 10);
        axis1.move(box1.getX(), box1.getHeight());
        axis1.color(0.41, 0.41, 0.41);
        
        axis2.setSize(10, 10);
        axis2.move(box2.getX(), box1.getHeight());
        axis2.color(0.41, 0.41, 0.41);
        
        rope1.color(0.41, 0.41, 0.41);
        rope2.color(0.41, 0.41, 0.41);
        rope3.color(0.41, 0.41, 0.41);
        
        rope1.setWidth(2);
        rope2.setWidth(2);
        rope3.setWidth(2);
        
        local fixture = emo.physics.FixtureDef();
        fixture.density  = 0.1;
        fixture.friction = 0.2;
        fixture.restitution = 0.1;
        
        box1Info  = physics.createDynamicSprite(world, box1, fixture);
        
        local box2Info  = physics.createDynamicSprite(world, box2, fixture);
        local axis1Info = physics.createStaticSprite(world, axis1, fixture);
        local axis2Info = physics.createStaticSprite(world, axis2, fixture);
        
        local jointDef = emo.physics.PulleyJointDef();
        jointDef.initialize(
            box1Info.getBody(),  box2Info.getBody(),
            axis1Info.getBody().getWorldCenter(),
            axis2Info.getBody().getWorldCenter(),
            box1Info.getBody().getWorldCenter(),
            box2Info.getBody().getWorldCenter(), 1.0);
        world.createJoint(jointDef);
        
        rope1.move(axis1.getCenterX(), axis1.getCenterY(),
                   box1.getCenterX(),  box1.getCenterY());
                   
        rope2.move(axis2.getCenterX(), axis2.getCenterY(),
                   box2.getCenterX(),  box2.getCenterY());
                   
        rope3.move(axis1.getCenterX(), axis1.getCenterY(),
                   axis2.getCenterX(), axis2.getCenterY());
        
        box1.setZ(2);
        box2.setZ(2);
        axis1.setZ(1);
        axis2.setZ(1);
        rope1.setZ(0);
        rope2.setZ(0);
        rope3.setZ(0);
        
        box1.load();
        box2.load();
        axis1.load();
        axis2.load();
        rope1.load();
        rope2.load();
        rope3.load();
    
        box1.setFrame(7);
        box2.setFrame(6);

        // change the text
        text.setText("TAP TO ADD VELOCITY");
        text.scale(0.7, 0.7);
        
        local tX = (stage.getWindowWidth()  - text.getScaledWidth())  / 2;
        text.move(tX, text.getScaledHeight());

        text.load();
        
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
        
        rope1.move(axis1.getCenterX(), axis1.getCenterY(),
                   box1.getCenterX(),  box1.getCenterY());
                   
        rope2.move(axis2.getCenterX(), axis2.getCenterY(),
                   box2.getCenterX(),  box2.getCenterY());
    }

    /*
     * Apply linear velocity when user touches the screen.
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            box1Info.getBody().setLinearVelocity(emo.Vec2(0, 5));
        }
    }
    
    /*
     * Called when the class ends
     */
    function onDispose() {
        box1.remove();
        box2.remove();
        axis1.remove();
        axis2.remove();
    }
}
function emo::onLoad() {
    stage.load(Main());
}
