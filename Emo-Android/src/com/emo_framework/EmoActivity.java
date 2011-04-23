package com.emo_framework;

import android.app.NativeActivity;

public class EmoActivity extends NativeActivity {
	/*
	 * echo method for jni testing.
	 */
	public String echo(String str) {
		return str;
	}
}
