local stage = emo.Stage();

/*
 * This example shows single text sprite
 */
class Main {

    // 16x16 text sprite with 2 pixel border and 1 pixel margin
    text = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);

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
        
        text.setText("HELLO, WORLD!");
        
        // move sprite to the center of the screen
        local x = (stage.getWindowWidth()  - text.getWidth())  / 2;
        local y = (stage.getWindowHeight() - text.getHeight()) / 2;

        text.move(x, y);

        // load sprite to the screen
        text.load();
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
        text.remove();
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            // scale up the text
            text.scale(text.getScaleX() * 1.1, text.getScaleY() * 1.1);
            
            // Move sprite to the center of the screen
            // Use getScaledWidth, getScaledHeight to retrive scaled size.
            local x = (stage.getWindowWidth()  - text.getScaledWidth())  / 2;
            local y = (stage.getWindowHeight() - text.getScaledHeight()) / 2;
            
            text.move(x, y);
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
