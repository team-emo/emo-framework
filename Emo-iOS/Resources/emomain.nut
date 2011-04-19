local runtime = emo.Runtime();
local event   = emo.Event();
local stage   = emo.Stage();

class Level_1 {

    dogSprite  = emo.Sprite("dog.png");
    logoSprite = emo.Sprite("logo.png");
    rectangle  = emo.Rectangle();

    function onLoad() {
        print("Level_1:onLoad"); 
        event.enableOnDrawCallback(5000);
        print(dogSprite.loadSheet(100, 100, 6, 34, 42, 1));
        print(logoSprite.load());
        print(rectangle.load(150, 150, 1, 1));
    }

    function onGainedFocus() {
        print("Level_1:onGainedFocus"); 
    }

    function onLostFocus() {
        print("Level_1:onLostFocus"); 
    }

    function onDispose() {
        print("Level_1:onDispose"); 
        dogSprite.remove();
		rectangle.remove();
    }

    function onDrawFrame(dt) {
        print("Level_1:onDrawFrame"); 

    }

    function onMotionEvent(motionEvent) {
        print("Level_1:onMotionEvent " + motionEvent.toString()); 
        if (motionEvent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            stage.load(Level_2());
        }
    }

    function onKeyEvent(keyEvent) {
        print("Level_1:KeyEvent " + keyEvent.toString());
        return false;
    }
}

class Level_2 {

    kingSprite = emo.Sprite("king.png");

    function onLoad() {
        print("Level_2:onLoad"); 
        event.enableOnDrawCallback(5000);
        print(kingSprite.loadSheet(100, 100, 6, 31, 49, 3, 2));
    }

    function onGainedFocus() {
        print("Level_2:onGainedFocus"); 
    }

    function onLostFocus() {
        print("Level_2:onLostFocus"); 
    }

    function onDispose() {
        print("Level_2:onDispose"); 
        kingSprite.remove();
    }
}

function emo::onLoad() {
    stage.load(Level_1());
}
