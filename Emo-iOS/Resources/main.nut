local stage = emo.Stage();

/*
 * This example shows single sprite that rotates and scales on touch-down event
 */
class Main {

    controller = emo.AnalogOnScreenController(
		"onscreen_control_base.png", "onscreen_control_knob.png");

	/*
	 * Called when this class is loaded
	 */
    function onLoad() {
        print("onLoad"); 
		
		stage.setContentScale(2);
		
		// move sprite to the center of the screen
		local x = (stage.getWindowWidth()  - controller.getWidth())  / 2;
		local y = (stage.getWindowHeight() - controller.getHeight()) / 2;
		
		controller.move(x, y);

		// load sprite to the screen
        controller.load();
    }
	
	function onControlEvent(controller, valueX, valueY, hasChanged) {
		print(format("%dx%d %s", valueX, valueY, hasChanged ? "changed" : "not changed"));
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
        controller.remove();
    }

}

function emo::onLoad() {
    stage.load(Main());
}
