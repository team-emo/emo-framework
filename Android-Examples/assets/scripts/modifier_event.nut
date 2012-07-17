local stage = emo.Stage();
local event = emo.Event();

/*
 * This example shows splash screen.
 */
class Splash {

    // iOS users: do not use special resource like 'Default.png'(default splash image)
    //            as sprite resource otherwise the app will be frozen for no reason.
    //            now we use the copy of Default.png as splash sprite.
    splash = emo.Sprite("Splash.png");
    
    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("Splash::onLoad"); 
        
        // Below statements is an example of multiple screen density support.
        // (i.e. Retina vs non-Retina, cellular phone vs tablet device).
        if (stage.getWindowWidth() > 320) {
            // if the screen has large display, scale contents twice
            // that makes the stage size by half.
            // This examples shows how to display similar-scale images
            // on Retina and non-Retina display.
            stage.setContentScale(stage.getWindowWidth() / 320.0);
        }
        
        // move sprite to the center of the screen
        splash.moveCenter(stage.getWindowWidth() / 2, stage.getWindowHeight() / 2);
        
        // add modifier to fade in the splash sprite.
        splash.addModifier(emo.AlphaModifier(0, 1, 1000, emo.easing.CubicIn));
        
        // load sprite to the screen
        splash.load();
        
        // onDrawFrame(dt) will be called 5 seconds later
        event.enableOnDrawCallback(3000);
    }
    
    /*
     * Called when the class ends
     */
    function onDispose() {
        print("Splash::onDispose");
        
        // remove sprite from the screen
        splash.remove();
    }
    /*
     * Enabled after onDrawCalleback event is enabled by enableOnDrawCallback
     * dt parameter is a delta time (millisecond)
     */
    function onDrawFrame(dt) {
        print("Splash::onDrawFrame");
        
        // disable onDraw listener because this is only one time event.
        event.disableOnDrawCallback();

        // create a 1 second modifier to fade out the splash screen
        local modifier = emo.AlphaModifier(1, 0, 1000, emo.easing.CubicOut);
        
        // set modifier event listener. 
        // onModifierEvent is called when modifier event occurs.
        modifier.setEventListener(this);
        
        // add modifier to the splash sprite.
        splash.addModifier(modifier);
    }

    // This function is called by modifier of the splash sprite.
    // eventType equals EVENT_MODIFIER_FINISH if the modifier ends.
    function onModifierEvent(obj, modifier, eventType) {
        if (eventType == EVENT_MODIFIER_FINISH) {
            // now splash animation is completed so proceed to the next level.
            stage.load(Main());
        }
    }
}

/*
 * This is the main class of your game.
 */
class Main {
    text = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);

    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("Main::onLoad");
        text.setText("CLICK TO START");
        text.moveCenter(stage.getWindowWidth()  / 2,
                        stage.getWindowHeight() / 2);
        text.load();
    }

    /*
     * Called when the app has gained focus
     */
    function onGainedFocus() {
        print("Main::onGainedFocus");
    }

    /*
     * Called when the app has lost focus
     */
    function onLostFocus() {
        print("Main::onLostFocus"); 
    }

    /*
     * Called when the class ends
     */
    function onDispose() {
        print("Main::onDispose");
    }
        
    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            text.setText("STARTED");
            text.moveCenter(stage.getWindowWidth()  / 2,
                            stage.getWindowHeight() / 2);
        }
    }
}
function emo::onLoad() {
    stage.load(Splash());
}
