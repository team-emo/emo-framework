/*
 * Physics example by Box2D with map scrolling.
 * This example uses OnDrawCallback to step the physics world.
 */
emo.Runtime.import("physics.nut");

local stage = emo.Stage();
local physics = emo.Physics();
local world   = emo.physics.World(emo.Vec2(0, 0), true);
local fps = 60.0;

const MAP_BLOCK_SIZE   = 32;

/*
 * This example shows tiled map sprite.
 */
class Main {
    controller = emo.AnalogOnScreenController("control_base.png", "control_knob.png");
    controlX = 0;
    controlY = 0;
    lastDirection = CONTROL_CENTER;
    
    // get center coordinate of screen
    centerX = stage.getWindowWidth()   / 2;
    centerY = stage.getWindowHeight()  / 2;
    
    // create map sprite with 32x32, 2 pixel border and 2 pixel margin
    map = emo.MapSprite("desert_chips.png", MAP_BLOCK_SIZE, MAP_BLOCK_SIZE, 1, 1);
    dog = emo.SpriteSheet("dog.png", 34, 42, 1, 1);
    ball  = emo.Sprite("ball.png");
    
    wallN  = emo.Rectangle();
    wallS  = emo.Rectangle();
    wallE  = emo.Rectangle();
    wallW  = emo.Rectangle();
    
    /*
     * Called when this class is loaded
     */
    function onLoad() {
        // load tmx file from graphics folder
        local rawname = "desert.tmx";
        local mapdata = stage.getMapData(rawname);
        map.setMap(mapdata.layer[0].data);
        
        // onControlEvent is fired every 16 milliseconds at most
        // the default updateInterval equals 100 milliseconds
        controller.updateInterval = 16;
        
        // set size of objects which don't have sprites
        wallN.setSize(map.getWidth(), 20)
        wallS.setSize(map.getWidth(), 20);
        wallE.setSize(20, map.getHeight());
        wallW.setSize(20, map.getHeight());
        
        // move sprite to the center of the screen
    	controller.move((stage.getWindowWidth() - controller.getWidth()) / 2,
                         stage.getWindowHeight() - controller.getHeight());
        map.move(0, 0);
        dog.moveCenter(centerX + 15, centerY);
        ball.moveCenter(centerX - 15, centerY);
        
        wallN.move(0, 0);
        wallS.move(0, map.getHeight() - wallS.getHeight());
        wallE.move(0, 0);
        wallW.move(map.getWidth()  - wallW.getWidth(), 0);
        
        // set Z-Order
        controller.setZ(80);
        ball.setZ(70);
        dog.setZ(60);
        wallN.setZ(20);
        wallS.setZ(20);
        wallE.setZ(20);
        wallW.setZ(20);
        map.setZ(10)
        
        // set up physics
        physics.createStaticSprite(world, wallN);
        physics.createStaticSprite(world, wallS);
        physics.createStaticSprite(world, wallE);
        physics.createStaticSprite(world, wallW);
        
        local fixtureDef = emo.physics.FixtureDef();
        fixtureDef.density  = 0.1;
        fixtureDef.friction = 0.3;
        fixtureDef.restitution = 0.5;
        physics.createDynamicCircleSprite(world, ball, ball.getWidth() * 0.5, fixtureDef);
        
        local bodyDef = emo.physics.BodyDef();
        
        bodyDef.fixedRotation = true;
        bodyDef.linearDamping = 1.0;
        
        fixtureDef.density  = 2.0;
        fixtureDef.friction = 3.0;
        fixtureDef.restitution = 0.1;
        physics.createDynamicSprite(world, dog, fixtureDef, bodyDef);
        
        // load sprites to the screen
        controller.load();
        map.load();
        dog.load();
        ball.load();
        wallN.load();
        wallS.load();
        wallE.load();
        wallW.load();
        
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
        
        local dogBody = dog.getPhysicsInfo().getBody();
        
        local point = emo.Vec2(dogBody.getPosition().x, dogBody.getPosition().y);       
        local force = emo.Vec2( controlX, controlY);
        dogBody.applyForce(force, point);
        
        stage.moveCamera(dog.getX() - centerX, dog.getY() - centerY);
    }

    /*
     * Called when the app has gained focus
     */
    function onGainedFocus() {
        print("onGainedFocus");
    }

    /*
     * Called when the app has lost focus
     */
    function onLostFocus() {
        print("onLostFocus"); 
    }

    /*
     * Called when the class ends
     */
    function onDispose() {
        print("onDispose");
        
        // remove sprite from the screen
        controller.remove();
        map.remove();
        dog.remove();
        ball.remove();
        wallN.remove();
        wallS.remove();
        wallE.remove();
        wallW.remove();
    }
    
    /*
     * on-screen controller event
     * controlX and controlY takes value from -100 to 100.
     */
    function onControlEvent(controller, _controlX, _controlY, hasChanged) {
        if (hasChanged) {
            controlX = _controlX;
            controlY = _controlY;
            if(controlX != 0){
                controlX /= 5.0;
            }
            if(controlY != 0){
                controlY /= 5.0;
            }
            
            if(controlX == 0 && controlY == 0 && dog.isAnimationFinished() == false){
                dog.pause();
                lastDirection = CONTROL_CENTER;
                return;
            }
            
            if (controlX < 0 && lastDirection != CONTROL_LEFT) {
	                dog.animate(5, 5, 120, -1);
	                lastDirection = CONTROL_LEFT;
            } else if (controlX > 0 && lastDirection != CONTROL_RIGHT ) {
                    dog.animate(0, 5, 120, -1);
                    lastDirection = CONTROL_RIGHT;
            }
        }
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
            //print(format("%4.2f, x=%4.2f, y=%4.2f", normalImpulse, x, y));
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}

/*
 * function for testing tmx file contents.
 */
function printData(_mapdata){
    print("mapdata : " + _mapdata);
    print("tileset : ");
    for(local i = 0; i < _mapdata.tileset.len() ; i++){
        print("firstgid : " + _mapdata.tileset[i].firstgid);
        print("name : " + _mapdata.tileset[i].name);
        print("tilewidth : " + _mapdata.tileset[i].tilewidth);
        print("tileheight : " + _mapdata.tileset[i].tileheight);
        print("source : " + _mapdata.tileset[i].source);
        print("trans : " + _mapdata.tileset[i].trans);
        print("width : " + _mapdata.tileset[i].width);
        print("height : " + _mapdata.tileset[i].height);
    }
    
    print("layer : ");
    for(local i = 0; i < _mapdata.layer.len() ; i++){
        print("name : " + _mapdata.layer[i].name);
        print("width : " + _mapdata.layer[i].width);
        print("height : " + _mapdata.layer[i].height);
        print("data : ");
        
        for(local oned = 0; _mapdata.layer[i].data.len() > oned ; oned++){
            local line = "";
            for(local twod = 0; _mapdata.layer[i].data[oned].len() > twod ; twod++){
                line += _mapdata.layer[i].data[oned][twod];
                if(twod != _mapdata.layer[i].data[oned].len()){
                    line += " ";
                }
            }
            print(oned + " : " + line);
        }
    }
}