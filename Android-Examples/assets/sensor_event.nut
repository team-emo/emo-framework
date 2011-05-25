local stage = emo.Stage();
local event = emo.Event();

class Main {

    block = emo.Sprite("tv.png");

	/*
	 * Called when this class is loaded
	 */
    function onLoad() {
        print("onLoad"); 
		
		// move sprite to the center of the screen
		local x = (stage.getWindowWidth()  - block.getWidth())  / 2;
		local y = (stage.getWindowHeight() - block.getHeight()) / 2;
		
		block.move(x, y);

		// load sprite to the screen
        block.load();
		
		if (emo.Runtime.isSimulator()) {
			error("This program cannot run on the simulator.");
		} else {
			event.registerSensors(SENSOR_TYPE_ACCELEROMETER);
		}
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
        block.remove();
    }

	/*
	 * touch event
	 */
	function onSensorEvent(sevent) {
		if (sevent.getType() == SENSOR_TYPE_ACCELEROMETER) {
			block.rotate(emo.toDegree(sevent.getAccelerationX()));
		}
	}
}

function emo::onLoad() {
    stage.load(Main());
}
