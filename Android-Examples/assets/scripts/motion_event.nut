local stage = emo.Stage();

/*
 * This example shows single block that handles drag-and-drop.
 */
class Main {

    block = emo.Rectangle();
    dragStart = false;

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
        
        block.setSize(stage.getWindowWidth() * 0.2, stage.getWindowWidth() * 0.2);
        block.color(1, 0, 0);
        
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
            if (block.contains(mevent.getX(), mevent.getY())) {
                // change block color to blue
                block.color(0, 0, 1, 1);
                // move the block to center of the event's coordinate
                block.moveCenter(mevent.getX(), mevent.getY());
                dragStart = true;
            }
        } else if (mevent.getAction() == MOTION_EVENT_ACTION_MOVE) {
            if (dragStart) {
                block.moveCenter(mevent.getX(), mevent.getY());
            }
        } else if (mevent.getAction() == MOTION_EVENT_ACTION_UP ||
                   mevent.getAction() == MOTION_EVENT_ACTION_CANCEL) {
            if (dragStart) {
                // change block color to red
                block.color(1, 0, 0, 1);
                dragStart = false;
            }
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
