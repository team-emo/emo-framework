local stage = emo.Stage();
local event = emo.Event();

/*
 * This example shows one block sprite that moves around the screen.
 */
class Main {

    block = emo.Sprite("flare.png");
    
    distantX = 2;
    distantY = 2;

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
        local x = (stage.getWindowWidth()  - block.getWidth())  / 2;
        local y = (stage.getWindowHeight() - block.getHeight()) / 2;
        
        block.move(x, y);
        
        block.color(1, 1, 0);

        // load sprite to the screen
        block.load();
        
        // onDrawFrame(dt) will be called on every 16 milliseconds
        event.enableOnDrawCallback(16);
    }

    /*
     * Called when the app has gained focus
     */
    function onGainedFocus() {
        print("onGainedFocus");
    }
    
    /*
     * Enabled after onDrawCalleback event is enabled by enableOnDrawCallback
     * dt parameter is a delta time (millisecond)
     */
    function onDrawFrame(dt) {
        local x = block.getX() + distantX;
        local y = block.getY() + distantY;
        
        if (x + block.getWidth()  >= stage.getWindowWidth()  || x <= 0) distantX = -distantX;
        if (y + block.getHeight() >= stage.getWindowHeight() || y <= 0) distantY = -distantY;
        
        block.rotate(block.getAngle() + 3);
        block.move(x, y);
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
        block.remove();
    }
}

function emo::onLoad() {
    stage.load(Main());
}
