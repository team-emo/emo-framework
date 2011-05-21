local stage = emo.Stage();

/*
 * This example shows single sprite with animations.
 */
class Main {

    sprite = emo.SpriteSheet("dog.png", 34, 42, 1);

	/*
	 * Called when this class is loaded
	 */
    function onLoad() {
        print("onLoad"); 
		
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
		
		sprite.animate(0, 5, 500);
    }

	/*
	 * Called when the app has lost focus
	 */
    function onLostFocus() {
        print("onLostFocus"); 
    }

	/*
	 * Called when this class will be disposed
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
