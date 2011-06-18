class Main {
    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad"); 
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
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            print(format("EVENT: %dx%d", mevent.getX(), mevent.getY()));
        }
    }
}

function emo::onLoad() {
    emo.Stage().load(Main());
}
