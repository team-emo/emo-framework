emo_import("emo.nut");

local str = "Hello, Squirrel from File!";
print(str);

local drawed = false;

function onLoad() { 
    print("onLoad");
	emo_options(OPT_ENABLE_ONDRAW_CALLBACK, OPT_ENABLE_PERSPECTIVE_FASTEST);
}

function onGainedFocus() {
    print("onGainedFocus"); 
}

function onLostFocus() {
    print("onLostFocus"); 
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