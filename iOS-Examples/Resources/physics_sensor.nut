emo.Runtime.import("physics.nut");

local stage = emo.Stage();
local event = emo.Event();
local physics = emo.Physics();

local gravity = emo.Vec2(0, 10);
local world   = emo.physics.World(gravity, true);

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

    // uncomment below to uses dynamic scaled image like iOS way(i.e. ball@2x.png).
    // sprite  = emo.Sprite(getScaledImage("ball.png"));
    sprite  = emo.Sprite("ball.png");

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad"); 

        // Below statements is an example of multiple screen density support.
        // (i.e. Retina vs non-Retina, cellular phone vs tablet device).
        if (stage.getWindowWidth() > 320) {
            // if the screen has large display, scale contents twice
            // that makes the stage size by half.
            // This examples shows how to display similar-scale images
            // on Retina and non-Retina display.
            stage.setContentScale(stage.getWindowWidth() / 320.0);
        }
        
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
        
        // move sprite to the center of the screen
        local x = (stage.getWindowWidth()  - sprite.getWidth())  / 2;
        local y = (stage.getWindowHeight() - sprite.getHeight()) / 2;
        
        sprite.move(x, y);

        local bodyDef = emo.physics.BodyDef();
        bodyDef.allowSleep = false;

        local fixture = emo.physics.FixtureDef();
        fixture.density  = 1.0;
        fixture.friction = 0.3;
        fixture.restitution = 0.3;
        
        physics.createDynamicCircleSprite(world, sprite, sprite.getWidth() * 0.5, fixture, bodyDef);
        
        // load sprite to the screen
        sprite.load();
        wall1.load();
        wall2.load();
        wall3.load();
        wall4.load();
        
        event.registerSensors(SENSOR_TYPE_ACCELEROMETER);
        
        // Set OnDrawCallback interval (millisecond)
        event.enableOnDrawCallback(1000.0 / FPS);
    }

    /*
     * Called when the app has gained focus
     */
    function onGainedFocus() {
        print("onGainedFocus");
        event.enableSensor(SENSOR_TYPE_ACCELEROMETER, 100);
    }

    /*
     * Called when the app has lost focus
     */
    function onLostFocus() {
        print("onLostFocus"); 
        event.disableSensor(SENSOR_TYPE_ACCELEROMETER);
    }

    /*
     * Called when the class ends
     */
    function onDispose() {
        print("onDispose");
        
        // remove sprite from the screen
        sprite.remove();
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
     * sensor event
     */
    function onSensorEvent(sevent) {
        if (sevent.getType() == SENSOR_TYPE_ACCELEROMETER) {
            gravity.x = sevent.getAccelerationX() * 10;
            gravity.y = -(sevent.getAccelerationY() * 10);
            world.setGravity(gravity);
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
