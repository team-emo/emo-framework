local stage = emo.Stage();


const NUMBER_OF_SPARKS = 36;

class ParabolicMoveModifier extends emo.MultiModifier {
	g        = 0.0004;
	velocity = 0.2
	angle    = null;
	toY      = null;
	function constructor(_from, _angle, _duration) {
		angle = emo.toRadian(_angle);
	
		local x = velocity * cos(angle) * _duration;
		local y = getY(velocity, angle, _duration, g);

		toY = y;
		
		base.constructor(_from, [_from[0] + x, _from[1] + y], _duration, [emo.easing.Linear, parabolic], 0);
	}
	function onModify(currentValue) {
		targetObj.moveCenter(currentValue[0], currentValue[1]);
	}
	function getY(_velocity, _angle, _elapsed, _g) {
		return -(_velocity * sin(_angle) * _elapsed - (1 * 0.5 * _g * pow(_elapsed, 2)));
	}
	function parabolic(elapsed, duration, modifier) {
		local y = modifier.getY(modifier.velocity, modifier.angle, elapsed, modifier.g);
		return y / modifier.toY.tofloat();
	}
}

/*
 * This example shows single sprite that rotates and scales on touch-down event
 */
class Main {

	// 16x16 text sprite with 2 pixel border and 1 pixel margin
    text = emo.TextSprite("font_16x16.png",
		" !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
		16, 16, 2, 1);
		
    sparks = [];

	/*
	 * Called when this class is loaded
	 */
    function onLoad() {
        print("onLoad"); 
		for (local i = 0; i < NUMBER_OF_SPARKS; i++) {
			local spark = emo.Sprite("flare.png");
			spark.color(1, 1, 0);
			spark.hide();
			spark.load();
			sparks.append(spark);
		}
		
		// change the text
		text.setText("TAP TO TEST MODIFIERS");
		
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
    }

	/*
	 * touch event
	 */
	function onMotionEvent(mevent) {
		if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
			for (local i = 0; i < sparks.len(); i++) {
				local startX = mevent.getX();
				local startY = mevent.getY();
				
				sparks[i].moveCenter(startX, startY);
				
				sparks[i].addModifier(ParabolicMoveModifier(
					[startX, startY],         // from [x, y]
					i * (360.0 / sparks.len()), // angle
					2000                      // duration
				));
				
				sparks[i].addModifier(emo.ColorModifier(
					[1, 1, 0],
					[0.7, 0.13, 0.13],
					2000,
					emo.easing.Linear
				));
				sparks[i].addModifier(emo.AlphaModifier(
					1,
					0,
					2000,
					emo.easing.CubicIn
				));
				sparks[i].show();
			}
		}
	}
}

function emo::onLoad() {
    stage.load(Main());
}
