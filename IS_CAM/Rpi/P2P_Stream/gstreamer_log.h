/*
 * gstreamer_log.h
 *
 *  Created on: Mar 27, 2014
 *      Author: cxphong
 */

#ifndef GSTREAMER_LOG_H_
#define GSTREAMER_LOG_H_

#include <gst/gst.h>

void
on_error_video (GstBus     *bus,
          GstMessage *message,
          gpointer    user_data);



#endif /* GSTREAMER_LOG_H_ */
