local stage = emo.Stage();

/*
 * This example shows a sprite atlas that uses packed texture
 */
class Main {

    // defines a sprite sheet that uses texture atlas.
    // XML formatted texture atlas file used by Sparrow & OGRE/CEGUI is supported by emo.
    // Those xml can be create by using packing tools like TexturePacker http://www.texturepacker.com/
    block1 = emo.SpriteSheet("icons_pack_OGRE.xml");    // Uses OGRE file
    block2 = emo.SpriteSheet("icons_pack_Sparrow.xml"); // Uses Sparrow file
    
    // On iOS, emo.SpriteSheet supports PVR compression with no-mipmap texture:
    // block2 = emo.SpriteSheet("icons_pack_Sparrow_pvr.xml");

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
        local x = (stage.getWindowWidth()  - block1.getWidth())  / 2;
        local y = (stage.getWindowHeight() - block1.getHeight()) / 2;
        
        block1.move(x, y);
        block2.move(block1.getX(), block1.getY() + block1.getHeight());

        // load sprite to the screen
        block1.load();
        block2.load();
                
        // Frame selection by both index and image name is supported.
        // Note that do not call setFrame/selectFrame BEFORE calling SpriteSheet#load()
        // otherwise the texture will not be updated correctly.
        //
        //block1.setFrame(2);
        //block2.selectFrame("ball");

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
        block1.remove();
        text.remove();
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        // change frame index on touch down event
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            currentFrame++;
            if (currentFrame >= block1.getFrameCount()) {
                currentFrame = 0;
            }
            block1.setFrame(currentFrame);
            block2.setFrame(currentFrame);
            
            block1.moveCenter(stage.getWindowWidth() / 2, stage.getWindowHeight() / 2);
            block2.move(block1.getX(), block1.getY() + block1.getHeight());
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
