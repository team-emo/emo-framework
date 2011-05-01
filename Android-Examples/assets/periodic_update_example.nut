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
        
		// onDrawFrame(dt) will be called on every 33 milliseconds
		event.enableOnDrawCallback(33);
    }

	/*
	 * Called when the app has gained focus
	 */
    function onGainedFocus() {
        print("onGainedFocus");
    }
    
    /*
     * Called when onDrawCalleback event is enabled
     * dt parameter is delta time (millisecond)
     */
    function onDrawFrame(dt) {
    	block.rotate(block.getAngle() + 3);
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
        block.remove();
    }
}

function emo::onLoad() {
    stage.load(Main());
}
