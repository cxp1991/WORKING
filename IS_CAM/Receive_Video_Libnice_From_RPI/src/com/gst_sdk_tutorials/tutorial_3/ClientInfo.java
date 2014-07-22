package com.gst_sdk_tutorials.tutorial_3;

public class ClientInfo {

	private String mClientName;
	
	private int mClientState;
	
	public final static int ONLINE = 0x01;
	
	public final static int OFFLINE = 0x02;
	
	public ClientInfo( String username, int state) 
	{
		this.mClientName = username;
		this.mClientState = state;
	}

	/**
	 * @return the mClientName
	 */
	public String getmClientName() {
		return mClientName;
	}

	/**
	 * @return the mClientState
	 */
	public int getmClientState() {
		return mClientState;
	}

	/**
	 * @param mClientName the mClientName to set
	 */
	public void setmClientName(String mClientName) {
		this.mClientName = mClientName;
	}

	/**
	 * @param mClientState the mClientState to set
	 */
	public void setmClientState(int mClientState) {
		this.mClientState = mClientState;
	}

	
}
