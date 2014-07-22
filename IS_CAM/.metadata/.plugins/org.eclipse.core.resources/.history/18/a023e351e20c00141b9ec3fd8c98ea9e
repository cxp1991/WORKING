#include "stream.h"
#include "../Login/login.h"


/* Change the content of the UI's TextView */
 void set_ui_message (const gchar *message, CustomData *data) {
  JNIEnv *env = get_jni_env ();
  GST_DEBUG ("Setting message to: %s", message);
  jstring jmessage = (*env)->NewStringUTF(env, message);
  (*env)->CallVoidMethod (env, data->app, set_message_method_id, jmessage);
  if ((*env)->ExceptionCheck (env)) {
    GST_ERROR ("Failed to call Java method");
    (*env)->ExceptionClear (env);
  }
  (*env)->DeleteLocalRef (env, jmessage);
}

/* Retrieve errors from the bus and show them on the UI */
void error_cb (GstBus *bus, GstMessage *msg, CustomData *data) {
  GError *err;
  gchar *debug_info;
  gchar *message_string;

  gst_message_parse_error (msg, &err, &debug_info);
  message_string = g_strdup_printf ("Error received from element %s: %s", GST_OBJECT_NAME (msg->src), err->message);
  g_clear_error (&err);
  g_free (debug_info);
  set_ui_message (message_string, data);
  g_free (message_string);
  gst_element_set_state (data->pipeline, GST_STATE_NULL);
}

/* Notify UI about pipeline state changes */
void state_changed_cb (GstBus *bus, GstMessage *msg, CustomData *data) {
  GstState old_state, new_state, pending_state;
  gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
  /* Only pay attention to messages coming from the pipeline, not its children */
  if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->pipeline)) {
    gchar *message = g_strdup_printf("State changed to %s", gst_element_state_get_name(new_state));
    set_ui_message(message, data);
    g_free (message);
  }
}

/* Check if all conditions are met to report GStreamer as initialized.
 * These conditions will change depending on the application */
void check_initialization_complete (CustomData *data) {
  JNIEnv *env = get_jni_env ();
  if (!data->initialized && data->native_window && data->main_loop) {
    GST_DEBUG ("Initialization complete, notifying application. native_window:%p main_loop:%p", data->native_window, data->main_loop);

    /* The main loop is running and we received a native window, inform the sink about it */
    gst_x_overlay_set_window_handle (GST_X_OVERLAY (data->video_sink), (guintptr)data->native_window);

    (*env)->CallVoidMethod (env, data->app, on_gstreamer_initialized_method_id);
    if ((*env)->ExceptionCheck (env)) {
      GST_ERROR ("Failed to call Java method");
      (*env)->ExceptionClear (env);
    }
    data->initialized = TRUE;
  }
}

/* Main method for the native code. This is executed on its own thread. */
 void *app_function (void *userdata) {
	 __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "app_function");
	JavaVMAttachArgs args;
	GstBus *bus;
	GST_CUSTOM_DATA *app_data = (GST_CUSTOM_DATA* )userdata;
	GError *error = NULL;
	GThread *text_receive, *video_receive, *send_audio, *receive_audio;

	GST_DEBUG ("Creating pipeline in CustomData at %p", app_data);


	/* Create our own GLib Main Context and make it the default one */
	app_data->video_receive_data->context = g_main_context_new ();
	app_data->send_audio_data->context = app_data->video_receive_data->context;
	app_data->receive_audio_data->context = app_data->video_receive_data->context;
	app_data->text->context = app_data->video_receive_data->context;
	g_main_context_push_thread_default(app_data->video_receive_data->context);
	app_data->video_receive_data->main_loop = g_main_loop_new (app_data->video_receive_data->context, FALSE);
	app_data->send_audio_data->main_loop = app_data->video_receive_data->main_loop;
	app_data->receive_audio_data->main_loop = app_data->video_receive_data->main_loop;
	app_data->text->main_loop = app_data->video_receive_data->main_loop;

	/* Request to connect Rpi */
	//connect_to_rpi();

	/* Gathering flag for each thread */
	video_receive_gathering_done = (gboolean *)malloc(sizeof(gboolean));
	(*video_receive_gathering_done) = FALSE;

	send_audio_gathering_done = (gboolean *)malloc(sizeof(gboolean));
	*send_audio_gathering_done = FALSE;

	receive_audio_gathering_done = (gboolean *)malloc(sizeof(gboolean));
	(*receive_audio_gathering_done) = FALSE;

	controller_gathering_done = (gboolean *)malloc(sizeof(gboolean));
	(*controller_gathering_done) = FALSE;

	is_connect_to_rpi = FALSE;

	video_receive = g_thread_new("video_receive_main", &_video_receive_main, app_data->video_receive_data);
	receive_audio = g_thread_new("receive audio", &_receive_audio_main, app_data->receive_audio_data);
	send_audio = g_thread_new("send_audio", &_send_audio_main, app_data->send_audio_data);
	text_receive = g_thread_new("text_receive_main", &controller, app_data->text);

	g_main_loop_run (app_data->video_receive_data->main_loop);

	/* Free resource */
	g_thread_join (video_receive);
	g_thread_join (receive_audio);
	g_thread_join (send_audio);
	g_thread_join (text_receive);

	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "Quit program");
	g_main_loop_unref (app_data->video_receive_data->main_loop);
	app_data->video_receive_data->main_loop = NULL;
	g_main_context_pop_thread_default(app_data->video_receive_data->context);
	g_main_context_unref (app_data->video_receive_data->context);

	/* receive video */
	gst_element_set_state (app_data->video_receive_data->pipeline, GST_STATE_NULL);
	gst_object_unref (app_data->video_receive_data->video_sink);
	gst_object_unref (app_data->video_receive_data->pipeline);
	g_object_unref(app_data->video_receive_data->agent);

	/* receive audio */
	gst_element_set_state (app_data->receive_audio_data->pipeline, GST_STATE_NULL);
	gst_object_unref (app_data->receive_audio_data->pipeline);
	g_object_unref(app_data->receive_audio_data->agent);

	/* send audio */
	gst_element_set_state (app_data->send_audio_data->pipeline, GST_STATE_NULL);
	gst_object_unref (app_data->send_audio_data->pipeline);
	g_object_unref(app_data->send_audio_data->agent);

	/* text */
	g_object_unref(app_data->text->agent);

	return NULL;
}

/*
 * Java Bindings
 */

/* Instruct the native code to create its internal data structure, pipeline and thread */
 void gst_native_init (JNIEnv* env, jobject thiz, jstring _client_name, jstring _rpi_name) {

  client_name = (*env)->GetStringUTFChars(env, _client_name, 0);
  rpi_name = (*env)->GetStringUTFChars(env, _rpi_name, 0);

  GST_CUSTOM_DATA *app_data;
  app_data = g_new0 (GST_CUSTOM_DATA, 1);
  app_data->video_receive_data = g_new0 (CustomData, 1);
  app_data->send_audio_data = g_new0 (CustomData, 1);
  app_data->receive_audio_data = g_new0 (CustomData, 1);
  app_data->text = g_new0 (CustomData, 1);

  SET_CUSTOM_DATA (env, thiz, video_receive_custom_data_field_id, app_data->video_receive_data);

  GST_DEBUG_CATEGORY_INIT (debug_category, "tutorial-3", 0, "Android tutorial 3");
  gst_debug_set_threshold_for_name("tutorial-3", GST_LEVEL_DEBUG);
  GST_DEBUG ("Created CustomData at %p", app_data);

  app_data->video_receive_data->app = (*env)->NewGlobalRef (env, thiz);
  app_data->send_audio_data->app = (*env)->NewGlobalRef (env, thiz);
  app_data->receive_audio_data->app = (*env)->NewGlobalRef (env, thiz);
  app_data->text->app = (*env)->NewGlobalRef (env, thiz);
  __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "%d %d %d", app_data->video_receive_data->app,
		  app_data->send_audio_data->app, app_data->receive_audio_data->app);
  pthread_create (&gst_app_thread, NULL, &app_function, app_data);
  __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "NativeInit done");
}

/* Quit the main loop, remove the native thread and free resources */
 void gst_native_finalize (JNIEnv* env, jobject thiz) {
  GST_CUSTOM_DATA *app_data;

  /* Gathering flag for each thread */
   (*video_receive_gathering_done) = FALSE;
  	*send_audio_gathering_done = FALSE;
	(*receive_audio_gathering_done) = FALSE;
  	(*controller_gathering_done) = FALSE;

//  /* close socket */
//  shutdown(global_socket, 2);

  app_data->video_receive_data = GET_CUSTOM_DATA (env, thiz, video_receive_custom_data_field_id);

  if (!app_data) return;
  	  GST_DEBUG ("Quitting main loop...");

  g_main_loop_quit (app_data->video_receive_data->main_loop);
  GST_DEBUG ("Waiting for thread to finish...");

  pthread_join (gst_app_thread, NULL);
	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "Deleting GlobalRef for app object at %p", app_data->video_receive_data->app);

  (*env)->DeleteGlobalRef (env, app_data->video_receive_data->app);
	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "Freeing CustomData at %p", app_data->video_receive_data);

//  (*env)->DeleteGlobalRef (env, app_data->send_audio_data->app);
//  __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "Freeing CustomData at %p", app_data->send_audio_data);
//
//  __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "7");
//  (*env)->DeleteGlobalRef (env, app_data->receive_audio_data->app);
//  __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "Freeing CustomData at %p", app_data->receive_audio_data);
//
// __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "8");
  g_free (app_data);
  SET_CUSTOM_DATA (env, thiz, video_receive_custom_data_field_id, NULL);

  __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "9");
}


/* Set pipeline to PLAYING state */
 void gst_native_play (JNIEnv* env, jobject thiz) {
	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "gst_native_play");
  CustomData *data = GET_CUSTOM_DATA (env, thiz, video_receive_custom_data_field_id);
  if (!data) return;
  GST_DEBUG ("Setting state to PLAYING");
  gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
}

/* Set pipeline to PAUSED state */
 void gst_native_pause (JNIEnv* env, jobject thiz) {
	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "gst_native_paused");
  CustomData *data = GET_CUSTOM_DATA (env, thiz, video_receive_custom_data_field_id);
  if (!data) return;
  GST_DEBUG ("Setting state to PAUSED");
  gst_element_set_state (data->pipeline, GST_STATE_PAUSED);
}

/*  class initializer: retrieve method and field IDs */
 jboolean gst_native_class_init (JNIEnv* env, jclass klass) {
	 __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "gst_native_class_init start");
	video_receive_custom_data_field_id = (*env)->GetFieldID (env, klass, "video_receive_native_custom_data", "J");
	send_audio_custom_data_field_id = (*env)->GetFieldID (env, klass, "audio_send_native_custom_data", "J");
	set_message_method_id = (*env)->GetMethodID (env, klass, "setMessage", "(Ljava/lang/String;)V");
	__android_log_print (ANDROID_LOG_ERROR, "tutorial-3", "set_message_method_id = %d", set_message_method_id);
	on_gstreamer_initialized_method_id = (*env)->GetMethodID (env, klass, "onGStreamerInitialized", "()V");
	//jclass cls = (*env)->FindClass(env, "com/gst_sdk_tutorials/tutorial_3/Communicate_Rpi");
	//set_message_from_rpi = (*env)->GetMethodID (env, cls, "receive_message_from_rpi", "(Ljava/lang/String;)V");

	if (!send_audio_custom_data_field_id || !video_receive_custom_data_field_id  ||
			!set_message_method_id || !on_gstreamer_initialized_method_id)
	{
		/* We emit this message through the Android log instead of the GStreamer log because the later
		* has not been initialized yet.
		*/
		__android_log_print (ANDROID_LOG_ERROR, "tutorial-3", "The calling class does not implement all necessary interface methods");
		return JNI_FALSE;
	}
	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "NativeClassInit done");
	return JNI_TRUE;
}

void gst_native_surface_init (JNIEnv *env, jobject thiz, jobject surface)
{
	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "native_surface_init start");
	CustomData *data = GET_CUSTOM_DATA (env, thiz, video_receive_custom_data_field_id);

	if (!data)
		return;

	ANativeWindow *new_native_window = ANativeWindow_fromSurface(env, surface);
	GST_DEBUG ("Received surface %p (native window %p)", surface, new_native_window);
	if (data->native_window)
	{
		ANativeWindow_release (data->native_window);

		if (data->native_window == new_native_window)
		{
			GST_DEBUG ("New native window is the same as the previous one", data->native_window);
			if (data->video_sink)
			{
				gst_x_overlay_expose(GST_X_OVERLAY (data->video_sink));
				gst_x_overlay_expose(GST_X_OVERLAY (data->video_sink));
			}
			return;
		}
		else
		{
			GST_DEBUG ("Released previous native window %p", data->native_window);
			data->initialized = FALSE;
		}
	}
	data->native_window = new_native_window;

	check_initialization_complete (data);
	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "native_surface_init done");
}

void gst_native_surface_finalize (JNIEnv *env, jobject thiz)
{
	CustomData *data = GET_CUSTOM_DATA (env, thiz, video_receive_custom_data_field_id);

	if (!data) return;
		GST_DEBUG ("Releasing Native Window %p", data->native_window);

	if (data->video_sink) {
		gst_x_overlay_set_window_handle (GST_X_OVERLAY (data->video_sink), (guintptr) NULL);
		gst_element_set_state (data->pipeline, GST_STATE_READY);
	}

	ANativeWindow_release (data->native_window);
	data->native_window = NULL;
	data->initialized = FALSE;
}


void close_server_socket (JNIEnv *env, jobject thiz)
{
	/* close socket */
	shutdown(global_socket, 2);
	global_socket = 0;
	//__android_log_print (ANDROID_LOG_ERROR, "tutorial-3","global_socket = %d", global_socket);
}

void exit_streaming (JNIEnv *env, jobject thiz)
{
	char sender[181] = {0};
	char info[181] = {0};
	char receiveBuffer[181] = {0};
	char *tmp;

	/*
	 * Establish signal
	 */
	sprintf(info, "005$%s$%s", client_name, rpi_name);

	__android_log_print (ANDROID_LOG_ERROR, "tutorial-3", "info = %s", info);
	Base64Encode(info, sender, BUFFFERLEN);
	send(global_socket, sender, 181, NULL);

}

