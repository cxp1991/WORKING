package com.gst_sdk_tutorials.tutorial_3;

import java.util.ArrayList;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;



public class ClientAdapter extends BaseExpandableListAdapter {
	private ArrayList<Client> mParent;
	private Context mContext;
	private String mUserName;
	
	public ClientAdapter(Context context, ArrayList<Client> mParent, String userName) {
		this.mContext = context;
		this.mParent  = mParent;
		this.mUserName = userName;
	}
	
	@Override
	public Object getChild(int groupPosition, int childPosition) {
		return mParent.get(groupPosition).getmClientDetailInfo().get(childPosition);
	}

	@Override
	public long getChildId(int groupPosition, int childPosition) {
		return childPosition;
	}

	class ChildHolder
	{
		TextView tv1;
		TextView tv2;
		TextView tv3;
		
		public ChildHolder(View view)
		{
			tv1 = (TextView) view.findViewById(R.id.tv01);
			tv2 = (TextView) view.findViewById(R.id.tv02);
			tv3 = (TextView) view.findViewById(R.id.tv03);
		}
	}
	
	@Override
	public View getChildView(int groupPosition, int childPosition, boolean isLastChild, 
			View convertView, ViewGroup parent) {
		
		ChildHolder holder;
		ClientDetailInfo child = mParent.get(groupPosition).getmClientDetailInfo().get(childPosition);
		
		if (convertView == null) {
			 LayoutInflater li = (LayoutInflater) mContext 
                    .getSystemService(Context.LAYOUT_INFLATER_SERVICE); 
			 convertView = li.inflate(R.layout.state_item_layout, null);  
			 holder = new ChildHolder (convertView);  
            convertView.setTag(holder); 
		} else {
			 holder = (ChildHolder) convertView.getTag();  
		}
		
		holder.tv1.setText(child.getmTemperature() + "");
		holder.tv2.setText(child.getmLocation() + "");
		holder.tv3.setText(child.getmTimeOnline() + "");
		
		return convertView;
	}

	@Override
	public int getChildrenCount(int groupPosition) {
		return mParent.get(groupPosition).getmClientDetailInfo().size();
	}

	@Override
	public Object getGroup(int groupPosition) {
		return mParent.get(groupPosition);
	}

	@Override
	public int getGroupCount() {
		return mParent.size();
	}

	@Override
	public long getGroupId(int groupPosition) {
		return groupPosition;
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
					Intent intent = new Intent(mContext, Tutorial3.class);
					intent.putExtra("rpi_name", mClientName.getText().toString());
					intent.putExtra("user_name", mUserName);
					((Activity)mContext).startActivityForResult(intent, 0);
					Log.i("", "User name = " + mUserName);
					Log.i("", "Rpi name = " + mClientName.getText().toString());
				}
			});
		}
	}
	
	@Override
	public View getGroupView(int groupPosition, boolean isExpanded,
			View convertView, ViewGroup parent) {
		ViewHolder holder;
		if (convertView == null) {
			 LayoutInflater li = (LayoutInflater) mContext 
                     .getSystemService(Context.LAYOUT_INFLATER_SERVICE); 
			 convertView = li.inflate(R.layout.listview_item, null);  
			 holder = new ViewHolder(convertView);  
             convertView.setTag(holder); 
		} else {
			 holder = (ViewHolder) convertView.getTag();  
		}
		
		/*
		 * Set state icon
		 */
		if (mParent.get(groupPosition).getmClientState() == Client.ONLINE)
			holder.mStateIcon.setImageResource(R.drawable.online);
		else
			holder.mStateIcon.setImageResource(R.drawable.offline);
		
		/*
		 * Set client's name
		 */
		holder.mClientName.setText(mParent.get(groupPosition).getmClientName());
		
		/*
		 * Set button
		 */
		if (mParent.get(groupPosition).getmClientState() == Client.OFFLINE)
			holder.mConnectTo.setEnabled(false);
		else
			holder.mConnectTo.setEnabled(true);
		
		return convertView;
	}

	@Override
	public boolean hasStableIds() {
		return true;
	}

	@Override
	public boolean isChildSelectable(int arg0, int arg1) {
		return true;
	}

	public void setData(ArrayList<Client> mParent2) {
		mParent = mParent2;
		
		for (Client client : mParent) {
			client.getmClientDetailInfo().add(new ClientDetailInfo(10, 20, 30));
		}
		
		((Activity) mContext).runOnUiThread(new Runnable() {
			
			@Override
			public void run() {
				notifyDataSetChanged();
			}
		});
	}
	
}
