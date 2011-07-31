local stage = emo.Stage();

/*
 * This example shows the way to compile user string dynamically
 */
class Main {

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad");

        print(format("document directory: %s", emo.Runtime.getDocumentDir()));
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
        // compiles user script dynamically
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
          local script = format("print(%d+%d);", mevent.getX(), mevent.getY());
          print("compiling... " + script);
          emo.Runtime.compilebuffer(script);
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
