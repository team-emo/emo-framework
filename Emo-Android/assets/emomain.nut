local runtime = emo.Runtime();
local event   = emo.Event();
local stage   = emo.Stage();

class Level1 {

    dogSprite  = emo.Sprite("dog.png");

    function onLoad() {
        print("Level1:onLoad"); 
        event.enableOnDrawCallback(5000);
        print(dogSprite.loadSheet(0, 0, 6, 34, 42, 1));
    }

    function onGainedFocus() {
        print("Level1:onGainedFocus"); 

    }

    function onLostFocus() {
        print("Level1:onLostFocus"); 

    }

    function onDispose() {
        print("Level1:onDispose"); 
        dogSprite.remove();
    }

    function onDrawFrame(dt) {
        print("Level1:onDrawFrame"); 

    }

    function onMotionEvent(motionEvent) {
        print("Level1:onMotionEvent " + motionEvent.toString()); 
        if (motionEvent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            stage.load(Level2());
        }
    }

    function onKeyEvent(keyEvent) {
        print("Level1:KeyEvent " + keyEvent.toString());
        return false;
    }
}

class Level2 {

    kingSprite = emo.Sprite("king.png");

    function onLoad() {
        print("Level2:onLoad"); 
        event.enableOnDrawCallback(5000);
        print(kingSprite.loadSheet(100, 100, 6, 31, 49, 2, 3));
    }
}

function emo::onLoad() {
    stage.load(Level1());
}
