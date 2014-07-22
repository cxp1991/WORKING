#include "stream.h"

GMutex gather_mutex, negotiate_mutex;
gboolean exit_thread, candidate_gathering_done, negotiation_done;
const gchar *candidate_type_name[] = {"host", "srflx", "prflx", "relay"};
const gchar *state_name[] = {"disconnected", "gathering", "connecting",
                                    "connected", "ready", "failed"};

NiceAgent *libnice_create_NiceAgent_without_gstreamer (gboolean *signal_type,
													   GMainContext *context);

NiceAgent *libnice_create_NiceAgent_with_gstreamer ( gboolean *signal_type,
											    	 GMainContext *context);

guint libnice_create_stream_id (NiceAgent *agent);

int libnice_start_gather_candidate (NiceAgent *agent,
									guint stream_id,
									GMainContext *context);

int connect_to_rpi();

void  libnice_candidate_gathering_done (NiceAgent *agent,
										guint stream_id,
										gboolean *signal_type);

void libnice_print_local_data (	NiceAgent *agent,
								guint stream_id,
								guint component_id,
								gchar* local_info);

int libnice_receive_information_from_client_remote (gchar *local_info,
													gchar *remote_info);

void libnice_parse_remote_data (NiceAgent *agent,
								guint stream_id,
								guint component_id,
								gchar* remote_info);

NiceCandidate* libnice_parse_candidate (char *scand,
										guint stream_id);

void libnice_component_state_changed(NiceAgent *agent,
									guint stream_id,
									guint component_id,
									guint state,
									gboolean signal_type);

void libnice_new_selected_pair(	NiceAgent *agent,
								guint stream_id,
								guint component_id,
								gchar *lfoundation,
								gchar *rfoundation,
								gboolean signal_type);
