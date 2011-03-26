local runtime = emo.Runtime();
local event   = emo.Event();

runtime.echo("Hello, Runtime!");

local str = "Hello, Squirrel from File!";
print(str);

function onLoad() { 
    print("onLoad");

    runtime.setOptions(OPT_WINDOW_KEEP_SCREEN_ON, OPT_ENABLE_PERSPECTIVE_NICEST);

    event.registerSensors(SENSOR_TYPE_ACCELEROMETER);
    event.enableOnDrawCallback(1000);
}

function onGainedFocus() {
    print("onGainedFocus"); 
    event.enableSensor(SENSOR_TYPE_ACCELEROMETER, 100);
}

function onLostFocus() {
    print("onLostFocus"); 
    event.disableSensor(SENSOR_TYPE_ACCELEROMETER);
}

function onDispose() {
    print("onDispose"); 
} 

function onError(msg) {
    print("onError: " + msg);
}

function onDrawFrame() {
    print("onDrawFrame");
}

function onLowMemory() {
    print("onLowMemory");
}

function onMotionEvent(motionEvent) {
    print("MotionEvent: " + motionEvent.toString());
}

function onKeyEvent(keyEvent) {
    print("KeyEvent: " + keyEvent.toString());
}
