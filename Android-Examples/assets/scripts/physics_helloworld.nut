/*
 * Physics example by Box2D with one dynamic body and one static body.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("emo-framework/physics.nut");

local stage   = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 10), true);

const FPS = 60.0;

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
    ground = emo.Rectangle();
    sprite = emo.Sprite("tv.png");
    // uncomment below to uses dynamic scaled image like iOS way(i.e. tv@2x.png).
    // sprite  = emo.Sprite(getScaledImage("tv.png"));
    
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
    
        sprite.move((stage.getWindowWidth() - sprite.getWidth()) / 2, 0);
        sprite.rotate(30);

        physics.createStaticSprite(world, ground);
        
        local fixture = emo.physics.FixtureDef();
        fixture.density  = 1.0;
        fixture.friction = 0.2;
        fixture.restitution = 0.1;

        physics.createDynamicSprite(world, sprite, fixture);
    
        ground.load();
        sprite.load();
    
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
        ground.remove();
        sprite.remove();
    }
}
function emo::onLoad() {
    stage.load(Main());
}
