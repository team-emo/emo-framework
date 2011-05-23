local stage = emo.Stage();
local event = emo.Event();

/*
 * This example shows one block sprite that moves around the screen.
 */
class Main {

    block = emo.Sprite("block.png");
    
    distantX = 2;
    distantY = 2;

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
     * Enabled after onDrawCalleback event is enabled by enableOnDrawCallback
     * dt parameter is a delta time (millisecond)
     */
    function onDrawFrame(dt) {
    	local x = block.getX() + distantX;
    	local y = block.getY() + distantY;
    	
    	if (x + block.getWidth()  >= stage.getWindowWidth()  || x <= 0) distantX = -distantX;
    	if (y + block.getHeight() >= stage.getWindowHeight() || y <= 0) distantY = -distantY;
    	
    	block.rotate(block.getAngle() + 3);
    	block.move(x, y);
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
