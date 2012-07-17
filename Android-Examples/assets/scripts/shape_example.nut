local stage = emo.Stage();

/*
 * This example shows one rectangular shape that moves to the position on touch-down event.
 */
class Main {

    rectangle = emo.Rectangle();

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad"); 

        local width  = 60;
        local height = 60;
        
        // move rectangle to the center of the screen
        local x = (stage.getWindowWidth()  - width)  / 2;
        local y = (stage.getWindowHeight() - height) / 2;
        
        rectangle.move(x, y);
        
        // set rectangle color(RGBA)
        rectangle.color(1, 0, 0, 1);

        // initial rectangle size is 1x1 so update the size to 60x60
        rectangle.setSize(width, height);

        // load rectangle to the screen
        rectangle.load();
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
        
        // remove rectangle from the screen
        rectangle.remove();
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        // move rectangle to the position on the motion event 
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            local x = mevent.getX() - (rectangle.getWidth()  / 2);
            local y = mevent.getY() - (rectangle.getHeight() / 2);
            rectangle.move(x, y);
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
