#include "gstreamer_utils.h"

/* Listen for element's state change */
void on_state_changed (GstBus *bus, GstMessage *msg, CustomData *data)
{
	char *TAG = "[Jni] on_state_changed";

	GstState old_state, new_state, pending_state;
	gst_message_parse_state_changed (msg, &old_state,
									 &new_state,
									 &pending_state);

	if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->pipeline))
	{
		 gchar *message = g_strdup_printf("%s changed to state %s",
						 GST_MESSAGE_SRC_NAME(msg),
						 gst_element_state_get_name(new_state));

		__android_log_print (ANDROID_LOG_ERROR, TAG,
				"[receive video]%s\n", message);
		g_free (message);
	}

	/* Video is ready to play, so display it on surfaceview */
	if (data->pipeline->current_state == GST_STATE_PLAYING)
	{
		__android_log_print (ANDROID_LOG_DEBUG, TAG,
				"Video is ready!");
		JNIEnv *env = get_jni_env ();
		jclass cls = (*env)->GetObjectClass(env, data->app);
		jfieldID video_available_field_id = (*env)->GetFieldID (env, cls,
													"isVideoAvailable", "Z");
		(*env)->SetBooleanField(env, data->app,
				video_available_field_id, JNI_TRUE);
	}
}

void
on_error (GstBus     *bus,
          GstMessage *message,
          gpointer    user_data)
{
	  char *TAG = "[Jni] on_error";

	  GError *err;
	  gchar *debug_info;
	  gchar *message_string;

	  gst_message_parse_error (message, &err, &debug_info);
	  __android_log_print (ANDROID_LOG_INFO, TAG,
			  "=========================================\n");
	  message_string = g_strdup_printf ("Error received from element %s: %s",
			  GST_OBJECT_NAME (message->src), err->message);
	  __android_log_print (ANDROID_LOG_INFO, TAG,
			  "debug_info = %s \n\n message_string = %s\n", debug_info, message_string);
	  __android_log_print (ANDROID_LOG_INFO, TAG,
			  "=========================================\n");
	  g_clear_error (&err);
	  g_free (debug_info);
	  g_free (message_string);
}

void on_pad_added (GstElement* object, GstPad* pad, gpointer data)
{
	char *TAG = "[Jni] on_pad_added";
	gchar *pad_name = gst_pad_get_name(pad);
	__android_log_print (ANDROID_LOG_DEBUG, TAG,
			"on_pad_added = %s", pad_name);
	GstPad *sinkpad;
	GstElement *autovideosink = (GstElement *) data;
	sinkpad = gst_element_get_static_pad (autovideosink, "sink");
	gst_pad_link (pad, sinkpad);
	gst_object_unref (sinkpad);
}
