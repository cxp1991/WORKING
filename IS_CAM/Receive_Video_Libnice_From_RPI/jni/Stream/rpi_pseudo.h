#include "stream.h"

void *rpi_pseudo_func (GST_CUSTOM_DATA *data);
static void wait_peer_connect_to ();
static void peer_shaking (GST_CUSTOM_DATA *data);
static void peer_init_gstreamer (	NiceAgent *magent,
							guint streamID,
							CustomData *video_data);