int check_stun_info (char *stun_message);

void  _video_send_cb_candidate_gathering_done(NiceAgent *agent, guint stream_id, gpointer data);

int  _video_send_print_local_data(NiceAgent *agent, guint stream_id, guint component_id);

int  _video_send_parse_remote_data(NiceAgent *agent, guint streamID,
    guint component_id, char *line);

static NiceCandidate* _video_send_parse_candidate(char *scand, guint streamID);

static void _video_send_cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
    guint len, gchar *buf, gpointer data);

static int _video_send_ClientThread();
