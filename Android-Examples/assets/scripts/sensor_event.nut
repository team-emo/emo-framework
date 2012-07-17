local stage = emo.Stage();
local event = emo.Event();

class Main {

    // 16x16 text sprite with 2 pixel border and 1 pixel margin
    text = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
        
    dog = emo.SpriteSheet("dog.png", 34, 42, 1, 1);

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad"); 
        
        // move sprite to the center of the screen
        local x = (stage.getWindowWidth()  - dog.getWidth())  / 2;
        local y = (stage.getWindowHeight() - dog.getHeight()) / 2;
        
        local scale = stage.getWindowWidth() / 320.0;
        
        dog.scale(scale, scale);
        
        dog.move(x, y);

        // load sprite to the screen
        dog.load();
        
        text.scale(scale, scale);
        changeText(0, 0);
        text.load();
        
        if (emo.Runtime.isSimulator()) {
            error("This program cannot run on the simulator.");
        } else {
            event.registerSensors(SENSOR_TYPE_ACCELEROMETER);
        }
    }
    
    function changeText(x, y) {
        text.setText(format("X:%4.2f, Y:%4.2f", x, y));
        
        local tX = (stage.getWindowWidth()  - text.getScaledWidth())  / 2;
        text.move(tX, text.getHeight());
    }

    /*
     * Called when the app has gained focus
     */
    function onGainedFocus() {
        print("onGainedFocus");
        if (!emo.Runtime.isSimulator()) {
            event.enableSensor(SENSOR_TYPE_ACCELEROMETER, 100);
        }
    }

    /*
     * Called when the app has lost focus
     */
    function onLostFocus() {
        print("onLostFocus"); 
        if (!emo.Runtime.isSimulator()) {
            event.disableSensor(SENSOR_TYPE_ACCELEROMETER);
        }
    }

    /*
     * Called when the class ends
     */
    function onDispose() {
        print("onDispose");
        
        // remove sprite from the screen
        dog.remove();
    }

    /*
     * touch event
     */
    function onSensorEvent(sevent) {
        if (sevent.getType() == SENSOR_TYPE_ACCELEROMETER) {
            dog.rotate(emo.toDegree(sevent.getAccelerationX()));
            changeText(sevent.getAccelerationX(),
                       sevent.getAccelerationY());
        
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
