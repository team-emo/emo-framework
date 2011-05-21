local stage = emo.Stage();

const BLOCK_SIZE   = 32;
const BLOCK_BORDER = 2;

/*
 * This example shows tiled map sprite.
 */
class Main {

    sprite = emo.MapSprite("blocks.png", BLOCK_SIZE, BLOCK_SIZE, BLOCK_BORDER);

	/*
	 * Called when this class is loaded
	 */
    function onLoad() {
        print("onLoad"); 

		local tiles = [
			[-1,  8,  9, 10, -1, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1,  8,  9, 10, -1, -1],
			[-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
			[-1, -1, -1, -1, -1,  8,  9, 10, -1, -1, -1,  8,  9, 10, -1, 11, 12, 13, 14, 15],
			[-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
			[-1, 13, 14, 15, -1, -1, -1,  8,  9, -1, -1, 13, 14, 15, -1, -1, -1,  8,  9, -1],
			[-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
			[-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
			[-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 18, 19, -1, -1, -1, -1, -1, -1, -1],
			[-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 16, 17, -1, -1, -1, -1, -1, -1, -1],
			[-1   0,  1,  2,  3,  4,  5, -1,  6,  7, -1   7,  6,  7,  6,  7,  6,  7,  6,  7],
		];
		sprite.setMap(tiles);
		sprite.move(0, stage.getWindowHeight() - (BLOCK_SIZE * tiles.len()));

		// load sprite to the screen
        sprite.load();
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
        sprite.remove();
    }

	/*
	 * touch event
	 */
	function onMotionEvent(mevent) {
		// move one block right
		if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
			sprite.move(sprite.getX() - BLOCK_SIZE, sprite.getY());
		}
	}
}

function emo::onLoad() {
    stage.load(Main());
}
