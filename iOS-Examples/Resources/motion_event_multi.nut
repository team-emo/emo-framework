local stage = emo.Stage();

/*
 * This example shows draggable sprites that handles multi-touch
 */
class Main {

    // 16x16 text sprite with 2 pixel border and 1 pixel margin
    text = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
        
    rectangles = {};

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
        
        // change the text
        text.setText("DRAG TO TEST MULTITOUCH");
        text.scale(0.7, 0.7);
        
        local tX = (stage.getWindowWidth()  - text.getScaledWidth())  / 2;
        text.move(tX, text.getScaledHeight());

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
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        // pointer id is a unique id of the pointer.
        local id = mevent.getPointerId();
        local action = mevent.getAction();

        if (!rectangles.rawin(id)) {
            // if new pointer comes in, create new rectangle
            local rectangle = emo.Rectangle();
            rectangle.setSize(stage.getWindowWidth() * 0.2, stage.getWindowWidth() * 0.2);
            rectangle.color(1, 0, 0);
            rectangle.moveCenter(mevent.getX(), mevent.getY());
            rectangle.load();
            
            // add rectangle to the hash table.
            rectangles[id] <- rectangle;
        }
        if (action == MOTION_EVENT_ACTION_UP || action == MOTION_EVENT_ACTION_POINTER_UP) {
            print("UP: " + id);
        } else if (action == MOTION_EVENT_ACTION_DOWN || action == MOTION_EVENT_ACTION_POINTER_DOWN) { 
            print("DOWN: " + id);
        }
        handleTouch(rectangles[id], mevent);
    }
    
    /*
     * move and remove the rectangle
     */
    function handleTouch(rectangle, mevent) {
        local action = mevent.getAction();
        if (action == MOTION_EVENT_ACTION_DOWN || action == MOTION_EVENT_ACTION_POINTER_DOWN) {
            rectangle.moveCenter(mevent.getX(), mevent.getY());
        } else if (action == MOTION_EVENT_ACTION_MOVE) {
            rectangle.moveCenter(mevent.getX(), mevent.getY());
        } else if (action == MOTION_EVENT_ACTION_UP ||
                   action == MOTION_EVENT_ACTION_CANCEL ||
                   action == MOTION_EVENT_ACTION_OUTSIDE ||
                   action == MOTION_EVENT_ACTION_POINTER_UP) {
            delete rectangles[mevent.getPointerId()];
            rectangle.remove();
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
