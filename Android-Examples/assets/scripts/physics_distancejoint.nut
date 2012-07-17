/*
 * Physics example by Box2D with dynamic body and DistanceJoint.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("emo-framework/physics.nut");

local stage   = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 10), true);

const FPS = 60.0;
const WALL_WIDTH = 10;

class Main {
    box     = emo.SpriteSheet("blocks.png", 32, 32, 2, 2);
    axis    = emo.Rectangle();
    rope    = emo.Line();
    dropBox = emo.SpriteSheet("blocks.png", 32, 32, 2, 2);

    lastRecycledTime = 0;
    
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
        
        createWall();
        
        box.setSize(stage.getWindowWidth() / 4, 20);
        box.move(
            (stage.getWindowWidth()  - box.getWidth())  / 2,
            (stage.getWindowHeight() - box.getHeight()) / 2);
        
        dropBox.setSize(box.getWidth(), 10);
        dropBox.hide();
        dropBox.load();
    
        axis.setSize(10, 10);
        axis.move(
            (stage.getWindowWidth()  - axis.getWidth())  / 2,
            box.getHeight());
        axis.color(0.41, 0.41, 0.41);
        
        rope.color(0.41, 0.41, 0.41);
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
    
        box.setFrame(7);
        dropBox.setFrame(6);

        // apply linear velocity to sprite to see how the DistanceJoint works.
        boxInfo.getBody().setLinearVelocity(emo.Vec2(5, 0));
    
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
        world.step(dt / 1000.0, 6, 2);
        world.clearForces();
        
        rope.move(
            axis.getCenterX(), axis.getCenterY(),
            box.getCenterX(),  box.getCenterY());
        
        if (emo.Runtime().uptime() - lastRecycledTime > 1000) {
            lastRecycledTime = emo.Runtime().uptime();
            
            // if the drop box is sleeping, drop it again.
            if (dropBox.getPhysicsInfo() == null ||
                 !dropBox.getPhysicsInfo().getBody().isAwake()) {
                local dropX = (stage.getWindowWidth() - box.getWidth()) / 2 + (rand() % box.getWidth());
                addNewDropBox(dropX, 100, rand() % 180);
            }
        }
    }
    
    /*
     * Called when the class ends
     */
    function onDispose() {
        box.remove();
        axis.remove();
        dropBox.remove();
    }
    
    function addNewDropBox(x, y, angle) {
        dropBox.hide();
        dropBox.rotate(angle);
        
        if (dropBox.getPhysicsInfo() != null) {
            dropBox.getPhysicsInfo().remove();
            dropBox.physicsInfo = null;
        }

        dropBox.move(x, y);
        
        local fixture = emo.physics.FixtureDef();
        fixture.density  = 0.1;
        physics.createDynamicSprite(world, dropBox, fixture);
        
        dropBox.show();
    }
    
    /*
     * drop boxes at touch event.
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            addNewDropBox(mevent.getX(), mevent.getY(), rand() % 180);
        }
    }
}
function emo::onLoad() {
    stage.load(Main());
}
