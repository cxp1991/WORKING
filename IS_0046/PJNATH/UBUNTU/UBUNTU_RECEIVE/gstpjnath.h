#include "gstpjnathsink.h"
#include "gstpjnathsrc.h"

#if GST_CHECK_VERSION (1,0,0)
#define PLUGIN_NAME pjnath
#else
#define PLUGIN_NAME "pjnath"
#endif

gboolean plugin_init (GstPlugin *plugin);
