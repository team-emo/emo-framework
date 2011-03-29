local runtime = emo.Runtime();
local event   = emo.Event();
local audio   = emo.Audio();

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

    audio.createEngine(2);
    audio.load(0, "waterfall.wav");
    audio.load(1, "clank.wav");
}

function emo::onGainedFocus() {
    print("onGainedFocus"); 
    event.enableSensor(SENSOR_TYPE_ACCELEROMETER, 100);
}

function emo::onLostFocus() {
    print("onLostFocus"); 
    event.disableSensor(SENSOR_TYPE_ACCELEROMETER);
}

function emo::onDispose() {
    print("onDispose"); 
    audio.closeEngine();
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
        audio.play(0);
        audio.play(1);
        print("0: " + audio.getVolume(0) + " - " + audio.getMaxVolume(0));
        print("1: " + audio.getVolume(1) + " - " + audio.getMaxVolume(1));
        audio.setVolume(0, audio.getVolume(0) -1);
    }
}

function emo::onKeyEvent(...) {
    local keyEvent = emo.KeyEvent(vargv);
    print("KeyEvent: " + keyEvent.toString());
}

function emo::onSensorEvent(...) {
    local sensorEvent = emo.KeyEvent(vargv);
    print("SensorEvent: " + sensorEvent.toString());

}
