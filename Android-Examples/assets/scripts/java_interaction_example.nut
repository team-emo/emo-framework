class Main {
    
    secondTimer = null;
        
    counter = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
    text = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
    result = emo.TextSprite("font_16x16.png",
        " !\"c*%#'{}@+,-./0123456789:;[|]?&ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        16, 16, 2, 1);
        
    /*
     * Called when this class is loaded
     */
    function onLoad() {
        print("onLoad"); 
        
        setSecondTimer();
        
        text.setText("TAP TO START JAVA ACTIVIY");
        text.setZ(1);
        text.moveCenter(emo.Stage.getWindowWidth() * 0.5, emo.Stage.getWindowHeight() * 0.5);
        text.load();
        
        counter.setText("0");
        counter.setZ(1);
        counter.moveCenter(emo.Stage.getWindowWidth() * 0.5, emo.Stage.getWindowHeight() * 0.1);
        counter.load();
        
        result.setZ(1);
        result.hide();
        result.load();
    }
    
    /*
     * touch event
     */
    function onMotionEvent(mevent) {
        if (mevent.getAction() == MOTION_EVENT_ACTION_DOWN) {
            print(format("EVENT: %dx%d", mevent.getX(), mevent.getY()));
            
            local intent = emo.Android.Intent("com.emo_framework.examples.JavaActivity");
            intent.putExtra("string", "Hello!");
            intent.putExtra("integer", 99999);
            intent.putExtra("float", 2.5686);
            intent.putExtra("bool", true);
            intent.putExtra("string_array", ["this", "is", "an", "array"] );
            intent.putExtra("integer_array", [2012, 7, 15] );
            
            intent.setFlag(FLAG_ACTIVITY_CLEAR_TOP | FLAG_ACTIVITY_SINGLE_TOP);
            
            emo.Android.transit(intent, 102);
        }
    }
    
    /*
     * Called when this Activity gets back from other Activities
     */
    function onReturn(requestCode, resultCode, className, extras){
        local realTimePassed = floor(EMO_RUNTIME_STOPWATCH.realElapsed() * 0.001) ;
        print("Real time passed : " + realTimePassed);
        
        if(resultCode == ANDROID_RESULT_OK ){
            // retrieve extras
            local resultString = extras.rawget("string");
            local aButtonFlag  = extras.rawget("aButtonFlag");
            local array;
            if(aButtonFlag == true){
                array = extras.rawget("intArray");
            }else{
                array = extras.rawget("stringArray");
            }
        
            // show result to the screen
            result.hide();
            result.setText( resultString );
            result.moveCenter(emo.Stage.getWindowWidth() * 0.5, emo.Stage.getWindowHeight() * 0.75);
            result.show();
        
            print("Request code      : " + requestCode);
            print("Result code       : " + resultCode);
            print("Source class name : " + className);
            
            for(local i = 0; i < array.len() ; i++){
                print("index [" + i + "] : " + array[i]);
            }
        }else{
            local resultString = "YOU CANCELED";
            
            // show result to the screen
            result.hide();
            result.setText( resultString );
            result.moveCenter(emo.Stage.getWindowWidth() * 0.5, emo.Stage.getWindowHeight() * 0.75);
            result.show();
            
            print("Activity canceled");
        }
    }
    
    
    function onDrawFrame(dt){
        updateGameTime();
    }

    /*
     * Called when the app has gained focus
     */
    function onGainedFocus() {
        emo.Event().enableOnDrawCallback(500);
        print("onGainedFocus");
    }

    /*
     * Called when the app has lost focus
     */
    function onLostFocus() {
        emo.Event().disableOnDrawCallback();
        print("onLostFocus"); 
    }

    /*
     * Called when the class ends
     */
    function onDispose() {
        print("onDispose");
    }
    
    function updateGameTime(){
        // !!FUNCTION "isGSecondPassed()" MUST BE CALLED EACH TIME STEP!!
        if(isSecondPassed() == true){
            // correct game time
            local gameSeconds = floor(EMO_RUNTIME_STOPWATCH.elapsed() * 0.001) ;
            counter.hide();
            counter.setText( gameSeconds.tostring() );
            counter.moveCenter(emo.Stage.getWindowWidth() * 0.5, emo.Stage.getWindowHeight() * 0.1);
            counter.show();
        }
    }

    function setSecondTimer(){
        secondTimer = EMO_RUNTIME_STOPWATCH.elapsed();
    }
    
    function isSecondPassed(){
        if (EMO_RUNTIME_STOPWATCH.elapsed() - secondTimer > 1000 ){
            setSecondTimer();
            return true;
        }
        return false;
    }
    
}

function emo::onLoad() {
    emo.Stage().load(Main());
}
