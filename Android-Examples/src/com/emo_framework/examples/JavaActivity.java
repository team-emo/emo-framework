package com.emo_framework.examples;

import java.util.ArrayList;
import java.util.Arrays;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Button;

import com.emo_framework.examples.R.id;

public class JavaActivity extends Activity {
    protected WebView webView;
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.java);
        webView = (WebView) findViewById(R.id.webview);
        webView.setWebViewClient( new WebViewClient() );
        
        Button webButton = (Button) findViewById(id.web_button);
        webButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                webView.loadUrl("http://www.google.com");
            }
        });
        
        Button aButton = (Button) findViewById(id.a_button);
        aButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent intent = new Intent();
                
                // put string extra
                intent.putExtra("string", "YOU HAVE PRESSED A");
                
                // put boolean extra
                intent.putExtra("aButtonFlag", true);
                
                // put int array extra
                Integer[] intArray = { 1, 555, 88888, -258 };
                ArrayList<Integer> intList = new ArrayList<Integer>(Arrays.asList(intArray));
                intent.putIntegerArrayListExtra("intArray", intList);
                
                 // set result and finish
                setResult(RESULT_OK, intent);
                JavaActivity.this.finish();
            }
        });
        
        Button bButton = (Button) findViewById(id.b_button);
        bButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent intent = new Intent();
                
                // put string extra
                intent.putExtra("string", "YOU HAVE PRESSED B");
                
                // put boolean extra
                intent.putExtra("aButtonFlag", false);
                
                 // put string array extra
                String[] stringArray = { "This", "is", "from", "Java" };
                ArrayList<String> stringList = new ArrayList<String>(Arrays.asList(stringArray));
                intent.putStringArrayListExtra("stringArray", stringList);
                
                // set result and finish
                setResult(RESULT_OK, intent);
                JavaActivity.this.finish();
            }
        });
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
