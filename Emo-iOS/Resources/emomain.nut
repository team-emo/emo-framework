local str = "Hello, Squirrel from File!";
print(str);

function onLoad() { 
    print("onLoad"); 
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
