/*
 * receive_audio.c
 *
 *  Created on: Mar 16, 2014
 *      Author: cxphong
 */

#include "stream.h"
#include "../Login/login.h"

void*  _receive_audio_main(CustomData *data)
{
	// Create the nice agent
	data->agent = libnice_create_NiceAgent_with_gstreamer ( receive_audio_gathering_done,
															data->context);
	// Create a new stream with one component
	data->stream_id = libnice_create_stream_id (data->agent);

	while((*video_receive_gathering_done) == FALSE)
		usleep(100);

	 __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "start receive audio");
	/* Init Gstreamer */
	_receive_audio_init_gstreamer(data->agent, data->stream_id, data);

	// Start gathering candidates
	libnice_start_gather_candidate (data->agent,
									data->stream_id,
									data->context);
}

void  _receive_audio_init_gstreamer(NiceAgent *magent, guint streamID, CustomData *data)
{
	GstElement *pipeline, *nicesrc, *capsfilter, *rtpspeexdepay, *speexdec, *autoaudiosink;
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

	nicesrc = gst_element_factory_make ("nicesrc", NULL);
	capsfilter = gst_element_factory_make ("capsfilter", NULL);
	rtpspeexdepay = gst_element_factory_make ("rtpspeexdepay", NULL);
	speexdec = gst_element_factory_make ("speexdec", NULL);
	autoaudiosink = gst_element_factory_make ("autoaudiosink", NULL);

	g_object_set (nicesrc, "agent", magent, NULL);
	g_object_set (nicesrc, "stream", streamID, NULL);
	g_object_set (nicesrc, "component", 1, NULL);
	g_object_set (capsfilter, "caps", gst_caps_from_string("application/x-rtp, media=(string)audio, clock-rate=(int)44100, encoding-name=(string)SPEEX, encoding-params=(string)1, payload=(int)110, ssrc=(uint)1647313534, timestamp-offset=(uint)2918479805, seqnum-offset=(uint)26294"), NULL);

	pipeline = gst_pipeline_new ("test-pipeline");
	if (!pipeline || !nicesrc || !capsfilter || !rtpspeexdepay|| !speexdec|| !autoaudiosink)
	{
		g_printerr ("Not all elements could be created.\n");
		return;
	}

	gst_bin_add_many (GST_BIN (pipeline), nicesrc, capsfilter, rtpspeexdepay, speexdec, autoaudiosink, NULL);
	if (gst_element_link_many ( nicesrc, capsfilter, rtpspeexdepay, speexdec, autoaudiosink, NULL) != TRUE)
	{
		g_printerr ("Elements could not be linked.\n");
		gst_object_unref (pipeline);
		return;
	}

	data->pipeline = pipeline;
	gst_element_set_state(data->pipeline, GST_STATE_PLAYING);

//	data->video_sink = gst_bin_get_by_interface(GST_BIN(data->pipeline), GST_TYPE_X_OVERLAY);
//	if (!data->video_sink) {
//	GST_ERROR ("Could not retrieve video sink");
//	return NULL;
//	}

	//Instruct the bus to emit signals for each received message, and connect to the interesting signals
	bus = gst_element_get_bus (data->pipeline);
	bus_source = gst_bus_create_watch (bus);
	g_source_set_callback (bus_source, (GSourceFunc) gst_bus_async_signal_func, NULL, NULL);
	g_source_attach (bus_source, data->context);
	g_source_unref (bus_source);
	//g_signal_connect (G_OBJECT (bus), "message::error", (GCallback)error_cb, data);
	//g_signal_connect (G_OBJECT (bus), "message::state-changed", (GCallback)state_changed_cb, data);
	gst_object_unref (bus);

	//Create a GLib Main Loop and set it to run
	GST_DEBUG ("Entering main loop... (CustomData:%p)", data);
	//data->main_loop = g_main_loop_new (data->context, FALSE);
	//check_initialization_complete (data);
}
