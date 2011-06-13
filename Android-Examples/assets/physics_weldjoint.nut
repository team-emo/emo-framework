/*
 * Physics example by Box2D with dynamic body and WeldJoint.
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
        
    axis = emo.Rectangle();
    boxes = [];
    
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
        
        axis.setSize(40, 10);
        axis.move(0, stage.getCenterY());
        
        local fixture = emo.physics.FixtureDef();
        fixture.density  = 0.1;
        fixture.friction = 0.2;
        fixture.restitution = 0.1;
    
        local axisInfo = physics.createStaticSprite(world, axis);
        local prevBody = axisInfo.getBody();
        
        local jointDef = emo.physics.WeldJointDef();
            
        for (local i = 1; i <= 5; i++) {
            local box = emo.Rectangle();
            box.setSize(40, 10);
            box.move(axis.getX() + (i * axis.getWidth()), axis.getY());
            box.color(0.2* i, 0.2* i, 0.2* i, 1);
            
            box.setZ(i);
            box.load();
            
            boxes.append(box);
        
            local boxInfo = physics.createDynamicSprite(world, box, fixture);
        
            jointDef.initialize(
                prevBody, boxInfo.getBody(),
                boxInfo.getBody().getWorldCenter());
            world.createJoint(jointDef);
            
            prevBody = boxInfo.getBody();
        }

        axis.setZ(0);

        // load the sprites
        axis.load();
    
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
    }
    
    /*
     * Apply linear velocity when user touches the screen.
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            boxes[boxes.len() - 1].getPhysicsBody().setLinearVelocity(emo.Vec2(0, 5));
        }
    }
    
    /*
     * Called when the class ends
     */
    function onDispose() {
        axis.remove();
        for (local i = 0; i < boxes.len(); i++) {
            boxes[i].remove();
        }
        boxes.clear();
    }
}
function emo::onLoad() {
    stage.load(Main());
}
