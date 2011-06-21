local stage = emo.Stage();

const NUMBER_OF_SPARKS = 12;
local touchCount = 0;

/*
 * A modifier class that emulates parabolic movement.
 */
class ParabolicMoveModifier extends emo.MultiModifier {
    g        = 0.0005;
    velocity = 0.2;
    angle    = null;
    toY      = null;

    // this constructor calculates the final destination (x, y) and pass them to the parent.
    function constructor(_from, _angle, _duration, _startTime = null) {
        angle = emo.toRadian(_angle);
        
        local x = velocity * cos(angle) * _duration;
        local y = getY(velocity, angle, _duration, g);

        toY = y;
        
        base.constructor(_from, [_from[0] + x, _from[1] + y], _duration, [emo.easing.Linear, parabolic], 0, _startTime);
    }
    
    // move the target object when modify event is fired.
    function onModify(currentValue) {
        targetObj.moveCenter(currentValue[0], currentValue[1]);
    }
    
    // calculate the current position
    function getY(_velocity, _angle, _elapsed, _g) {
        return -(_velocity * sin(_angle) * _elapsed - (1 * 0.5 * _g * pow(_elapsed, 2)));
    }
    
    // ParabolicMoveModifier implements equation function by itself.
    function parabolic(elapsed, duration, modifier) {
        local y = modifier.getY(modifier.velocity, modifier.angle, elapsed, modifier.g);
        return y / modifier.toY.tofloat();
    }
}

/*
 * This example shows particle-like sparks using multiple modifiers
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
        text.setText("TAP TO ADD MODIFIERS");
        
        local tX = (stage.getWindowWidth()  - text.getScaledWidth())  / 2;
        text.move(tX, text.getScaledHeight());

        text.load();
        
        addSpark(stage.getCenterX(), stage.getCenterY());
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
    
    function addSpark(startX, startY) {
        // save the start time of the modifiers because
        // there's a 'time lag' between the first modifier and the last modifier.
        local startTime = emo.Runtime().uptime();
        local doRandom = touchCount++ % 2 == 0;
        
        for (local i = 0; i < sparks.len(); i++) {
            
            sparks[i].moveCenter(startX, startY);
            
            // clear previous modifiers if exists.
            sparks[i].clearModifier();
            
            // change the angle to throw the sparks
            local angle = i * (360.0 / sparks.len());
            
            // modifier for parabolic movement
            // defined by custom modifier class: ParabolicMoveModifier
            local pModifier = ParabolicMoveModifier(
                [startX, startY],           // from [x, y]
                angle,                      // angle
                2000,                       // duration
                startTime                   // modifier start time
            );
            if (doRandom) {
                pModifier.g = 0;
                pModifier.velocity = 0.02 + (0.01 * (rand() % 4));
            }
            sparks[i].addModifier(pModifier);
            
            // modifier for changing colors
            sparks[i].addModifier(emo.ColorModifier(
                [1, 1, 0],                  // from [rgb]
                [0.7, 0.13, 0.13],          // to   [rgb]
                2000,                       // duration
                emo.easing.CubicIn,         // equation
                0,                          // repeat count
                startTime                   // modifier start time
                
            ));
            
            // modifier for changing alpha colors
            sparks[i].addModifier(emo.AlphaModifier(
                1,                          // from
                0,                          // to
                2000,                       // duration
                emo.easing.CubicIn,         // equation
                0,                          // repeat count
                startTime                   // modifier start time
            ));
            
            // modifier for rotation
            sparks[i].addModifier(emo.RotateModifier(
                angle,
                angle + 180,
                2000,                       // duration
                emo.easing.CubicOut,        // equation
                0,                          // repeat count
                startTime                   // modifier start time
            ));
            sparks[i].show();
        }
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            addSpark(mevent.getX(), mevent.getY());
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
