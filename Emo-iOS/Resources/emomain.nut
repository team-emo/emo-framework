local runtime = emo.Runtime();
local event   = emo.Event();

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
    event.enableOnDrawCallback(1000);
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
} 

function emo::onError(msg) {
    print("onError: " + msg);
}

function emo::onDrawFrame() {
    print("onDrawFrame");
}

function emo::onLowMemory() {
    print("onLowMemory");
}

function emo::onMotionEvent(motionEvent) {
    print("MotionEvent: " + motionEvent.toString());
}

function emo::onKeyEvent(keyEvent) {
    print("KeyEvent: " + keyEvent.toString());
}
