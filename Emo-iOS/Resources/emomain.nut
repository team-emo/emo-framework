local runtime = emo.Runtime();
local event   = emo.Event();
local stage   = emo.Stage();
local audio   = emo.Audio();

local frameIndex = 0;

class Level_1 {

    logoSprite = emo.Sprite("logo.png");
    dogSprite  = emo.SpriteSheet("dog.png", 34, 42, 1);
    rectangle  = emo.Rectangle();

	audioCh0 = audio.createChannel(0);
		
    function onLoad() {
        print("Level_1:onLoad"); 
        event.enableOnDrawCallback(100);
		
		rectangle.color(1, 0, 0, 1);
		
        print(logoSprite.load());
        print(dogSprite.load(100, 100, 6));
        print(rectangle.load(150, 150, 10, 10));

        dogSprite.animate(0, 5, 1000, 0);
		
		audioCh0.load("tada.wav");
		audioCh0.close();

		audioCh0.load("tada.wav");
		audioCh0.close();
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
        //print("Level_1:onDrawFrame"); 
        local sprite = emo.Sprite("logo.png");
		sprite.load(200, 200);
		sprite.remove();		
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

    kingSprite = emo.SpriteSheet("king.png", 31, 49, 3, 2);

    function onLoad() {
        print("Level_2:onLoad"); 
        event.enableOnDrawCallback(5000);
        print(kingSprite.load(200, 200, 6));
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
