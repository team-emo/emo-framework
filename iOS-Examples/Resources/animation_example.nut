local stage = emo.Stage();

/*
 * This example shows single sprite with animations.
 */
class Main {

    sprite = emo.SpriteSheet("dog.png", 34, 42, 3, 2);

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

		// load sprite to the screen
        sprite.load();
    }

	/*
	 * Called when the app has gained focus
	 */
    function onGainedFocus() {
        print("onGainedFocus");
		
		// loop count = -1 means inifinite loop
		sprite.animate(0, 5, 500, -1);
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
    }

	/*
	 * touch event
	 */
	function onMotionEvent(mevent) {
		if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
			sprite.pauseAt(0);
		}
	}
}

function emo::onLoad() {
    stage.load(Main());
}
