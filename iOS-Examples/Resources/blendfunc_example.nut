local stage = emo.Stage();

/*
 * This example shows multiple sprites with blend function that can be moved by on-screen controller
 */
class Main {

    sprite1 = emo.SpriteSheet("dog.png", 34, 42, 1, 1);
    sprite2 = emo.SpriteSheet("dog.png", 34, 42, 1, 1);
    
    controller1 = emo.AnalogOnScreenController(
        "control_base.png", "control_knob.png");
    controller2 = emo.AnalogOnScreenController(
        "control_base.png", "control_knob.png");

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
        
        // move sprite to the center of the screen
        local x = (stage.getWindowWidth()  - sprite1.getWidth())  / 2;
        local y = (stage.getWindowHeight() - sprite1.getHeight()) / 2;
        
        sprite1.move(x - sprite1.getWidth(), y);
        sprite2.move(x + sprite2.getWidth(), y);
        
        // sprite2 draws over sprite1 that makes blending success
        sprite1.setZ(1);
        sprite2.setZ(2);
        
        controller1.move(0, stage.getWindowHeight() - controller1.getHeight());
        controller2.move(stage.getWindowWidth()  - controller2.getWidth(),
                         stage.getWindowHeight() - controller2.getHeight());
                         
        // onControlEvent is fired every 16 milliseconds at most
        // the default updateInterval equals 100 milliseconds
        controller1.updateInterval = 16;
        controller2.updateInterval = 16;

        //
        // sprite accepts blending function detailed below:
        // http://www.opengl.org/resources/faq/technical/transparency.htm
        // http://www.opengl.org/sdk/docs/man/xhtml/glBlendFunc.xml
        // 
        // default blending is alpha blending:
        // sprite2.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // alpha blending
        // 
        // sprite2.blendFunc(GL_SRC_ALPHA, GL_ONE);  // additive blending
        // sprite2.blendFunc(GL_ZERO, GL_SRC_COLOR);
        // sprite2.blendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
        // sprite2.blendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
        
        sprite2.blendFunc(GL_SRC_ALPHA, GL_ONE);  // additive blending
        
        // load sprite into the screen
        sprite1.load();
        sprite2.load();
        controller1.load();
        controller2.load();
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
        sprite1.remove();
        sprite2.remove();
        controller1.remove();
        controller2.remove();
    }

    /*
     * on-screen controller event
     * controlX and controlY takes value from -100 to 100.
     */
    function onControlEvent(controller, controlX, controlY, hasChanged) {
        local sprite = null;
        if (controller == controller1) {
            sprite = sprite1;
        } else {
            sprite = sprite2;
        }
        local nextX = sprite.getX() + (controlX / 10);
        local nextY = sprite.getY() + (controlY / 10);
        
        if (nextX < 0 || nextX > stage.getWindowWidth() - sprite.getWidth()) {
            nextX = sprite.getX();
        }
        if (nextY < 0 || nextY > stage.getWindowHeight() - sprite.getHeight()) {
            nextY = sprite.getY();
        }
        
        sprite.move(nextX, nextY);
        
        if (hasChanged) {
            local direction = controller.getDirection();
            if (direction == CONTROL_LEFT) {
                sprite.setFrame(5);
            } else if (direction == CONTROL_RIGHT) {
                sprite.setFrame(0);
            } else if (direction == CONTROL_UP) {
                sprite.setFrame(1);
            } else if (direction == CONTROL_DOWN) {
                sprite.setFrame(2);
            } else {
                sprite.setFrame(0);
            }
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
