local stage = emo.Stage();
local event = emo.Event();

const NUMBER_OF_SPARKS = 100;

/*
 * This example shows single point sprite
 */
class Main {

    spark = null;
    points = [];

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad"); 
        
        // initialize points
        for (local i = 0; i < NUMBER_OF_SPARKS; i++) {
            points.append(emo.Vec2(
                rand() % stage.getWindowWidth(),
                rand() % stage.getWindowHeight()));
        }

        // create point sprite with texture
        spark = emo.PointSprite("flare.png", points)
        spark.color(1, 1, 0);
        spark.load();
        
        // print frame per second on every 5 seconds
        event.enableOnFpsCallback(5000);
        
        // onDrawFrame(dt) will be called on every 33 milliseconds
        event.enableOnDrawCallback(33);
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
        spark.remove();
    }

    /*
     * Enabled after onDrawCalleback event is enabled by enableOnDrawCallback
     * dt parameter is a delta time (millisecond)
     */
    function onDrawFrame(dt) {
        // update points to random position
        for (local i = 0; i < NUMBER_OF_SPARKS; i++) {
            points[i].x = rand() % stage.getWindowWidth();
            points[i].y = rand() % stage.getWindowHeight();
        }
        spark.setPoints(points);
    }
    
    function onFps(fps) {
        print(format("FPS: %4.2f", fps));
    }
}

function emo::onLoad() {
    stage.load(Main());
}
