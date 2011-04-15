local runtime = emo.Runtime();
local event   = emo.Event();
local stage   = emo.Stage();

class Level_1 {

    dogSprite  = emo.Sprite("dog.png");

    function onLoad() {
        print("Level_1:onLoad"); 
        event.enableOnDrawCallback(5000);
        print(dogSprite.loadSheet(0, 0, 6, 34, 42, 1));
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
        print(kingSprite.loadSheet(100, 100, 6, 31, 49, 2, 3));
    }

    function onGainedFocus() {
        print("Level_2:onGainedFocus"); 

    }

    function onLostFocus() {
        print("Level_2:onLostFocus"); 
    }

    function onDispose() {
        print("Level_2:onDispose"); 
        dogSprite.remove();
    }
}

function emo::onLoad() {
    stage.load(Level_1());
}
