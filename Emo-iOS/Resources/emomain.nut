runtime <- emo.Runtime();
event   <- emo.Event();
stage   <- emo.Stage();

local str = "Hello, Squirrel from File!";
print(str);

local dogSprite = emo.Sprite("robot.png");
local dogSprite2 = emo.Sprite("logo.png");

function emo::onLoad() { 
    print("onLoad");

    runtime.setOptions(OPT_WINDOW_KEEP_SCREEN_ON, OPT_ENABLE_PERSPECTIVE_NICEST);

    event.enableOnDrawCallback(5000);

    print(dogSprite.load());
}

function emo::onGainedFocus() {
    print("onGainedFocus"); 
}

function emo::onLostFocus() {
    print("onLostFocus"); 
}

function emo::onDispose() {
    print("onDispose");
} 

function emo::onError(msg) {
    print("onError: " + msg);
}

function emo::onDrawFrame(dt) {
    print("onDrawFrame: " + dt);
}

function emo::onLowMemory() {
    print("onLowMemory");
}

function emo::onMotionEvent(...) {
    local motionEvent = emo.MotionEvent(vargv);
    print("MotionEvent: " + motionEvent.toString());
    if (motionEvent.getAction() == MOTION_EVENT_ACTION_DOWN) {
        print(dogSprite.remove());
        print(dogSprite2.load());
    }
}

function emo::onKeyEvent(...) {
    local keyEvent = emo.KeyEvent(vargv);
    print("KeyEvent: " + keyEvent.toString());
    return false;
}

function emo::onSensorEvent(...) {
    local sensorEvent = emo.KeyEvent(vargv);
    print("SensorEvent: " + sensorEvent.toString());

}
