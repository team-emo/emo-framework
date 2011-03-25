local runtime = Runtime();
local event   = Event();

local str = "Hello, Squirrel from File!";
print(str);

local drawed = false;

function onLoad() { 
    print("onLoad");

    runtime.setOptions(OPT_ENABLE_ONDRAW_CALLBACK, OPT_ENABLE_PERSPECTIVE_FASTEST, OPT_WINDOW_KEEP_SCREEN_ON);
    event.registerSensors(SENSOR_TYPE_ACCELEROMETER, SENSOR_TYPE_GYROSCOPE);
}

function onGainedFocus() {
    print("onGainedFocus"); 
    event.enableSensor(SENSOR_TYPE_ACCELEROMETER, 0.1);
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
    if (!drawed) {
        print("onDrawFrame");
        drawed = true;
    }
}

function onLowMemory() {
    print("onLowMemory");
}

function onMotionEvent(...) {
    local sb = "MotionEvent: ";
    for(local i = 0; i < vargv.len(); i++) {
        sb = sb + vargv[i] + " ";
    }
    print(sb);
}

function onKeyEvent(...) {
    local sb = "KeyEvent: ";
    for(local i = 0; i < vargv.len(); i++) {
        sb = sb + vargv[i] + " ";
    }
    print(sb);
}
