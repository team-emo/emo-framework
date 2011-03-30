local runtime = emo.Runtime();
local event   = emo.Event();

local audio = emo.AudioManager();

local audioCh0 = emo.AudioChannel(0, audio);
local audioCh1 = emo.AudioChannel(1, audio);

runtime.info("This is Log Info");
runtime.error("This is Log Error");
runtime.warn("This is Log Warn");

print(runtime.echo("Hello, Runtime!"));

local str = "Hello, Squirrel from File!";
print(str);

function emo::onLoad() { 
    print("onLoad");

    runtime.setOptions(OPT_WINDOW_KEEP_SCREEN_ON, OPT_ENABLE_PERSPECTIVE_NICEST);

    event.registerSensors(SENSOR_TYPE_ACCELEROMETER);
    event.enableOnDrawCallback(5000);

    audioCh0.load("drums.wav");
    audioCh1.load("clang.wav");
    audioCh0.play();
}

function emo::onGainedFocus() {
    print("onGainedFocus"); 
    event.enableSensor(SENSOR_TYPE_ACCELEROMETER, 100);
}

function emo::onLostFocus() {
    print("onLostFocus"); 
    audioCh0.stop();
    audioCh1.stop();
    event.disableSensor(SENSOR_TYPE_ACCELEROMETER);
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
        audioCh1.play();

        print("Ch0: " + audioCh0.getVolume() + " - " + audioCh0.getMaxVolume());
        print("Ch1: " + audioCh0.getVolume() + " - " + audioCh1.getMaxVolume());

        audioCh0.setVolume(audioCh0.getVolume() -1);
    }
}

function emo::onKeyEvent(...) {
    local keyEvent = emo.KeyEvent(vargv);
    print("KeyEvent: " + keyEvent.toString());
    if (keyEvent.getAction() == KEY_EVENT_ACTION_DOWN && keyEvent.getKeyCode() != KEYCODE_BACK) {
        runtime.finish();
        return true;
    }
    if (keyEvent.getKeyCode() == KEYCODE_BACK) {
        return true;
    }
    return false;
}

function emo::onSensorEvent(...) {
    local sensorEvent = emo.KeyEvent(vargv);
    print("SensorEvent: " + sensorEvent.toString());

}
