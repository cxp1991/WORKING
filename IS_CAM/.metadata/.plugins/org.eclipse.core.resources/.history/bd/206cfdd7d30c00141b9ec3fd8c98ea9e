#include "stream.h"
#include "../Login/login.h"

void*  _send_audio_main(CustomData *data)
{

	data->agent = libnice_create_NiceAgent_with_gstreamer ( send_audio_gathering_done,
															data->context);
	// Create a new stream with one component
	data->stream_id = libnice_create_stream_id (data->agent);

	while((*receive_audio_gathering_done) == FALSE)
			usleep(100);

	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "start send audio");

	/* Init Gstreamer */
	_send_audio_init_gstreamer(data->agent, data->stream_id, data);

	// Set receiver function
	set_receiver (data->agent, data->stream_id, data->context);

	// Start gathering candidates
	libnice_start_gather_candidate (data->agent,
									data->stream_id,
									data->context);
}

static void
on_error (GstBus     *bus,
          GstMessage *message,
          gpointer    user_data)
{
	  GError *err;
	  gchar *debug_info;
	  gchar *message_string;

	  gst_message_parse_error (message, &err, &debug_info);
	  message_string = g_strdup_printf ("Error received from element %s: %s", GST_OBJECT_NAME (message->src), err->message);

	  __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "=========================================\n");
	  __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "debug_info = %s \n\n message_string = %s\n", debug_info, message_string);
	  g_clear_error (&err);
	  g_free (debug_info);
	  g_free (message_string);
}

void  _send_audio_init_gstreamer(NiceAgent *magent, guint stream_id, CustomData *data)
{
	GstElement *pipeline, *audiotestsrc, *openslessrc, *audioconvert, *caps, *rtpL16pay, *nicesink;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;
	GSource *bus_source;

	/* Initialize GStreamer */
  	gst_init (NULL, NULL);

  	//Register gstreamer plugin libnice
	gst_plugin_register_static (
		GST_VERSION_MAJOR,
		GST_VERSION_MINOR,
		"nice",
		"Interactive UDP connectivity establishment",
		plugin_init, "0.1.4", "LGPL", "libnice",
		"http://telepathy.freedesktop.org/wiki/", "");

	openslessrc = gst_element_factory_make ("openslessrc", NULL);
	audioconvert = gst_element_factory_make ("audioconvert", NULL);
	caps = gst_element_factory_make ("capsfilter", NULL);
	rtpL16pay = gst_element_factory_make ("rtpL16pay", NULL);
	nicesink = gst_element_factory_make ("nicesink", NULL);

	g_object_set (caps, "caps", gst_caps_from_string("audio/x-raw-int, channels=1, rate=16000, payload=96"), NULL);
	//Set properties
	g_object_set (nicesink, "agent", magent, NULL);
	g_object_set (nicesink, "stream", stream_id, NULL);
	g_object_set (nicesink, "component", 1, NULL);


	pipeline = gst_pipeline_new ("Audio pipeline");
	if (!pipeline || !openslessrc || !audioconvert || !caps || !rtpL16pay || !nicesink)
	{
		g_printerr ("Not all elements could be created.\n");
		return;
	}

	gst_bin_add_many (GST_BIN (pipeline), openslessrc, audioconvert, caps, rtpL16pay, nicesink, NULL);
	if (gst_element_link_many (openslessrc, audioconvert, caps, rtpL16pay, nicesink, NULL) != TRUE)
	{
		g_printerr ("Elements could not be linked.\n");
		gst_object_unref (pipeline);
		return;
	}

	bus = gst_element_get_bus (pipeline);
	gst_bus_enable_sync_message_emission (bus);
	gst_bus_add_signal_watch (bus);

	g_signal_connect (bus, "message::error",
			(GCallback) on_error, NULL);

	data->pipeline = pipeline;
	gst_element_set_state(data->pipeline, GST_STATE_PLAYING);
}
