#include "stream.h"

void on_state_changed ( GstBus *bus,
						GstMessage *msg,
						CustomData *data);

void on_error (	GstBus     *bus,
          	  	GstMessage *message,
          	  	gpointer    user_data);

void on_pad_added (GstElement* object,
				   GstPad* pad,
				   gpointer data);
