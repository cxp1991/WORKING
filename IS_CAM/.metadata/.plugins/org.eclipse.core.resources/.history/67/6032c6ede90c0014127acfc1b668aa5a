package com.gst_sdk_tutorials.tutorial_3;

import android.app.Activity;
import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.gstreamer.GStreamer;



public class Tutorial3 extends Activity implements SurfaceHolder.Callback {
    
	/** 
	 * Gstreamer 
	 */
	private native void nativeInit(String userName, String rpiName); // Initialize native code, build pipeline, etc
    private native void nativeFinalize(); // Destroy pipeline and shutdown native code
    private native void nativePlay(); // Set pipeline to PLAYING
    private native void nativePause(); // Set pipeline to PAUSED
    private static native boolean nativeClassInit(); // Initialize native class: cache Method IDs for callbacks
    private native void nativeSurfaceInit(Object surface);
    private native void nativeSurfaceFinalize();
    private native void nativeExitStreaming();
    
    private long native_custom_data; // Native code will use this to keep private data
    private long video_receive_native_custom_data;
    private long audio_send_native_custom_data;
    private boolean is_playing_desired; // Whether the user asked to go to PLAYING
    
    /** 
     * Setting Option 
     */
    private RelativeLayout settingLayout;
    private Button settingButton;
    
    /** 
     * Progress bar
     */
    private ProgressBar videoWaitingProgressbar;
    private Thread checkVideoAvailableThread;
    private boolean isVideoAvailable = false;
    
    /** 
     * Servo 
     */
    private float prevX, prevY, totalX = 0, totalY = 0, distance;
    private int rotateThreadHoldX, rotateThreadHoldY, angle;
    private final int ROTATE_RIGHT_TO_LEFT = 0x1;
    private final int ROTATE_LEFT_TO_RIGHT = 0x2;
    private final int ROTATE_TOP_TO_BOTTOM = 0x3;
    private final int ROTATE_BOTTOM_TO_TOP = 0x4;
    private final int ROTATE_X_AXIS 	   = 0x5;
    private final int ROTATE_Y_AXIS 	   = 0x6;
    private final int UNKNOWN_DIRECTION    = 0x7;
    private native void native_request_servo_rotate(int direction);
    private float x0 = 0, y0 = 0;
    private float x1 = 0, y1 = 0;
    private float dx, dy;
    private int count = 0;
    private int servoRotateDirection;
    private final int COUNT_TO_CALC_DIRECTION = 10;
    
    /** 
     * Temperature sensor 
     */
    private GetTemperature getTemperatureRunnable;
    private Thread getTemperatureThread;
    private boolean getTemperatureIsRunning = true;
    private native void native_get_temperature();
    
    /**
     *  Piezosiren
     */
    private final int PIEZO_OFF = 0x8;
    private final int PIEZO_ON  = 0x9;
    private int mPiezoStatus = PIEZO_OFF;
    private native void native_control_piezo(int status);
    
    /** 
     * Pumb controller
     */
    private final int PUMP_ON  = 0xA;
    private final int PUMP_OFF = 0xB;
    private int mPumpStatus = PUMP_OFF;
    private native void native_pump_controller (int status);
    
    /**
     * Back button
     */
    private ImageView mImvBack;
    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        Log.i("TAG", "Tutorial 3");
        /*
         * Set full screen, no title, alwayls On
         */
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        /*
         *  Initialize GStreamer and warn if it fails
         */
        try 
        {
            GStreamer.init(this);
        }
        catch (Exception e) 
        {
            Toast.makeText(this, e.getMessage(), Toast.LENGTH_LONG).show();
            finish();
            return;
        }

        setContentView(R.layout.main);

        /*
         * Get intent
         */
        Intent intent = getIntent();
        String rpiName = intent.getStringExtra("rpi_name");
        String userName = intent.getStringExtra("user_name");
        
        Log.i("", "username = " + userName + ", rpiname = " + rpiName);
        
        /* Setting Layout */
        settingLayout = (RelativeLayout) findViewById(R.id.settingLayout);
        settingLayout.setVisibility(View.GONE);
        settingButton = (Button) findViewById(R.id.settingbtn);
        settingButton.setVisibility(View.GONE);
        
        /* Video waiting progress bar */
        videoWaitingProgressbar = (ProgressBar) findViewById(R.id.progressBar1);
        Drawable drawableProgress = getResources().getDrawable(R.drawable.greenprogress);
        videoWaitingProgressbar.setProgressDrawable(drawableProgress);
        checkVideoAvailableThread = new Thread(CheckVideoAvailable);
        checkVideoAvailableThread.start();
        
        /* Back button*/
        mImvBack = (ImageView) findViewById(R.id.imv_back);
        mImvBack.setVisibility(View.GONE);
        mImvBack.setOnClickListener(onExitStreaming);
        
        /*
         *  Initialize video viewer 
         */
        SurfaceView sv = (SurfaceView) this.findViewById(R.id.surface_video);
        SurfaceHolder sh = sv.getHolder();
        sh.addCallback(this);
        sv.setOnTouchListener(SurfaceviewOnTouchListener);
        
        /*
         * Start thread to get temperature
         */
        getTemperatureRunnable = new GetTemperature();
        getTemperatureThread = new Thread(getTemperatureRunnable);
        getTemperatureThread.start();
        
        /* Get phone screen size */
        Display display = getWindowManager().getDefaultDisplay(); 
        int screenWidth = display.getWidth();  // deprecated
        int screenHeight = display.getHeight();  // deprecated
        Log.d ("TAG", "width = " + screenWidth + " height = " + screenHeight);
        
        /* Servo threadhold */
        rotateThreadHoldX = (screenWidth)/360;  
        rotateThreadHoldY = (screenHeight)*2/180;
        
        /*
         * Save playing state
         * onResume() will continue play
         */
        if (savedInstanceState != null) 
        {
            is_playing_desired = savedInstanceState.getBoolean("playing");
            Log.i ("GStreamer", "Activity created. Saved state is playing:" + is_playing_desired);
        } 
        else 
        {
            is_playing_desired = false;
            Log.i ("GStreamer", "Activity created. There is no saved state, playing: false");
        }

        /*
         * Initialize gstreamer application
         */
        nativeInit(userName, rpiName);
    }

    OnClickListener onExitStreaming = new OnClickListener() {
		
		@Override
		public void onClick(View v) {
			/*
    		 * Send signal disconnect to RPI
    		 */
			int count = 3;
			
			do {
				Log.e("", "Exit Streaming");
				
				try {
					Thread.sleep(10);
				} catch (InterruptedException e) {
				}
				
				nativeExitStreaming();
				count --;
				
			} while (count > 0);
			
			finish();
		}
	};
	
    /**
     * Control servo 
     */
    OnTouchListener SurfaceviewOnTouchListener = new OnTouchListener()
    {
		@Override
		public boolean onTouch(View v, MotionEvent event) 
		{
			switch (event.getAction()) 
	    	{
				case MotionEvent.ACTION_DOWN:
					/* Save started point*/
					//prevX = event.getX();
					//prevY = event.getY();
					x0 = event.getX();
					y0 = event.getY();
					break;
					
				case MotionEvent.ACTION_MOVE:
					if (count < COUNT_TO_CALC_DIRECTION)
					{
						count ++;
						break;
					}
					
					if (count == COUNT_TO_CALC_DIRECTION)
					{
						count++;
						x1 = event.getX();
						y1 = event.getY();
						
						/* Calc direction */
						dx = (float) Math.sqrt((x1 - x0)*(x1 - x0));
						dy = (float) Math.sqrt((y1 - y0)*(y1 - y0));
						
						Log.d("TAG", "dx = " + dx + ", dy = " + dy);
						
						/* x axis */
						if (dx >= 4*dy)
						{
							servoRotateDirection = ROTATE_X_AXIS;
						}
						/* y axis */
						else if(dy >= 4*dx)
						{
							servoRotateDirection = ROTATE_Y_AXIS;
						}
						else 
						{
							Log.d ("TAG", "Unknown direction!");
							servoRotateDirection = UNKNOWN_DIRECTION;
						}
						
						prevX = event.getX();
						prevY = event.getY();
						
						break;
					}
					
					float x,y;

					x = event.getX();
					y = event.getY();
					
					float dx = x - prevX;
					float dy = y - prevY;
					
					totalX += dx;
					totalY += dy;
										
					/* If distance lager than threadhold -> rotate servo */
					if ((Math.abs(totalX) > rotateThreadHoldX) 
							&& (servoRotateDirection == ROTATE_X_AXIS))
					{
						Log.d ("TAG", "totalX = " + totalX);
						
						/* Rotate 5 degree a time */
						if (totalX < 0)
							native_request_servo_rotate (ROTATE_RIGHT_TO_LEFT);
						else
							native_request_servo_rotate (ROTATE_LEFT_TO_RIGHT);

						totalX = 0;
					}
										
					if ((Math.abs(totalY) > rotateThreadHoldY) && (servoRotateDirection == ROTATE_Y_AXIS))
					{
						Log.d ("TAG", "totalY = " + totalY);
						
						/* Rotate 5 degree a time */
						if (totalY < 0)
							native_request_servo_rotate (ROTATE_BOTTOM_TO_TOP);
						else
							native_request_servo_rotate (ROTATE_TOP_TO_BOTTOM);

						totalY = 0;
					}
										
					prevX = x;
					prevY = y;
										
					break;
					
				case MotionEvent.ACTION_UP:
					count = 0;
					break;
				default:
					break;
			}
	    	
			return true;
		}
	};
    
	/**
	 *  Get temperature
	 */
	private class GetTemperature implements Runnable
	{
		private boolean isRunning = false;
		
		public GetTemperature() 
		{
			isRunning = true;
		}
		
		public void run() 
		{
			while(isRunning)
			{
				native_get_temperature();
				
				/*
				 *  Send command to get Temperature every 1s
				 */
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
		
		public void terminateThread()
		{
			isRunning = false;
		}
	}
	
	/** 
	 * Check video available 
	 * Then hide progressbar,
	 * Enable controlling menu
	 */
	Runnable CheckVideoAvailable  = new  Runnable() 
	{
		@Override
		public void run() 
		{
			long startTime = SystemClock.currentThreadTimeMillis();
			while(!isVideoAvailable);
			long stopTime = SystemClock.currentThreadTimeMillis();
			Log.d("TAG", "OK, Video available!, waiting = " + (stopTime - startTime)/1000 + "s");
			
			runOnUiThread(new Runnable()
			{
				public void run() 
				{
					videoWaitingProgressbar.setVisibility(View.GONE);
					settingButton.setVisibility(View.VISIBLE);
					mImvBack.setVisibility(View.VISIBLE);
				}
			});
		}
		
	};
	
	/**
	 *  Display Temperature 
	 */
	 private void setMessage(final String message) 
	 {
	        final TextView tv = (TextView) this.findViewById(R.id.textview_message);
	        runOnUiThread (new Runnable() 
	        {
				public void run() 
				{
					tv.setText("Temperature = "  + message + " oC");
				}
	        });
	 }

	 public void SettingOnclickListener(View view)
	 {
		 if (settingLayout.getVisibility() == View.GONE)
		 {
			 settingLayout.setVisibility(View.VISIBLE);
		 }
		 else
		 {
			 settingLayout.setVisibility(View.GONE);
		 }
	 }
	 
	 /**
	  *  Control piezosiren
	  */
	 public void PiezoOnClick (View view)
	 {
		 if (mPiezoStatus == PIEZO_OFF)
		 {
		 	native_control_piezo(PIEZO_ON);
		 	mPiezoStatus = PIEZO_ON;
		 }
		 else if (mPiezoStatus == PIEZO_ON)
		 {
			 native_control_piezo(PIEZO_OFF);
			 mPiezoStatus = PIEZO_OFF;
		 }
	 }
	 
	 /**
	  *  Pump controller 
	  */
	 public void PumpOnclickListener(View view)
	 {

		 if (mPumpStatus == PUMP_OFF)
		 {
		 	native_pump_controller(PUMP_ON);
		 	mPumpStatus = PUMP_ON;
		 }
		 else if (mPumpStatus == PUMP_ON)
		 {
			 native_pump_controller(PUMP_OFF);
			 mPumpStatus = PUMP_OFF;
		 }
	 }
	 
	@Override
	protected void onStop() 
	{
   		super.onStop();
   		
   		/* 
   		 * Stop get temperature thread 
   		 */
		 if (getTemperatureThread != null) 
		 {
			 getTemperatureRunnable.terminateThread();
		     
			 try 
			 {
				 getTemperatureThread.join();
			 } 
			 catch (Exception e) 
			 {
				 e.printStackTrace();
			 }
			 
			 Log.d("TAG", "Stop get temperature!");
		 }
   	}
    
    protected void onDestroy() 
    {
        nativeFinalize();
        super.onDestroy();
    }

    protected void onSaveInstanceState (Bundle outState) 
    {
        Log.d ("GStreamer", "Saving state, playing: " + is_playing_desired);
        outState.putBoolean("playing", is_playing_desired);
    }
    
    // Called from native code. Native code calls this once it has created its pipeline and
    // the main loop is running, so it is ready to accept commands.
    private void onGStreamerInitialized () 
    {
        Log.i ("GStreamer", "Gst initialized. Restoring state, playing:" + is_playing_desired);
        nativePlay();
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int width,
            int height) 
    {
        Log.d("GStreamer", "Surface changed to format " + format + " width "
                + width + " height " + height);
        nativeSurfaceInit (holder.getSurface());
    }

    public void surfaceCreated(SurfaceHolder holder) 
    {
        Log.d("GStreamer", "Surface created: " + holder.getSurface());
    }

    public void surfaceDestroyed(SurfaceHolder holder)
    {
        Log.d("GStreamer", "Surface destroyed");
        nativeSurfaceFinalize ();
    }
    
    static 
    {
        System.loadLibrary("gstreamer_android");
        System.loadLibrary("Main");
        nativeClassInit();
    }
    
    /**
     * Disable backpress until 
     * connection is OK
     */
    
    @Override
    public void onBackPressed() {
    	/**
    	 * Disable this function
    	 */
    }
}