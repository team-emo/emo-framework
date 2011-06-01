local stage = emo.Stage();

const BLOCK_SIZE   = 32;

/*
 * This example shows tiled map sprite.
 */
class Main {

	// create map sprite with 32x32, 4 pixel border and 3 pixel margin
    sprite = emo.MapSprite("blocks.png", BLOCK_SIZE, BLOCK_SIZE, 4, 3);
	
	// 16x16 text sprite with 2 pixel border and 1 pixel margin
    text = emo.TextSprite("font_16x16.png",
		" !\"c*%#'{}@+,=./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
		16, 16, 2, 1);
		
	lastMoveX  = 0;
	lastMoveY  = 0;

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
		
		local tiles = [
			[-1,  8,  9, 10, -1, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1,  8,  9, 10, -1, -1, -1],
			[-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
			[-1, -1, -1, -1, -1,  8,  9, 10, -1, -1, -1,  8,  9, 10, -1, 11, 12, 13, 14, 15, -1],
			[-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
			[-1, 13, 14, 15, -1, -1, -1,  8,  9, -1, -1, 13, 14, 15, -1, -1, -1,  8,  9, -1, -1],
			[-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
			[-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1],
			[-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 18, 19, -1, -1, -1, -1, -1, -1, -1, -1],
			[-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 16, 17, -1, -1, -1, -1, -1, -1, -1, -1],
			[-1   0,  1,  2,  3,  4,  5, -1,  6,  7, -1   7,  6,  7,  6,  7,  6,  7,  6,  7, -1],
		];
		sprite.setMap(tiles);
		sprite.move(0, stage.getWindowHeight() - (BLOCK_SIZE * tiles.len()));

		// load sprite to the screen
        sprite.load();
		
		// change the text
		text.setText("DRAG TO MOVE THE TILE");
		text.scale(0.7, 0.7);
		
		local tX = (stage.getWindowWidth()  - text.getScaledWidth())  / 2;
		text.move(tX, text.getScaledHeight());

		text.load();
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
        sprite.remove();
    }

	/*
	 * this tiled map can be dragged along x-axis
	 */
	function onMotionEvent(mevent) {
		if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
			lastMoveX = mevent.getX();
			lastMoveY = mevent.getY();
		} else if (mevent.getAction() == MOTION_EVENT_ACTION_MOVE) {
			local x = sprite.getX() - (lastMoveX - mevent.getX());
			local y = sprite.getY() - (lastMoveY - mevent.getY());
			if (x <= 0 && x >= stage.getWindowWidth() - sprite.getWidth()) { 
				sprite.move(x, y);
			}
			lastMoveX = mevent.getX();
			lastMoveY = mevent.getY();
		}
	}
}

function emo::onLoad() {
    stage.load(Main());
}
