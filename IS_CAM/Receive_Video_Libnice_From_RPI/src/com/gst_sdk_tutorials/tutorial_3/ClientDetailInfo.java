package com.gst_sdk_tutorials.tutorial_3;

public class ClientDetailInfo {
	private int mTemperature;
	private int mTimeOnline;
	private int mLocation;
	
	public ClientDetailInfo(int mTemperature, int mTimeOnline, int mLocation) {
		super();
		this.mTemperature = mTemperature;
		this.mTimeOnline = mTimeOnline;
		this.mLocation = mLocation;
	}
	
	/**
	 * @return the mTemperature
	 */
	public int getmTemperature() {
		return mTemperature;
	}
	
	/**
	 * @return the mTimeOnline
	 */
	public int getmTimeOnline() {
		return mTimeOnline;
	}
	
	/**
	 * @return the mLocation
	 */
	public int getmLocation() {
		return mLocation;
	}
	
	/**
	 * @param mTemperature the mTemperature to set
	 */
	public void setmTemperature(int mTemperature) {
		this.mTemperature = mTemperature;
	}
	
	/**
	 * @param mTimeOnline the mTimeOnline to set
	 */
	public void setmTimeOnline(int mTimeOnline) {
		this.mTimeOnline = mTimeOnline;
	}
	
	/**
	 * @param mLocation the mLocation to set
	 */
	public void setmLocation(int mLocation) {
		this.mLocation = mLocation;
	}
	
	

}
