local stage = emo.Stage();
local event = emo.Event();

/*
 * This example shows one block sprite that rotates using modifier.
 */
class Main {

    block = emo.Sprite("block.png");
    
	/*
	 * Called when this class is loaded
	 */
    function onLoad() {
        print("onLoad"); 
		
		// Below statements is an example of multiple screen density support.
		// (i.e. Retina vs non-Retina, cellular phone vs tablet device).
		if (stage.getWindowWidth() >= 640) {
			// if the screen has large display, scale contents twice
			// that makes the stage size by half.
			// This examples shows how to display similar-scale images
			// on Retina and non-Retina display.
			stage.setContentScale(2);
		}
		
		// move sprite to the center of the screen
		local x = (stage.getWindowWidth()  - block.getWidth())  / 2;
		local y = (stage.getWindowHeight() - block.getHeight()) / 2;
		
		block.move(x, y);

		// load sprite to the screen
        block.load();
        
        // rotate the block from 0 to 360 degree in 5 seconds using Linear equation
        block.addModifier(emo.RotateModifier(0, 360, 5000, emo.easing.Linear));
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
        block.remove();
    }
}

function emo::onLoad() {
    stage.load(Main());
}
