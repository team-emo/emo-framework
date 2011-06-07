local stage = emo.Stage();

/*
 * This example shows single sprite with animations.
 */
class Main {

    sprite = emo.SpriteSheet("dog.png", 34, 42, 3, 2);
	explosion = emo.SpriteSheet("explosion.png", 19, 19, 3, 2);

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
		
		// move sprite to the center of the screen
		local x = (stage.getWindowWidth()  - sprite.getWidth())  / 2;
		local y = (stage.getWindowHeight() - sprite.getHeight()) / 2;
		
		sprite.move(x, y);

		// load sprite to the screen
        sprite.load();
		
		local eScale = (stage.getWindowWidth() / 320.0) * 2;
		explosion.scale(eScale, eScale);
		explosion.hide();
		explosion.load();
		
		sprite.setZ(0);
		explosion.setZ(1);
    }

	/*
	 * Called when the app has gained focus
	 */
    function onGainedFocus() {
        print("onGainedFocus");
		
		// start with first frame, number of frames equals 5 and
		// interval equals 500msec.  loop count = -1 means inifinite loop
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
		explosion.remove();
    }

	/*
	 * touch event
	 */
	function onMotionEvent(mevent) {
		if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
			// draw the explosion
			explosion.hide();
			explosion.moveCenter(mevent.getX(), mevent.getY());
			explosion.animate(0, 10, 66, 0);
			explosion.show();
		}
	}
}

function emo::onLoad() {
    stage.load(Main());
}
