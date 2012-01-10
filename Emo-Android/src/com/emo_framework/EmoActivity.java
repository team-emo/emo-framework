package com.emo_framework;

import android.app.NativeActivity;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.FontMetrics;
import android.graphics.Paint.Style;
import android.graphics.Typeface;
import android.os.AsyncTask;
import android.os.Vibrator;
import android.util.Log;
import android.view.Surface;
import android.widget.Toast;

import java.util.List;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Random;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.IOException;

import org.apache.http.NameValuePair;
import org.apache.http.StatusLine;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.HttpParams;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.HttpResponse;

public class EmoActivity extends NativeActivity {

    public static final String ECHO  = "ECHO";
    public static final String HTTP_ERROR = "HTTP_ERROR";
    
    public static final String ENGINE_TAG = "EmoFramework";

    protected String lastErrorMessage;
    protected String lastErrorCode;

    private static Random randomNumberGenerator;
    
    private static synchronized Random initRNG() {
        Random rnd = randomNumberGenerator;
        return (rnd == null) ? (randomNumberGenerator = new Random()) : rnd;
    }
    
    public String getDefaultLocale() {
    	return Locale.getDefault().toString();
    }

    public byte[] loadTextBitmap(String name, int fontSize,
    		String fontFace, boolean isBold, boolean isItalic,
    		String param1, String param2, String param3,
    		String param4, String param5, String param6) {
    	// extract property name
    	String targetName = name.substring(name.indexOf("::") + 2);
    	int targetId = getResources().getIdentifier(targetName, "string", getPackageName());

    	String targetValue = " ";
    	if (targetId != 0) {
        	targetValue = getResources().getString(targetId);
    	}
    	
    	// target value accepts 6 parameters at most
    	targetValue = String.format(targetValue,
    			param1, param2, param3, param4, param5, param6);
    	
    	Paint forePaint = new Paint();
    	Paint backPaint = new Paint();
    	
    	if (fontFace.length() == 0) {
    		if (isBold && isItalic) {
    			forePaint.setTypeface(Typeface.create(Typeface.DEFAULT, Typeface.BOLD_ITALIC));
    		} else if (isBold) {
    			forePaint.setTypeface(Typeface.DEFAULT_BOLD);
    		}
    	} else {
    		Typeface typeface = Typeface.DEFAULT;
    		String lowerCase = fontFace.toLowerCase();
    		
    		if (lowerCase.equals("serif")) {
    			typeface = Typeface.SERIF;
    		} else if (lowerCase.equals("sans_serif")) {
    			typeface = Typeface.SANS_SERIF;
    		} else if (lowerCase.equals("monospace")) {
    			typeface = Typeface.MONOSPACE;
    		} else if (fontFace.contains(".")) {
        		typeface = Typeface.createFromAsset(getAssets(), fontFace);
    		} else {
        		typeface = Typeface.createFromAsset(getAssets(), fontFace + ".ttf");
    		}
    		
    		if (isBold && isItalic) {
    			forePaint.setTypeface(Typeface.create(typeface, Typeface.BOLD_ITALIC));
    		} else if (isBold) {
    			forePaint.setTypeface(Typeface.create(typeface, Typeface.BOLD));
    		} else {
    			forePaint.setTypeface(typeface);
    		}
    	}
    	
    	forePaint.setColor(Color.WHITE);
    	if (fontSize > 0) forePaint.setTextSize(fontSize);
    	forePaint.setAntiAlias(true);
    	
    	backPaint.setColor(Color.TRANSPARENT);
    	backPaint.setStyle(Style.FILL);

    	FontMetrics metrics = forePaint.getFontMetrics();
    	int width  = (int)Math.ceil(forePaint.measureText(targetValue));
    	int height = (int)Math.ceil(Math.abs(metrics.ascent) + 
    			Math.abs(metrics.descent) + Math.abs(metrics.leading));
    	
    	Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
    	Canvas canvas = new Canvas(bitmap);
    	canvas.drawRect(0, 0, width, height, backPaint);
    	canvas.drawText(targetValue, 0, height - metrics.descent , forePaint);

    	ByteArrayOutputStream os = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.PNG, 100, os);
        return os.toByteArray();
    }

    /*
     * Returns a pseudorandom, uniformly distributed int value between 0 (inclusive) 
     * and the specified max value (exclusive), drawn from this random number generator's sequence.
     */
    public int random(int max) {
        Random rnd = randomNumberGenerator;
        if (rnd == null) rnd = initRNG();
        return rnd.nextInt(max);
    }
    
    public String echo(String str) {
        callback(ECHO, str, "", "");
        return str;
    }
    
    public String getLastErrorMessage() {
    	return lastErrorMessage;
    }
    
    public String getLastErrorCode() {
    	return lastErrorCode;
    }
    
    public void vibrate() {
    	Vibrator vibrator = (Vibrator)getSystemService(VIBRATOR_SERVICE);
        if (vibrator != null) vibrator.vibrate(400);
    }

    public String getDataFilePath(String name) {
        return getFilesDir() + File.separator + name;
    }
    
    public void toast(final String text, final int duration) {
		runOnUiThread(new Runnable() {
			public void run() {
				Toast.makeText(EmoActivity.this, text, duration).show();
			}
		});
    }

    public void asyncHttpRequest(final String name, final int timeout, final String... params) {
		runOnUiThread(new Runnable() {
			public void run() {
		        NetworkRequestTask aTask = new NetworkRequestTask(name, timeout);
		        aTask.execute(params);
			}
		});
    }

    class NetworkRequestTask extends AsyncTask<String, Void, String> {
        String name;
        int    timeout;
        NetworkRequestTask(String name, int timeout) {
            this.name = name;
            this.timeout = timeout;
        }
        protected String doInBackground(String... params) {
            if (params.length == 0) return "";

            String url    = params[0];
            String method = "GET";
            
            if (params.length > 1) method = params[1];

            if (method.toUpperCase().equals("GET")) {
            	if (params.length > 2) {
                	StringBuffer data = new StringBuffer();
            		for (int i = 2; i < params.length; i++) {
            			data.append(params[i]);
            			if (i % 2 == 0) {
            				data.append("=");
            			} else if (i < params.length -1){
            				data.append("&");
            			}
            		}
            		url = url + "?" + data.toString();
            	}

            	return httpRequestByGET(url, timeout);
            } else if (method.toUpperCase().equals("POST")) {
        		List<NameValuePair> data = new ArrayList<NameValuePair>();
            	if (params.length > 2) {
            		String key = "";
            		for (int i = 2; i < params.length; i++) {
            			if (i % 2 == 0) {
            				key = params[i];
            			} else if (i < params.length -1){
            				NameValuePair pair = new BasicNameValuePair(key, params[i]);
            				data.add(pair);
            				key = "";
            			}
            		}
            	}
            	return httpRequestByPOST(url, timeout, data);
            }
            return "";
        }
        protected void onPostExecute(String response) {
        	if (response == null) {
        		callback(HTTP_ERROR, name, lastErrorCode, lastErrorMessage);
        	} else {
        		callback(name, response, "", "");
        	}
        }
    }
    
    public String httpRequestByGET(String url, int timeout) {
        String response = "";
        try {
            HttpClient httpClient = new DefaultHttpClient();
            HttpParams params     = httpClient.getParams();
            HttpConnectionParams.setConnectionTimeout(params, timeout);
            HttpConnectionParams.setSoTimeout(params, timeout);
            HttpResponse serverResponse = httpClient.execute(new HttpGet(url));
            StatusLine statusLine = serverResponse.getStatusLine();
            if (statusLine.getStatusCode() < 400){
                InputStream inputStream = serverResponse.getEntity().getContent();
                InputStreamReader inputReader = new InputStreamReader(inputStream);
                BufferedReader bufferedReader = new BufferedReader(inputReader);
                StringBuffer buffer = new StringBuffer();
                String line;
                while((line = bufferedReader.readLine()) != null){
                    buffer.append(line);
                }
                response = buffer.toString();
                inputStream.close();
            } else {
            	this.lastErrorCode    = String.valueOf(statusLine.getStatusCode());
            	this.lastErrorMessage = statusLine.getReasonPhrase();
            	return null;
            }
        } catch (IOException e) {
        	this.lastErrorCode    = "-1";
        	this.lastErrorMessage = e.getLocalizedMessage();
            return null;
        }
        return response;
    }

    public String httpRequestByPOST(String url, int timeout, List<NameValuePair> params) {
        String response = "";
        try {
            HttpClient httpClient = new DefaultHttpClient();
        	HttpPost httpPost = new HttpPost(url);
            httpPost.setEntity(new UrlEncodedFormEntity(params, "UTF-8"));
            HttpResponse serverResponse = httpClient.execute(httpPost);
            StatusLine statusLine = serverResponse.getStatusLine();
            if (statusLine.getStatusCode() < 400){
                InputStream inputStream = serverResponse.getEntity().getContent();
                InputStreamReader inputReader = new InputStreamReader(inputStream);
                BufferedReader bufferedReader = new BufferedReader(inputReader);
                StringBuffer buffer = new StringBuffer();
                String line;
                while((line = bufferedReader.readLine()) != null){
                	buffer.append(line);
                }
                response = buffer.toString();
                inputStream.close();
            } else {
            	this.lastErrorCode    = String.valueOf(statusLine.getStatusCode());
            	this.lastErrorMessage = statusLine.getReasonPhrase();
            	return null;
            }
        	
        } catch (IOException e) {
        	this.lastErrorCode    = "-1";
        	this.lastErrorMessage = e.getLocalizedMessage();
        	return null;
        }
    	return response;
    }
    
    public String getRuntimeScriptName() {
    	try {
    	    ActivityInfo ai = getPackageManager().
    	    	getActivityInfo(this.getComponentName(), PackageManager.GET_META_DATA);
    	    return ai.metaData.getString("emo.script.runtime");
    	} catch (Exception e) {
    		return null;
    	}
    }

    public String getMainScriptName() {
    	try {
    	    ActivityInfo ai = getPackageManager().
    	    	getActivityInfo(this.getComponentName(), PackageManager.GET_META_DATA);
    	    return ai.metaData.getString("emo.script.main");
    	} catch (Exception e) {
    		return null;
    	}
    }

    public void LOGI(String message) {
    	Log.i(ENGINE_TAG, message, null);
    }

    public void LOGE(String message) {
    	Log.e(ENGINE_TAG, message, null);
    }

    public void LOGW(String message) {
    	Log.w(ENGINE_TAG, message, null);
    }

    public void setOrientationLandscape() {
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    }
    
    public void setOrientationPortrait() {
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
    }
    
    public String getDeviceName() {
    	return android.os.Build.DEVICE;
    }
    
    public boolean isSimulator() {
    	return "sdk".equals(android.os.Build.MODEL) && "generic".equals(android.os.Build.DEVICE);
    }
    
    public int getSDKVersion() {
    	return android.os.Build.VERSION.SDK_INT;
    }
    
    public String getOSVersion() {
    	return android.os.Build.VERSION.RELEASE;
    }

    public int getRotation() {                           
        int orientation = getWindowManager().getDefaultDisplay().getOrientation();
        switch (orientation) {
        case Surface.ROTATION_0:
            return 0;
        case Surface.ROTATION_90:
            return 90;
        case Surface.ROTATION_180:
            return 180;
        case Surface.ROTATION_270:
            return 270;
        default:
            return 0;
        }
    }
    
    public int getNativeOrientation(int width, int height) {
    	int rotation = getRotation();
        if(((rotation == 0 || rotation == 180) && (width >= height)) ||
        		((rotation == 90 || rotation == 270) && (width <= height))) {
            return ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
        } else {
            return ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
        }               
    }
    
    public native void callback(String name, String value, String errCode, String errMsg);
}
