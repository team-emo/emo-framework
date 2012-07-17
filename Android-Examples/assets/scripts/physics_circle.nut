/*
 * Physics example by Box2D with one dynamic circle body and static bodies.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("emo-framework/physics.nut");

local stage   = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 10), true);

local fps = 60.0;

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
    ground  = emo.Rectangle();
    blockL  = emo.Rectangle();
    blockR  = emo.Rectangle();
    
    // uncomment below to uses dynamic scaled image like iOS way(i.e. ball@2x.png).
    // sprite  = emo.Sprite(getScaledImage("ball.png"));
    sprite  = emo.Sprite("ball.png");
    // On iOS, emo.Sprite supports PVR compression with no-mipmap texture:
    // sprite  = emo.Sprite("A.pvr");
    
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
        
        ground.setSize(stage.getWindowWidth(), 20);
        ground.move(0, stage.getWindowHeight() - ground.getHeight());
        
        blockL.setSize(10, 100);
        blockR.setSize(10, 100);
        
        blockL.move(0, stage.getWindowHeight() - ground.getHeight() - blockL.getHeight());
        blockR.move(stage.getWindowWidth()  - blockR.getWidth(), 
                    stage.getWindowHeight() - ground.getHeight() - blockR.getHeight());
    
        sprite.move(0, 0);

        physics.createStaticSprite(world, ground);
        physics.createStaticSprite(world, blockL);
        physics.createStaticSprite(world, blockR);
        
        local fixture = emo.physics.FixtureDef();
        fixture.density  = 1.0;
        fixture.friction = 0.3;
        fixture.restitution = 0.2;
        physics.createDynamicCircleSprite(world, sprite, sprite.getWidth() * 0.5, fixture);
    
        ground.load();
        sprite.load();
        blockL.load();
        blockR.load();
    
        // set OnDrawCallback interval (millisecond)
        emo.Event.enableOnDrawCallback(1000.0 / fps);
    }
    
    /*
     * Called when the onDraw callback is enabled by enableOnDrawCallback.
     * This callback is called before drawing the screen at interval of milliseconds set by enableOnDrawCallback.
     * dt is the actual delta time elapsed (milliseconds).
     */
    function onDrawFrame(dt) {
        // world step interval(second)
        world.step(dt / 1000.0, 6, 2);
        world.clearForces();
    }
    
    /*
     * Called when the class ends
     */
    function onDispose() {
        ground.remove();
        sprite.remove();
    }
}
function emo::onLoad() {
    stage.load(Main());
}
