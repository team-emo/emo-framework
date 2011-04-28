local runtime = emo.Runtime();
local event   = emo.Event();
local stage   = emo.Stage();

local audio = emo.Audio();
local audioCh0 = audio.createChannel(0);

local frameIndex = 0;

class Level_1 {

    rectangle   = emo.Rectangle();
    logoSprite  = emo.Sprite("logo.png");
    dogSprite   = emo.SpriteSheet("dog.png", 34, 42, 1);
    mapSprite   = emo.MapSprite("blocks.png", 33, 33, 1);

    function onLoad() {
        print("Level_1:onLoad"); 

		rectangle.color(1, 0, 0, 1);
        rectangle.setSize(150, 10);

        dogSprite.animate(0, 5, 1000, 0);

        mapSprite.setZ(0);
        logoSprite.setZ(1);
        dogSprite.setZ(2);
        rectangle.setZ(3);

        logoSprite.load();
        dogSprite.load();
        rectangle.load();

        local tiles = [
                       [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12],
                       [11, 12, 13, 14, 15, 16, 17, -1, -1, -1, 0, 1, 2],
                       [-1,  6,  7,  8,  9, 10, 11, -1, -1, 0, 1, 2, 3],
                       [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12],
                       [11, 12, 13, 14, 15, 16, 17, -1, -1, -1, 0, 1, 2],
                       [-1,  6,  7,  8,  9, 10, 11, -1, -1, 0, 1, 2, 3],
                       [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12],
                       [11, 12, 13, 14, 15, 16, 17, -1, -1, -1, 0, 1, 2],
                       [-1,  6,  7,  8,  9, 10, 11, -1, -1, 0, 1, 2, 3],
                       [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12],
                       [11, 12, 13, 14, 15, 16, 17, -1, -1, -1, 0, 1, 2],
                       [-1,  6,  7,  8,  9, 10, 11, -1, -1, 0, 1, 2, 3],
                       [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12],
                       [11, 12, 13, 14, 15, 16, 17, -1, -1, -1, 0, 1, 2],
                       [-1,  6,  7,  8,  9, 10, 11, -1, -1, 0, 1, 2, 3]
                      ];
        mapSprite.setTile(tiles);
        mapSprite.load();
/*
        event.enableOnDrawCallback(100);
        //stage.interval(500);
		
		rectangle.color(1, 0, 0, 1);
		
        logoSprite.load();
        dogSprite.load(100, 100, 6);
        rectangle.load(150, 150, 10, 10);

        dogSprite.animate(0, 5, 1000, 0);
		
		audioCh0.load("tada.wav");
		audioCh0.play();

        local tiles = [
                       [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12],
                       [11, 12, 13, 14, 15, 16, 17, -1, -1, -1, 0, 1, 2],
                       [-1,  6,  7,  8,  9, 10, 11, -1, -1, 0, 1, 2, 3],
                       [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12],
                       [11, 12, 13, 14, 15, 16, 17, -1, -1, -1, 0, 1, 2],
                       [-1,  6,  7,  8,  9, 10, 11, -1, -1, 0, 1, 2, 3],
                       [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12],
                       [11, 12, 13, 14, 15, 16, 17, -1, -1, -1, 0, 1, 2],
                       [-1,  6,  7,  8,  9, 10, 11, -1, -1, 0, 1, 2, 3],
                       [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12],
                       [11, 12, 13, 14, 15, 16, 17, -1, -1, -1, 0, 1, 2],
                       [-1,  6,  7,  8,  9, 10, 11, -1, -1, 0, 1, 2, 3],
                       [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12],
                       [11, 12, 13, 14, 15, 16, 17, -1, -1, -1, 0, 1, 2],
                       [-1,  6,  7,  8,  9, 10, 11, -1, -1, 0, 1, 2, 3]
                      ];
        mapSprite.setTile(tiles);
        mapSprite.load(0);
*/
    }

    function onGainedFocus() {
        print("Level_1:onGainedFocus");
    }

    function onLostFocus() {
        print("Level_1:onLostFocus"); 
    }

    function onDispose() {
        print("Level_1:onDispose"); 
        mapSprite.remove();
        //dogSprite.remove();
		//rectangle.remove();
    }

    function onDrawFrame(dt) {
        mapSprite.move(mapSprite.getX(), mapSprite.getY() - 2);
    }

    function onCallback(name, value) {
        print("name: "  + name);
        print("value: " + value);
    }

    function onMotionEvent(motionEvent) {
        print("Level_1:onMotionEvent " + motionEvent.toString()); 
        if (motionEvent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            //stage.load(Level_2());
            mapSprite.remove();
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
