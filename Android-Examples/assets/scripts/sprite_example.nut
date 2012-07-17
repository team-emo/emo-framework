local stage = emo.Stage();

/*
 * This example shows single sprite that rotates and scales on touch-down event
 */
class Main {

    block = emo.Sprite("block.png");
    // On iOS, emo.Sprite supports PVR compression with no-mipmap texture:
    // block = emo.Sprite("potblocks.pvr");

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

        // load sprite to the screen
        block.load();
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
        block.remove();
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            block.scale(block.getScaleX()+0.5, block.getScaleY()+0.5);
            block.rotate(block.getAngle() + 20);
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
