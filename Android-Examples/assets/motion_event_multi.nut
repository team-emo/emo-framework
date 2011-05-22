local stage = emo.Stage();

/*
 * This example shows multiple sprites that handles multi-touch
 */
class Main {

	rectangles = {};

	/*
	 * Called when this class is loaded
	 */
    function onLoad() {
        print("onLoad"); 
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
    }

	/*
	 * touch event
	 */
	function onMotionEvent(mevent) {
		local id = mevent.getPointerId();
		if (!rectangles.rawin(id)) {
			// create new rectangle
			local rectangle = emo.Rectangle();
			rectangle.setSize(70, 70);
			rectangle.color(1, 0, 0);
			rectangle.move(mevent.getX(), mevent.getY());
			rectangle.load();
			
			rectangles[id] <- rectangle;
		}
		if (mevent.getAction() == MOTION_EVENT_ACTION_UP) {
			print("UP: " + id);
		} else if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) { 
			print("DOWN: " + id);
		}
		handleTouch(rectangles[id], mevent);
	}
	
	function handleTouch(rectangle, mevent) {
		if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
			rectangle.move(mevent.getX(), mevent.getY());
		} else if (mevent.getAction() == MOTION_EVENT_ACTION_MOVE) {
			rectangle.move(mevent.getX(), mevent.getY());
		} else if (mevent.getAction() == MOTION_EVENT_ACTION_UP ||
				   mevent.getAction() == MOTION_EVENT_ACTION_CANCEL) {
			delete rectangles[mevent.getPointerId()];
			rectangle.remove();
		}
	}
}

function emo::onLoad() {
    stage.load(Main());
}
