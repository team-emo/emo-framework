package com.emo_framework;

import android.app.NativeActivity;

public class EmoActivity extends NativeActivity {

    public String echo(String str) {
        callback("echo", str);
        return str;
    }

    public native void callback(String name, String value);
}
