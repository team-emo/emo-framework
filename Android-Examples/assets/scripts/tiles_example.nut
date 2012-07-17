local stage = emo.Stage();

/*
 * This example shows one block sprite sheet that changes frame index on touch-down event
 */
class Main {

    // defines a sprite sheet that consists of 32x32 block with 2 pixel border and 2 pixel margin.
    block = emo.SpriteSheet("blocks.png", 32, 32, 2, 2);
    // On iOS, emo.SpriteSheet supports PVR compression with no-mipmap texture:
    // block = emo.SpriteSheet("potblocks.pvr", 32, 32, 0, 0);

    // current frame index
    currentFrame = 0;

    // 16x16 text sprite with 2 pixel border and 1 pixel margin
    text = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
    
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
        local x = (stage.getWindowWidth()  - block.getWidth())  / 2;
        local y = (stage.getWindowHeight() - block.getHeight()) / 2;
        
        block.move(x, y);

        // load sprite to the screen
        block.load();
        
        // change the text
        text.setText("TAP TO CHANGE THE TILE");
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
        block.remove();
        text.remove();
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
