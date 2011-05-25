local stage = emo.Stage();
local event = emo.Event();

/*
 * This example shows loading sprites with modifier.
 */
class Main {

    circle = emo.Sprite("loading.png");
    text = emo.TextSprite("font_16x16.png", 16, 16, 
		" !\"c*%#'{}@+,=./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSQTVWXYZ");
    
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
		
		text.setText("  LOADING..");
		
		// move sprite to the center of the screen
		local x = (stage.getWindowWidth()  - circle.getWidth())  / 2;
		local y = (stage.getWindowHeight() - circle.getHeight()) / 2;
		
		circle.move(x, y);
		text.move((stage.getWindowWidth() - text.getWidth()) / 2,
					y + circle.getHeight() + text.getHeight());
		
		// set z-order (move text to front of the circle)
		circle.setZ(0);
		text.setZ(1);

		// load sprite to the screen
		text.load();
        circle.load();
        
        // rotate the block from 0 to 360 degree in 1 seconds using Linear equation with infinite loop.
        circle.addModifier(emo.RotateModifier(0, 360, 1000, emo.easing.Linear, -1));
		
		// change alpha color of the text in 2 seconds using Linear equation with infinite loop.
		text.addModifier(emo.AlphaModifier(1, 0, 2000, emo.easing.Linear, -1));
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
        circle.remove();
    }
}

function emo::onLoad() {
    stage.load(Main());
}
