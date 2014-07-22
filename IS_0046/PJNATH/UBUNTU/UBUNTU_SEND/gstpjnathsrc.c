#include <string.h>

#include "gstpjnathsrc.h"

GstpjnathSrc *src;

GST_DEBUG_CATEGORY_STATIC (pjnathsrc_debug);
#define GST_CAT_DEFAULT pjnathsrc_debug


#define BUFFER_SIZE (65536)

static GstFlowReturn
gst_pjnath_src_create (
  GstPushSrc *basesrc,
  GstBuffer **buffer);

static gboolean
gst_pjnath_src_unlock (
    GstBaseSrc *basesrc);

static gboolean
gst_pjnath_src_unlock_stop (
    GstBaseSrc *basesrc);

static void
gst_pjnath_src_set_property (
  GObject *object,
  guint prop_id,
  const GValue *value,
  GParamSpec *pspec);

static void
gst_pjnath_src_get_property (
  GObject *object,
  guint prop_id,
  GValue *value,
  GParamSpec *pspec);


static void
gst_pjnath_src_dispose (GObject *object);

static GstStateChangeReturn
gst_pjnath_src_change_state (
    GstElement * element,
    GstStateChange transition);

static GstStaticPadTemplate gst_pjnath_src_src_template =
GST_STATIC_PAD_TEMPLATE (
    "src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

G_DEFINE_TYPE (GstpjnathSrc, gst_pjnath_src, GST_TYPE_PUSH_SRC);

enum
{
  PROP_INSTANCE = 1,
  PROP_ADDRESS,
  PROP_COMPONENT
};


static void
gst_pjnath_src_class_init (GstpjnathSrcClass *klass)
{
  printf ("gst_pjnath_src_class_init\n");

  GstPushSrcClass *gstpushsrc_class;
  GstBaseSrcClass *gstbasesrc_class;
  GstElementClass *gstelement_class;
  GObjectClass *gobject_class;

  GST_DEBUG_CATEGORY_INIT (pjnathsrc_debug, "pjnathsrc",
      0, "libpjnath source");

  gstpushsrc_class = (GstPushSrcClass *) klass;
  gstpushsrc_class->create = GST_DEBUG_FUNCPTR (gst_pjnath_src_create);

  gstbasesrc_class = (GstBaseSrcClass *) klass;
  gstbasesrc_class->unlock = GST_DEBUG_FUNCPTR (gst_pjnath_src_unlock);
  gstbasesrc_class->unlock_stop = GST_DEBUG_FUNCPTR (gst_pjnath_src_unlock_stop);

  gobject_class = (GObjectClass *) klass;
  gobject_class->set_property = gst_pjnath_src_set_property;
  gobject_class->get_property = gst_pjnath_src_get_property;
  gobject_class->dispose = gst_pjnath_src_dispose;

  gstelement_class = (GstElementClass *) klass;
  gstelement_class->change_state = gst_pjnath_src_change_state;

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&gst_pjnath_src_src_template));
#if GST_CHECK_VERSION (1,0,0)
  gst_element_class_set_metadata (gstelement_class,
#else
  gst_element_class_set_details_simple (gstelement_class,
#endif
      "ICE source",
      "Source",
      "Interactive UDP connectivity establishment",
      "Dafydd Harries <dafydd.harries@collabora.co.uk>");

  g_object_class_install_property (gobject_class,PROP_INSTANCE,
      g_param_spec_pointer (
         "icest",
         "ice instance src",
         "The pjnathAgent this source is bound to",
         G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_ADDRESS,
     g_param_spec_pointer (
         "address",
         "address src",
         "The pjnathAgent this source is bound to",
         G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_COMPONENT,
      g_param_spec_uint (
         "component",
         "Component ID src",
         "The ID of the component to read from",
         0,
         G_MAXUINT,
         0,
         G_PARAM_READWRITE));
}

static void
gst_pjnath_src_init (GstpjnathSrc *src)
{
  gst_base_src_set_live (GST_BASE_SRC (src), TRUE);
  gst_base_src_set_format (GST_BASE_SRC (src), GST_FORMAT_TIME);
  gst_base_src_set_do_timestamp (GST_BASE_SRC (src), TRUE);
  src->icest = NULL;
  src->def_addr = NULL;
  src->comp_id = 0;
  src->mainctx = g_main_context_new ();
  src->mainloop = g_main_loop_new (src->mainctx, FALSE);
  src->unlocked = FALSE;
  src->idle_source = NULL;
  src->outbufs = g_queue_new ();
}

void gst_cb_on_rx_data (pj_ice_strans *ice_st,
			  unsigned comp_id, 
			  void *pkt, pj_size_t size,
			  const pj_sockaddr_t *src_addr,
			  unsigned src_addr_len)
{
  GstBaseSrc *basesrc = src;
  GstpjnathSrc *pjnathsrc = GST_PJNATH_SRC (basesrc);
  GstBuffer *buffer = NULL;

//  GST_LOG_OBJECT (agent, "Got buffer, getting out of the main loop");

#if GST_CHECK_VERSION (1,0,0)
  buffer = gst_buffer_new_allocate (NULL, size, NULL);
  gst_buffer_fill (buffer, 0, pkt, size);
#else
  buffer = gst_buffer_new_and_alloc (size);
  memcpy (GST_BUFFER_DATA (buffer), pkt, size);
#endif
  g_queue_push_tail (pjnathsrc->outbufs, buffer);

  g_main_loop_quit (pjnathsrc->mainloop);
}

static gboolean
gst_pjnath_src_unlock_idler (gpointer data)
{
  GstpjnathSrc *pjnathsrc = GST_PJNATH_SRC (data);

  GST_OBJECT_LOCK (pjnathsrc);
  if (pjnathsrc->unlocked)
    g_main_loop_quit (pjnathsrc->mainloop);

  if (pjnathsrc->idle_source) {
    g_source_destroy (pjnathsrc->idle_source);
    g_source_unref (pjnathsrc->idle_source);
    pjnathsrc->idle_source = NULL;
  }
  GST_OBJECT_UNLOCK (pjnathsrc);

  return FALSE;
}

static gboolean
gst_pjnath_src_unlock (GstBaseSrc *src)
{
  GstpjnathSrc *pjnathsrc = GST_PJNATH_SRC (src);

  GST_OBJECT_LOCK (src);
  pjnathsrc->unlocked = TRUE;

  g_main_loop_quit (pjnathsrc->mainloop);

  if (!pjnathsrc->idle_source) {
    pjnathsrc->idle_source = g_idle_source_new ();
    g_source_set_priority (pjnathsrc->idle_source, G_PRIORITY_HIGH);
    g_source_set_callback (pjnathsrc->idle_source, gst_pjnath_src_unlock_idler, src, NULL);
    g_source_attach (pjnathsrc->idle_source, g_main_loop_get_context (pjnathsrc->mainloop));
  }
  GST_OBJECT_UNLOCK (src);

  return TRUE;
}

static gboolean
gst_pjnath_src_unlock_stop (GstBaseSrc *src)
{
  GstpjnathSrc *pjnathsrc = GST_PJNATH_SRC (src);

  GST_OBJECT_LOCK (src);
  pjnathsrc->unlocked = FALSE;
  if (pjnathsrc->idle_source) {
    g_source_destroy (pjnathsrc->idle_source);
    g_source_unref(pjnathsrc->idle_source);
  }
  pjnathsrc->idle_source = NULL;
  GST_OBJECT_UNLOCK (src);

  return TRUE;
}

static GstFlowReturn
gst_pjnath_src_create (
  GstPushSrc *basesrc,
  GstBuffer **buffer)
{
  GstpjnathSrc *pjnathsrc = GST_PJNATH_SRC (basesrc);

  GST_LOG_OBJECT (pjnathsrc, "create called");

  GST_OBJECT_LOCK (basesrc);
  if (pjnathsrc->unlocked) {
    GST_OBJECT_UNLOCK (basesrc);
#if GST_CHECK_VERSION (1,0,0)
    return GST_FLOW_FLUSHING;
#else
    return GST_FLOW_WRONG_STATE;
#endif
  }
  GST_OBJECT_UNLOCK (basesrc);

  if (g_queue_is_empty (pjnathsrc->outbufs))
    g_main_loop_run (pjnathsrc->mainloop);

  *buffer = g_queue_pop_head (pjnathsrc->outbufs);
  if (*buffer != NULL) {
    GST_LOG_OBJECT (pjnathsrc, "Got buffer, pushing");
    return GST_FLOW_OK;
  } else {
    GST_LOG_OBJECT (pjnathsrc, "Got interrupting, returning wrong-state");
#if GST_CHECK_VERSION (1,0,0)
    return GST_FLOW_FLUSHING;
#else
    return GST_FLOW_WRONG_STATE;
#endif
  }

}

static void
gst_pjnath_src_dispose (GObject *object)
{
  GstpjnathSrc *src = GST_PJNATH_SRC (object);

  if (src->icest)
    g_object_unref (src->icest);
  src->icest = NULL;

  if (src->mainloop)
    g_main_loop_unref (src->mainloop);
  src->mainloop = NULL;

  if (src->mainctx)
    g_main_context_unref (src->mainctx);
  src->mainctx = NULL;

  if (src->outbufs)
    g_queue_free (src->outbufs);
  src->outbufs = NULL;

  G_OBJECT_CLASS (gst_pjnath_src_parent_class)->dispose (object);
}

static void
gst_pjnath_src_set_property (
  GObject *object,
  guint prop_id,
  const GValue *value,
  GParamSpec *pspec)
{
  GstpjnathSrc *src = GST_PJNATH_SRC (object);

  switch (prop_id)
    {
    case PROP_INSTANCE:
      if (src->icest)
        GST_ERROR_OBJECT (object,
            "Changing the agent on a pjnath src not allowed");
      else
        src->icest = g_value_get_pointer (value);
      break;

    case PROP_ADDRESS:
      src->def_addr = g_value_get_pointer (value);
      break;

    case PROP_COMPONENT:
      src->comp_id = g_value_get_uint (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gst_pjnath_src_get_property (
  GObject *object,
  guint prop_id,
  GValue *value,
  GParamSpec *pspec)
{
  GstpjnathSrc *src = GST_PJNATH_SRC (object);

  switch (prop_id)
    {
    case PROP_INSTANCE:
      g_value_set_pointer (value, src->icest);
      break;

    case PROP_ADDRESS:
      g_value_set_pointer (value, src->def_addr);
      break;

    case PROP_COMPONENT:
      g_value_set_uint (value, src->comp_id);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static GstStateChangeReturn
gst_pjnath_src_change_state (GstElement * element, GstStateChange transition)
{
  //GstpjnathSrc *src;
  GstStateChangeReturn ret;

  src = GST_PJNATH_SRC (element);

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      if (src->icest == NULL || src->def_addr == NULL || src->comp_id == 0)
        {
          GST_ERROR_OBJECT (element,
              "Trying to start pjnath source without an agent set");
          return GST_STATE_CHANGE_FAILURE;
        }
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
    case GST_STATE_CHANGE_PAUSED_TO_READY:
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (gst_pjnath_src_parent_class)->change_state (element,
      transition);

  return ret;
}


