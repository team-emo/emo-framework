local stage = emo.Stage();

/*
 * This example shows single sprite with animations.
 */
class Main {

    sprite = emo.SpriteSheet("dog.png", 34, 42, 1, 1);
    explosion = emo.SpriteSheet("explosion.png", 19, 19, 3, 2);

    direction = "RIGHT";
    startX = null;
    endX   = null;

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
        
        // move sprite to the center of the screen
        local x = (stage.getWindowWidth()  - sprite.getWidth())  / 2;
        local y = (stage.getWindowHeight() - sprite.getHeight()) / 2;
        
        sprite.move(x, y);

        // load sprite to the screen
        sprite.load();
        
        local eScale = (stage.getWindowWidth() / 320.0) * 2;
        explosion.scale(eScale, eScale);
        explosion.hide();
        explosion.load();
        
        sprite.setZ(0);
        explosion.setZ(1);
        
        startX = stage.getWindowWidth() / 5.0;
        endX   = startX * 4;
        
        // onDrawFrame(dt) will be called on every 33 milliseconds
        emo.Event().enableOnDrawCallback(33);
    }

    /*
     * Enabled after onDrawCalleback event is enabled by enableOnDrawCallback
     * dt parameter is a delta time (millisecond)
     */
    function onDrawFrame(dt) {
        if (direction == "RIGHT") {
            // if the dog reaches the endX, change the animation
            if (sprite.getX() >= endX) {
                direction = "LEFT";
                
                // draw the explosion
                addExplosion(sprite.getX() + sprite.getWidth(), sprite.getY() + sprite.getHeight());

                // start with 5th frame, number of frames equals 2 (no.5 to no.6) and
                // interval equals 100msec.  loop count = -1 means inifinite loop
                sprite.animate(5, 2, 200, -1);
            } else {
                sprite.move(sprite.getX() + 2, sprite.getY());
            }
        } else {
            // if the dog reaches the startX, change the animation
            if (sprite.getX() <= startX) {
                direction = "RIGHT";
                
                // draw the explosion
                addExplosion(sprite.getX() + sprite.getWidth(), sprite.getY() + sprite.getHeight());
                
                // start with first frame, number of frames equals 2(no.0 to no.1) and
                // interval equals 100msec.  loop count = -1 means inifinite loop
                sprite.animate(0, 2, 200, -1);
            } else {
                sprite.move(sprite.getX() - 2, sprite.getY());
            }
        }
    }
    
    /*
     * Called when the app has gained focus
     */
    function onGainedFocus() {
        print("onGainedFocus");
        
        if (direction == "RIGHT") {
            // if current direction equals right,
            // start with first frame, number of frames equals 2(no.0 to no.1) and
            // interval equals 100msec.  loop count = -1 means inifinite loop
            sprite.animate(0, 2, 200, -1);
        } else {
            // if current direction equals left,
            // start the left animation
            sprite.animate(5, 2, 200, -1);
        }
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
        sprite.remove();
        explosion.remove();
    }
    
    // draw the explosion
    function addExplosion(x, y) {
        explosion.hide();
        explosion.moveCenter(x, y);
        explosion.animate(0, 10, 66, 0);
        explosion.show();
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            // draw the explosion
            addExplosion(mevent.getX(), mevent.getY());
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
