package com.gst_sdk_tutorials.tutorial_3;

import java.util.ArrayList;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

public class ClientStateAdapter extends BaseAdapter{

	private ArrayList<ClientInfo> mClientArralist;
	
	private String mUserName;
	
	private Activity mActivity;
	
	public ClientStateAdapter(Activity activity, ArrayList<ClientInfo> clientArraylist,
			String userName) {
		this.mActivity = activity;
		this.mClientArralist = clientArraylist;
		this.mUserName = userName;
	}

	@Override
	public int getCount() {
		return mClientArralist.size();
	}

	@Override
	public Object getItem(int position) {
		return mClientArralist.get(position);
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	/**
	 * Set adapter's data
	 */
	public void setData(Activity activity, ArrayList<ClientInfo> arrayList) {
		this.mClientArralist = arrayList;
		
		activity.runOnUiThread(new Runnable() {
			
			@Override
			public void run() {
				notifyDataSetChanged();
			}
		});
		
	}
	
	class ViewHolder
	{
		ImageView mStateIcon;
		TextView mClientName;
		Button mConnectTo;
		
		public ViewHolder(View view)
		{
			mStateIcon  = (ImageView) view.findViewById(R.id.imgView_client_status);
			mClientName = (TextView) view.findViewById(R.id.tv_client_name);
			mConnectTo  = (Button) view.findViewById(R.id.button_connect_to);
			
			/*
			 * Implement client listener 
			 */
			mConnectTo.setOnClickListener(new OnClickListener() {
				
				@Override
				public void onClick(View view) {
					/*
					 * Start Tutorial3 activity
					 */
					Intent intent = new Intent(mActivity, Tutorial3.class);
					intent.putExtra("rpi_name", mClientName.getText().toString());
					intent.putExtra("user_name", mUserName);
					mActivity.startActivity(intent);
				}
			});
		}
	}
	
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		
		ViewHolder holder;
		
		if (convertView == null) 
        {  
             LayoutInflater li = (LayoutInflater) mActivity  
                       .getSystemService(Context.LAYOUT_INFLATER_SERVICE);  
             convertView = li.inflate(R.layout.listview_item, null);  

             holder = new ViewHolder(convertView);  
             convertView.setTag(holder);  
        }
        else 
        {  
             holder = (ViewHolder) convertView.getTag();  
        }  
		
		/*
		 * Set state icon
		 */
		if (mClientArralist.get(position).getmClientState() == ClientInfo.ONLINE)
			holder.mStateIcon.setImageResource(R.drawable.online);
		else
			holder.mStateIcon.setImageResource(R.drawable.offline);
		
		/*
		 * Set client's name
		 */
		holder.mClientName.setText(mClientArralist.get(position).getmClientName());
		
		/*
		 * Set button
		 */
		if (mClientArralist.get(position).getmClientState() == ClientInfo.OFFLINE)
			holder.mConnectTo.setEnabled(false);
		else
			holder.mConnectTo.setEnabled(true);
		
		return convertView;
	}

}
