emo.Runtime.import("emo-framework/physics.nut");

local stage = emo.Stage();
local event = emo.Event();
local physics = emo.Physics();

local gravity = emo.Vec2(0, 10);
local world   = emo.physics.World(gravity, true);

const FPS = 60.0;
const WALL_WIDTH = 10;
const NUMBER_OF_BALLS = 30;

local nextBallIndex = 0;
local ballFixture   = null;
local ballDef       = null;

class Main {

    sprites  = [];

    /*
     * pre-load all sprites because loading sprites is heavy operation.
     * it is not recommended to load sprites dynamically at runtime (i.e. at onMotionEvent)
     */
    function preloadBalls() {
    
        ballFixture = emo.physics.FixtureDef();
        ballFixture.density  = 1.0;
        ballFixture.friction = 0.3;
        ballFixture.restitution = 0.2;
        
        // the balls won't sleep
        ballDef = emo.physics.BodyDef();
        ballDef.allowSleep = false;
        
        for (local i = 0; i < NUMBER_OF_BALLS; i++) {
            local sprite = emo.Sprite("ball.png");
            sprite.hide();
            sprite.load();
            sprites.append(sprite);
        }
    }

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad");
        
         event.enableOnFpsCallback(5000); 

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
        
        wall1.load();
        wall2.load();
        wall3.load();
        wall4.load();
        
        preloadBalls();
        
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
        
        addBall(stage.getCenterX(), stage.getCenterY());
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
        for (local i = 0; i < sprites.len(); i++) {
            sprites[i].remove();
        }
        
        sprites.clear();
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
    
    /*
     * drop boxes at touch event.
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            addBall(mevent.getX(), mevent.getY());
        }
    }
    
    function addBall(x, y) {
    
        local i = nextBallIndex;
    
        if (i >= sprites.len()) {
            return;
        }
            
        sprites[i].moveCenter(x, y);
        physics.createDynamicCircleSprite(world, sprites[i], sprites[i].getWidth() * 0.5, ballFixture, ballDef);
        sprites[i].show();
            
        nextBallIndex++;
    }
    
    function onFps(fps) {
        print(format("FPS: %4.2f", fps));
    }
    
    function onLowMemory() {
        print("onLowMemory: too many sprites");
        
        // clear all objects except only one sprite.
        for (local i = 1; i < sprites.len(); i++) {
            sprites[i].remove();
        }
        for (local i = 1; i < sprites.len(); i++) {
            sprites.pop();
        }
        
        nextBallIndex = 1;
    }
}

function emo::onLoad() {
    stage.load(Main());
}
