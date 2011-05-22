local stage = emo.Stage();
local event = emo.Event();

class Main {

    block = emo.Sprite("block.png");

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
		
		event.registerSensors(SENSOR_TYPE_ACCELEROMETER);
    }

	/*
	 * Called when the app has gained focus
	 */
    function onGainedFocus() {
        print("onGainedFocus");
		event.enableSensor(SENSOR_TYPE_ACCELEROMETER, 100);
    }

	/*
	 * Called when the app has lost focus
	 */
    function onLostFocus() {
        print("onLostFocus"); 
		event.disableSensor(SENSOR_TYPE_ACCELEROMETER);
    }

	/*
	 * Called when this class will be disposed
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
			print(format("x=%f, y=%f, z=%f", 
				sevent.getAccelerationX(), 
				sevent.getAccelerationY(),
				sevent.getAccelerationZ()));
		}
	}
}

function emo::onLoad() {
    stage.load(Main());
}
