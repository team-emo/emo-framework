local stage = emo.Stage();
local event = emo.Event();

/*
 * This example shows block sprites using Modifiers.
 */
class Main {

    block1 = emo.Sprite("flare.png");
    block2 = emo.Sprite("flare.png");
    block3 = emo.Sprite("flare.png");
    block4 = emo.Sprite("flare.png");
    block5 = emo.Sprite("flare.png");
    
    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad"); 
        
        // Below statements is an example of multiple screen density support.
        // (i.e. Retina vs non-Retina, cellular phone vs tablet device).
        if (stage.getWindowWidth() > 320) {
            // if the screen has large display, scale contents twice
            // that makes the stage size by half.
            // This examples shows how to display similar-scale images
            // on Retina and non-Retina display.
            stage.setContentScale(stage.getWindowWidth() / 320.0);
        }
        
        block1.color(1, 1, 0);
        block2.color(1, 1, 0);
        block3.color(1, 1, 0);
        block4.color(1, 1, 0);
        block5.color(1, 1, 0);

        // load sprite to the screen
        block1.load();
        block2.load();
        block3.load();
        block4.load();
        block5.load();
        
        addModifiers();
    }
    
    /*
     * setup block modifiers
     */
    function addModifiers() {
        // clear all modifiers
        block1.clearModifier();
        block2.clearModifier();
        block3.clearModifier();
        block4.clearModifier();
        block5.clearModifier();
        
        // move sprite to the center of the screen
        local x = stage.getWindowWidth() / 5;
        local y = stage.getWindowHeight() - block1.getHeight();
        
        block1.move(x * 0, block1.getHeight());
        block2.move(x * 1, block2.getHeight());
        block3.move(x * 2, block3.getHeight());
        block4.move(x * 3, block4.getHeight());
        block5.move(x * 4, block5.getHeight());
        
        // block1 moves along axis by Linear equation in 5 seconds 
        block1.addModifier(emo.MoveModifier(
            [block1.getX(), block1.getY()],  // from [x, y]
            [block1.getX(), y],              // to   [x, y]
            5000, emo.easing.Linear));       // duration, equation
            
        // block2 moves along axis by BackInOut equation in 5 seconds 
        block2.addModifier(emo.MoveModifier(
            [block2.getX(), block2.getY()],
            [block2.getX(), y],
            5000, emo.easing.BackInOut));

        // block3 moves along axis by BounceOut equation in 5 seconds 
        block3.addModifier(emo.MoveModifier(
            [block3.getX(), block3.getY()],
            [block3.getX(), y],
            5000, emo.easing.BounceOut));
            
        // block4 moves along axis by ElasticInOut equation in 5 seconds 
        block4.addModifier(emo.MoveModifier(
            [block4.getX(), block4.getY()],
            [block4.getX(), y],
            5000, emo.easing.ElasticInOut));
        
        // all sprites are able to have multiple modifiers:
        // block5 moves along axis by Linear equation  and
        // rotates 360 degree by CubicIn equation 
        // in 5 seconds 
        block5.addModifier(emo.MoveModifier(
            [block5.getX(), block5.getY()], // from [x, y]
            [block5.getX(), y],             // to   [x, y]
            5000, emo.easing.Linear));      // duratin, equation
        block5.addModifier(emo.RotateModifier(
            0, 360, 5000, emo.easing.CubicIn));
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
        block1.remove();
        block2.remove();
        block3.remove();
        block4.remove();
        block5.remove();
    }
    
    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            addModifiers();
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
