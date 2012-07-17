local stage = emo.Stage();

// create audio manager with 1 channel.
local audio = emo.Audio(1);

// select audio channel 1 (index=0) to play the audio.
// you can handle audios through this channel.
local audioCh1 = audio.createChannel(0);

/*
 * This example of audio
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
    
    volText = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
    maxText = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
    minText = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
    
    volumeLevel = 100;

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
        
        // enable looping for the background audio
        // NOTE:  calling setLoop before loading the audio will fail.
        audioCh1.setLoop(true);
        
        text.setText("TAP TO CHANGE VOLUME");
        volText.setText("VOL: ");
        maxText.setText("MAX: ");
        minText.setText("MIN: ");
        
        // move sprite to the center of the screen
        local x = (stage.getWindowWidth()  - text.getWidth())  / 2;
        local y = (stage.getWindowHeight() - text.getHeight()) / 2;
        
        text.move(x, y);
        volText.move(x, y + text.getHeight());
        maxText.move(x, y + text.getHeight() * 2);
        minText.move(x, y + text.getHeight() * 3);
        
        // load sprite to the screen
        text.load();
        volText.load();
        maxText.load();
        minText.load();
    }

    /*
     * Called when the app has gained focus
     */
    function onGainedFocus() {
        print("onGainedFocus");
        
        // play the audio.
        audioCh1.play();
        
        volText.setText(format("VOL: %4.2f", audioCh1.getVolume()));
        maxText.setText(format("MAX: %4.2f", audioCh1.getMaxVolume()));
        minText.setText(format("MIN: %4.2f", audioCh1.getMinVolume()));
    }

    /*
     * Called when the app has lost focus
     */
    function onLostFocus() {
        print("onLostFocus"); 
        
        // pause the audio
        audioCh1.pause();
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
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            if (audioCh1.getVolume() > audioCh1.getMinVolume()) {
                volumeLevel--;
            } else {
                volumeLevel = 100;
            }
            audioCh1.setVolume(audioCh1.getMaxVolume() / 100.0 * volumeLevel);
            volText.setText(format("VOL: %4.2f", audioCh1.getVolume()));
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
