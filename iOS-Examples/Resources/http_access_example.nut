local stage = emo.Stage();

const MY_REQUEST_NAME = "My HTTP Request";

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
        
        // change the text
        text.setText("TAP TO ACCESS");
        
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
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            // http request
            emo.Net.request(MY_REQUEST_NAME, "http://www.example.com/");
            
            // http request with METHOD and request timeout(msec)
            // emo.Net.request(MY_REQUEST_NAME, "http://www.example.com/", "GET", 1000);
            
            // http request with METHOD, timeout and parameters
            // emo.Net.request(MY_REQUEST_NAME, "http://www.example.com/",
            //        "POST", 1000, "key1", "value1", "key2", "value2");
        }
    }
    
    /*
     * Asynchronous http request callback
     */
    function onNetCallback(name, response, err) {
        if (name == MY_REQUEST_NAME) {
            print(format("============ RESPONSE FROM %s ============", name));
            print(response);
            text.setText("ACCESS: OK");
        } else if (name == HTTP_ERROR) {
            // if the request has failed, name equals HTTP_ERROR
            // and the response equals the request name ('My HTTP Request').
            print(format("============ ERROR RESPONSE FROM %s ============", response));
            
            // print the error description and error code.
            print(err.code);
            print(err.message);
            
            text.setText("ACCESS: FAILED");
        }
    }
}

function emo::onLoad() {
    emo.Stage().load(Main());
}
