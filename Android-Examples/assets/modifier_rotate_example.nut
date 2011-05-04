local stage = emo.Stage();
local event = emo.Event();

/*
 * This example shows one block sprite that moves around the screen.
 */
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
        
        // add modifier
        block.addModifier(emo.RotateModifier(0, 360, 5000, emo.easing.Linear));
        
		// onDrawFrame(dt) will be called on every 33 milliseconds
		// onDrawCallback must be enabled if sprite uses modifiers.
		event.enableOnDrawCallback(33);
    }

	/*
	 * Called when the app has gained focus
	 */
    function onGainedFocus() {
        print("onGainedFocus");
    }
    
    /*
     * Called when onDrawCalleback event is enabled by enableOnDrawCallback
     * dt parameter is delta time (millisecond)
     */
    function onDrawFrame(dt) {

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
