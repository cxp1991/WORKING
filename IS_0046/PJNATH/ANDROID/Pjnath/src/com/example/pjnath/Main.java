package com.example.pjnath;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class Main extends Activity {

	private native void nativePjnathSetup ();
	private String TAG = "[JAVA] Main";
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        Log.i(TAG, " ");
        nativePjnathSetup();
    }
    
    static {
    	System.loadLibrary("icedemo");
    }
}
