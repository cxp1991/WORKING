package com.gst_sdk_tutorials.tutorial_3;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

/**
 * Login to server
 * If login is successful then list rpis is online,
 * and save user&password
 */
public class Login extends Activity 
{
	private EditText mUsername, mPassword;
	
	private Button mLoginButton;
	
	private final int LOGIN_WRONG_USERINPUT = 0x00;
	
	private final int LOGIN_SERVER_UNREACHABLE = 0x01;

	private final int LOGIN_SUCCESSED = 0x02;
	
	private native int nativeLogin (String mUsername, String mPassword);
	
	private DialogFragment dialog;
	
	private TextView mTvRegister;
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.login_layout);
		
		mUsername = (EditText)  findViewById(R.id.editUserName);
		mPassword = (EditText)  findViewById(R.id.editPassword);
		mLoginButton = (Button) findViewById(R.id.buttonLogin);
		
		/*
		 * Get old nearest correct username, password
		 */
		SharedPreferences sharedPref = getPreferences(Context.MODE_PRIVATE);
		mUsername.setText(sharedPref.getString("USERNAME", null));
		mPassword.setText(sharedPref.getString("PASSWORD", null));
		
		/*
		 * Register new user
		 */
		mTvRegister = (TextView) findViewById(R.id.tv_register);
		mTvRegister.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				Intent intent = new Intent (getBaseContext(), SignUp.class);
				startActivity(intent);
			}
		});
	}
	
	
	public void login(View v)
	{
		int ret = nativeLogin(mUsername.getText().toString(), mPassword.getText().toString());
		
		switch (ret) {
		
		case LOGIN_SUCCESSED:
			
			/* 
			 * Save username & password for suggesstion later
			 */
			SharedPreferences sharedPref = getPreferences(Context.MODE_PRIVATE);
			SharedPreferences.Editor editor = sharedPref.edit();
			editor.putString("USERNAME", mUsername.getText().toString());
			editor.putString("PASSWORD", mPassword.getText().toString());
			editor.commit();
			
			/*
			 * Call ClientState Activity enclose data
			 */
			Intent intent = new Intent(this, ClientState.class);
			intent.putExtra("username", mUsername.getText().toString());
			startActivity(intent);
			break;
			
		case LOGIN_WRONG_USERINPUT:
			
			/*
			 * Couldn't connect to server because wrong input or 
			 * server failed.
			 */
			dialog = LoginFailedDialogFragment.newInstance("Wrong UserName or Password.");
			dialog.show(getFragmentManager(), null);
			
			break;
			
		case LOGIN_SERVER_UNREACHABLE:
			
			/*
			 * Server was unreachable
			 */
			dialog = LoginFailedDialogFragment.newInstance("Server was unreachable");
			dialog.show(getFragmentManager(), null);
			break;
			
		default:
			break;
		}
	}	
	
	/**
	 * Dialog show when login failed
	 */
	public static class LoginFailedDialogFragment extends DialogFragment {
		
		private String mMessage = null;
		
		public static LoginFailedDialogFragment newInstance(String message){
			LoginFailedDialogFragment f = new LoginFailedDialogFragment();

            Bundle args = new Bundle();
            args.putString("MESSAGE", message);
            f.setArguments(args);

            return f;
		}
	
		
	    @Override
	    public Dialog onCreateDialog(Bundle savedInstanceState) 
	    {
	    	mMessage = getArguments().getString("MESSAGE");
	    	
	        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
	        builder.setMessage(mMessage);
	        builder.setTitle("Error");
	        builder.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
				
				@Override
				public void onClick(DialogInterface dialog, int which) {
					dismiss();
				}
			});
	               
	        return builder.create();
	    }
	}
	
	static 
	{
        System.loadLibrary("gstreamer_android");
        System.loadLibrary("Main");
    }
	
}
