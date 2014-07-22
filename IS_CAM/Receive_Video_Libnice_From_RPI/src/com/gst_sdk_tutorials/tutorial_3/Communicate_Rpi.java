package com.gst_sdk_tutorials.tutorial_3;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

public class Communicate_Rpi extends Activity {
	private native void nativeSendText(String string); 
	private native void nativeInitReciveText();
	
	@Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.comunicate_rpi);
        //nativeInitReciveText();
    }
	
	private void receive_message_from_rpi (final String message)
	{
		final TextView tv = (TextView) this.findViewById(R.id.textView2);
        runOnUiThread (new Runnable() {
          public void run() {
            tv.setText(message);
          }
        });
	}
	
	public void send_message_to_rpi(View v) 
	{
		EditText edit = (EditText) findViewById(R.id.editToRpi);
		nativeSendText(edit.getText().toString());
    }
	
	static {
        System.loadLibrary("gstreamer_android");
        System.loadLibrary("Main");
    }
}
