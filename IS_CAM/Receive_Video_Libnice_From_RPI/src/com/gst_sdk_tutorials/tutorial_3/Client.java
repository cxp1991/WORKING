package com.gst_sdk_tutorials.tutorial_3;

import java.util.ArrayList;

public class Client {

	private String mClientName;
	private int mClientState;
	public final static int ONLINE = 0x01;
	public final static int OFFLINE = 0x02;
	private ArrayList<ClientDetailInfo> mClientDetailInfo = new ArrayList<ClientDetailInfo>();

	public Client(String mClientName, int mClientState) {
		super();
		this.mClientName = mClientName;
		this.mClientState = mClientState;
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

	public ArrayList<ClientDetailInfo> getmClientDetailInfo() {
		return mClientDetailInfo;
	}

	public void setmClientDetailInfo(ArrayList<ClientDetailInfo> mClientDetailInfo) {
		this.mClientDetailInfo = mClientDetailInfo;
	}
	
}
