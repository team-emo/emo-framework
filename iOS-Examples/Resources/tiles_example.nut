local stage = emo.Stage();

/*
 * This example shows one block sprite sheet that changes frame index on touch-down event
 */
class Main {

    // defines a sprite sheet that consists of 32x32 block with 2 pixel border.
    block = emo.SpriteSheet("blocks.png", 32, 32, 2);

    // current frame index
    currentFrame = 0;

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
	 * Called when this class will be disposed
	 */
    function onDispose() {
        print("onDispose");
        
        // remove sprite from the screen
        block.remove();
    }

	/*
	 * touch event
	 */
	function onMotionEvent(mevent) {
		// change frame index on touch down event
		if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
			currentFrame++;
			if (currentFrame >= block.getFrameCount()) {
				currentFrame = 0;
			}
			block.setFrame(currentFrame);
		}
	}
}

function emo::onLoad() {
    stage.load(Main());
}
