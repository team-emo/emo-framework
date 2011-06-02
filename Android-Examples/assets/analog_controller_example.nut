local stage = emo.Stage();

/*
 * This example shows single sprite that can be moved by on-screen controller
 */
class Main {

    sprite = emo.SpriteSheet("dog.png", 34, 42, 3, 2);
	controller = emo.AnalogOnScreenController(
		"onscreen_control_base.png", "onscreen_control_knob.png");

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
			stage.setContentScale(stage.getWindowWidth() / 320);
		}
		
		// move sprite to the center of the screen
		local x = (stage.getWindowWidth()  - sprite.getWidth())  / 2;
		local y = (stage.getWindowHeight() - sprite.getHeight()) / 2;
		
		sprite.move(x, y);
		controller.move((stage.getWindowWidth() - controller.getWidth()) / 2,
						 stage.getWindowHeight() - controller.getHeight());

		// load sprite into the screen
        sprite.load();
		controller.load();
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
        sprite.remove();
		controller.remove();
    }

	/*
	 * on-screen controller event
	 */
	function onControlEvent(controller, controlX, controlY, hasChanged) {
		local nextX = sprite.getX() + (controlX / 5);
		local nextY = sprite.getY() + (controlY / 5);
		
		if (nextX < 0 || nextX > stage.getWindowWidth() - sprite.getWidth()) {
			nextX = sprite.getX();
		}
		if (nextY < 0 || nextY > stage.getWindowHeight() - sprite.getHeight()) {
			nextY = sprite.getY();
		}
		
		sprite.move(nextX, nextY);
		
		local direction = controller.getDirection();
		if (direction == CONTROL_LEFT) {
			sprite.setFrame(5);
		} else if (direction == CONTROL_RIGHT) {
			sprite.setFrame(0);
		} else if (direction == CONTROL_UP) {
			sprite.setFrame(1);
		} else if (direction == CONTROL_DOWN) {
			sprite.setFrame(2);
		} else {
			sprite.setFrame(0);
		}
	}
}

function emo::onLoad() {
    stage.load(Main());
}
