local stage = emo.Stage();
local runtime = emo.Runtime();

/*
 * This example shows a font sprite that uses native fonts and 
 * changes text on touch-down event
 */
class Main {

    text1 = null;
    text2 = null;
    text3 = null;
    text4 = null;
    text5 = null;

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

        // The first parameter is the property name for the string.
        // The second parameter is the font size.
        //
        // The property value is set in the strings property file.
        // See "res/values/strings.xml"  for Android,
        // and "Resources/strings.plist" for iOS.
        
        text1 = emo.FontSprite("hello", 30);
        text2 = emo.FontSprite("hello_ja", 30);
        text3 = emo.FontSprite("hello_kr", 30);
        text4 = emo.FontSprite("hello_ru", 30);
        
        // To use with custom TrueType fonts:
        // 
        // For iOS: Add UIAppFonts property into your Info.plist
        // FYI: http://richardwarrender.com/2010/08/custom-fonts-on-ipad-and-iphone/
        text5 = emo.FontSprite("hello_font", 30, "TestarossaNF"); 
        
        // additional parameter can be set for formatted string.
        // e.g. "Hello, %s!"
        //
        // Note that only ASCII character can be set to this parameter
        text1.setParam("World");
        
        // At most 6 parameter can be set to the formatted string
        //text1.setParam("World", "2", "3", "4", "5", "6");
        
        // load sprite to the screen
        text1.load();
        text2.load();
        text3.load();
        text4.load();
        text5.load();
        
        text1.move(0, 0);
        text2.move(0, text1.getY() + text1.getHeight());
        text3.move(0, text2.getY() + text2.getHeight());
        text4.move(0, text3.getY() + text3.getHeight());
        text5.move(0, text4.getY() + text4.getHeight());
        
        // you can change the font color just as same as the normal sprite
        text1.color(1, 0, 0, 1);
        text2.color(1, 1, 0, 1);
        text3.color(1, 0, 1, 1);
        text4.color(0, 1, 1, 1);
        text5.color(0, 1, 0, 1);
        
        // print current locale
        print(runtime.getDefaultLocale());
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
        text1.remove();
    }

    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            // the string parameter can be updated at runtime. 
            text1.setParam("MotionEvent");
            text1.reload();
            
            text2.reload("hello_gr");
        }
    }
}

function emo::onLoad() {
    stage.load(Main());
}
