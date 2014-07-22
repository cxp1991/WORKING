#ifndef _GST_PJNATH_SINK_H
#define _GST_PJNATH_SINK_H

#include <gst/gst.h>
#include <gst/base/gstbasesink.h>
#include <pjlib.h>
#include <pjlib-util.h>
#include <pjnath.h>

G_BEGIN_DECLS

#define GST_TYPE_PJNATH_SINK \
  (gst_pjnath_sink_get_type())
#define GST_PJNATH_SINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_PJNATH_SINK,GstPjnathSink))
#define GST_PJNATH_SINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_PJNATH_SINK,GstPjnathSinkClass))
#define GST_IS_PJNATH_SINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_PJNATH_SINK))
#define GST_IS_PJNATH_SINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_PJNATH_SINK))

typedef struct _GstPjnathSink GstPjnathSink;

struct _GstPjnathSink
{

  GstBaseSink parent;
  GstPad *sinkpad;

  pj_ice_strans	*icest;
  guint comp_id;
  pj_sockaddr *def_addr;
  pj_pool_t *pool;

};

typedef struct _GstPjnathSinkClass GstPjnathSinkClass;

struct _GstPjnathSinkClass
{
  GstBaseSinkClass parent_class;
};

GType gst_pjnath_sink_get_type (void);

G_END_DECLS

#endif
