local stage = emo.Stage();

// create audio manager with 3 channels.
local audio = emo.Audio(3);

// select audio channel 1 (index=0) to play background(drums.wav).
local audioCh1 = audio.createChannel(0);

// select audio channel 2 (index=1) to play clang.wav.
local audioCh2 = audio.createChannel(1);

// select audio channel 3 (index=2) to play tada.wav.
local audioCh3 = audio.createChannel(2);

/*
 * This example of multiple audio channels
 *
 * Supported audio format
 * For iOS: * * emo uses 'Extended Audio File Services' for iOS 
 * so following format can be loaded through the channel.
 * AIFF (.aif, .aiff), CAF (.caf), MPEG-1 to 4 (.mp1, .mp2, .mp3,
 * .acc, .m4a, .mp4), WAV (.wav), 3gpp (.3gp), 3gp2 (.3g2).. etc. 
 *
 * See http://developer.apple.com/library/mac/#documentation/MusicAudio/Reference/AudioFileConvertRef/Reference/reference.html
 * Extended Audio File Services: Built-In Audio File Types for details. 
 * 
 *
 * For Android:
 * 
 * Only PCM-WAVE audio file is supported.
 */
class Main {

    // 16x16 text sprite with 2 pixel border and 1 pixel margin
    text = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
    clangButton = emo.Rectangle();
    tadaButton  = emo.Rectangle();

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
        
        // load the audio.
        audioCh1.load("drums.wav");
        audioCh2.load("clang.wav");
        audioCh3.load("tada.wav");
        
        // enable looping for the background audio
        // NOTE:  calling setLoop before loading the audio will fail.
        audioCh1.setLoop(true);
        
        text.setText("TAP THE BOX");
        
        // move sprite to the center of the screen
        text.moveCenter(stage.getWindowWidth()  / 2,
                        stage.getWindowHeight() / 2);
                        
        // set z-order of the shapes
        text.setZ(0);
        clangButton.setZ(1);
        tadaButton.setZ(1);

        // load sprite to the screen
        text.load();
        
        local x = stage.getWindowWidth()  / 2;
        local y = stage.getWindowHeight() / 2;
        
        clangButton.setSize(50, 50);
        clangButton.moveCenter(x - 25, y + text.getHeight() + clangButton.getHeight());
        clangButton.color(1, 0, 0);
        clangButton.load();
        
        tadaButton.setSize(50, 50);
        tadaButton.moveCenter(x + 25, y + text.getHeight() + tadaButton.getHeight());
        tadaButton.color(0, 0, 1);
        tadaButton.load();
    }

    /*
     * Called when the app has gained focus
     */
    function onGainedFocus() {
        print("onGainedFocus");
        
        // play the background audio.
        audioCh1.play();
    }

    /*
     * Called when the app has lost focus
     */
    function onLostFocus() {
        print("onLostFocus"); 
        
        // pause the audio
        audioCh1.pause();
        audioCh2.pause();
        audioCh3.pause();
    }

    /*
     * Called when the class ends
     */
    function onDispose() {
        print("onDispose");
        
        // remove sprite from the screen
        text.remove();
        
        // close the audio
        audioCh1.close();
        audioCh2.close();
        audioCh3.close();
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            if (clangButton.contains(mevent.getX(), mevent.getY())) {
                clangButton.scale(2, 2);
                
                // bring the button to front
                clangButton.setZ(2);
                tadaButton.setZ(1);
                
                audioCh2.play(true);
            } else if (tadaButton.contains(mevent.getX(), mevent.getY())) {
                tadaButton.scale(2, 2);
                
                // bring the button to front
                clangButton.setZ(1);
                tadaButton.setZ(2);
                
                audioCh3.play(true);
            }
        } else if (mevent.getAction() == MOTION_EVENT_ACTION_UP) {
            clangButton.scale(1, 1);
            tadaButton.scale(1, 1);
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
