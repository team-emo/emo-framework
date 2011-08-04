local stage = emo.Stage();

/*
 * This example shows the way to compile user string dynamically
 * 
 * Note: this example doesn't run on iOS if the sandbox is enabled.
 * Comment out EMO_WITH_SANDBOX in emo/Constants.h and re-compile to disable the sandbox.
 */
class Main {

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad");

        print(format("document directory: %s", emo.Runtime.getDocumentDir()));
        // user generated script can be loaded by using emo.Runtime.compile.
        // use TYPE_DOCUMENT when the script is in the user document directory.
        // user document directory can be retrieved via emo.Runtime.getDocumentDir()
        // emo.Runtime.compile("test.nut", TYPE_DOCUMENT);
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
          // script string can be loaded by using emo.Runtime.compilebuffer.
          // you have to be sure to filter out the malicious input.
          local script = format("print(%d+%d);", mevent.getX(), mevent.getY());
          print("compiling... " + script);
          emo.Runtime.compilebuffer(script);
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
