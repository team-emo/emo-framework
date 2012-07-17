/*
 * Physics example by Box2D with one dynamic circle body and static bodies.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("emo-framework/physics.nut");

local stage   = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 10), true);

local fps = 60.0;

class Main {
    ground  = emo.Rectangle();
    blockL  = emo.Rectangle();
    blockR  = emo.Rectangle();
    sprite  = emo.Sprite("ball.png");
    explosion = emo.SpriteSheet("explosion.png", 19, 19, 3, 2);
    
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

        if (stage.getWindowWidth() >= 640) {
            explosion.scale(2, 2);
        }
        
        explosion.hide();
        explosion.load();
        explosion.setZ(2)
    
        ground.setZ(0);
        blockL.setZ(0);
        blockR.setZ(0);
        sprite.setZ(1);
        
        ground.load();
        sprite.load();
        blockL.load();
        blockR.load();
    
        // set OnDrawCallback interval (millisecond)
        emo.Event.enableOnDrawCallback(1000.0 / fps);
        
        // enable the physics worlds contact listener
        world.enableContactListener();
        
        // in this example we use only add state so disable other status.
        // this will suppress unnecessary event calls.
        world.enableContactState(PHYSICS_STATE_NULL,    false);
        world.enableContactState(PHYSICS_STATE_PERSIST, false);
        world.enableContactState(PHYSICS_STATE_REMOVE,  false);
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
        
        if (explosion.isAnimationFinished()) {
            explosion.hide();
        }
    }
    
    /*
     * Called when the class ends
     */
    function onDispose() {
        ground.remove();
        sprite.remove();
    }

    /*
     * Called when the physics objects are collided.
     */
    function onContact(state, fixtureA, fixtureB, 
            position, normal, normalImpulse, tangentImpulse) {
            
        // state is contact point state aka b2PointState.
        // see http://programanddesign.com/box2d/b2Collision_8h.html for details
        if (state == PHYSICS_STATE_ADD) {
            // get the event coordinate
            local x = position.x * world.getScale();
            local y = position.y * world.getScale();
            
            // draw the explosion
            explosion.hide();
            explosion.moveCenter(x, y);
            explosion.animate(0, 10, 66, 0);
            explosion.show();
            
            print(format("%4.2f, x=%4.2f, y=%4.2f", normalImpulse, x, y));
        }
    }
}
function emo::onLoad() {
    stage.load(Main());
}
