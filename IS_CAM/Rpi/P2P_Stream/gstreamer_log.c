#include "gstreamer_log.h"
void
on_error_video (GstBus     *bus,
          GstMessage *message,
          gpointer    user_data)
{
	  GError *err;
	  gchar *debug_info;
	  gchar *message_string;

	  gst_message_parse_error (message, &err, &debug_info);
	  message_string = g_strdup_printf ("Error received from element %s: %s", GST_OBJECT_NAME (message->src), err->message);

      g_message ("debug_info = %s \n message_string = %s\n", debug_info, message_string);
	  g_clear_error (&err);
	  g_free (debug_info);
	  g_free (message_string);
}

/* Listen for element's state change */
static void on_state_changed (GstBus *bus, GstMessage *msg, gpointer user_data)
{
	GstState old_state, new_state, pending_state;
	gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
	gchar *message = g_strdup_printf("%s changed to state %s",  GST_MESSAGE_SRC_NAME(msg), gst_element_state_get_name(new_state));
	g_printerr("%s\n", message);
	g_free (message);
}

