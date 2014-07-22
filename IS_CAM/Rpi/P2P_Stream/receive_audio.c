#include "stream.h"

static GMutex gather_mutex, negotiate_mutex;
static gboolean exit_thread, candidate_gathering_done, negotiation_done;
static const gchar *candidate_type_name[] = {"host", "srflx", "prflx", "relay"};
static const gchar *state_name[] = {"disconnected", "gathering", "connecting",
                                    "connected", "ready", "failed"};
static GThread* connectThread;
static GMutex *mutex;
static GCond *cond;
static gchar *mInfo_ReceiveAudio;
static gchar *AndroidInfo_ReceiveAudio;
static gboolean stopThread;
static int flag_trans=0;
static gboolean hasdata = FALSE;

static gchar data_buf[512] = {0};
static int data_len = 0;
struct sockaddr_in addr;
int sConnect;
static int _audio_receive_ClientThread();
static void	on_error (GstBus     *bus,
          GstMessage *message,
          gpointer    user_data);

void*  _audio_receive_main()
{
	//NiceAgent *agent;
	//guint streamID = 0;

	/* Init agent */
	RpiData_ReceiveAudio->agent = nice_agent_new(g_main_loop_get_context (gloop),
	NICE_COMPATIBILITY_RFC5245);
	if (RpiData_ReceiveAudio->agent == NULL)
		g_error("Failed to create agent");

	g_object_set(G_OBJECT(RpiData_ReceiveAudio->agent), "stun-server", STUNSR_ADDR, NULL);
	g_object_set(G_OBJECT(RpiData_ReceiveAudio->agent), "stun-server-port", STUNSR_PORT, NULL);
	g_object_set(G_OBJECT(RpiData_ReceiveAudio->agent), "controlling-mode", 1, NULL);

	g_signal_connect(G_OBJECT(RpiData_ReceiveAudio->agent), "candidate-gathering-done",
	G_CALLBACK( _audio_receive_cb_candidate_gathering_done), NULL);

	//g_signal_connect(G_OBJECT(agent), "new-selected-pair",
	//G_CALLBACK( _video_receive_cb_new_selected_pair), NULL);

	RpiData_ReceiveAudio->streamID = nice_agent_add_stream(RpiData_ReceiveAudio->agent, 1);
	if (RpiData_ReceiveAudio->streamID == 0)
		g_error("Failed to add stream");

	while(send_audio_gathering_done == FALSE)
		usleep(100);

	/* Init Gstreamer */
	_audio_receive_init_gstreamer(RpiData_ReceiveAudio->agent, RpiData_ReceiveAudio->streamID);

//	nice_agent_attach_recv(agent, streamID, 1,
//      	g_main_loop_get_context (gloop), _audio_receive_cb_nice_recv, NULL);

	/* Start gathering local candidates */
  	if (!nice_agent_gather_candidates(RpiData_ReceiveAudio->agent, RpiData_ReceiveAudio->streamID))
    		g_error("Failed to start candidate gathering");
	printf("[Audio] start gathering\n");

}

static void
on_error (GstBus     *bus,
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

void  _audio_receive_init_gstreamer(NiceAgent *magent, guint stream_id)
{
	//GstElement *pipeline, *nicesrc, *caps, *rtpL16depay, *audioconvert, *autoaudiosink;
	GstElement *nicesrc, *caps, *rtpL16depay, *audioconvert, *autoaudiosink;
	//GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;

	/* Initialize GStreamer */
  	gst_init (NULL, NULL);

	nicesrc = gst_element_factory_make ("nicesrc", NULL);
	caps = gst_element_factory_make ("capsfilter", NULL);
	rtpL16depay = gst_element_factory_make ("rtpL16depay", NULL);
	audioconvert = gst_element_factory_make ("audioconvert", NULL);
	autoaudiosink = gst_element_factory_make ("alsasink", NULL);

	//nicesrc
	g_object_set (nicesrc, "agent", magent, NULL);
	g_object_set (nicesrc, "stream", stream_id, NULL);
	g_object_set (nicesrc, "component", 1, NULL);

	g_object_set (caps, "caps", gst_caps_from_string("application/x-rtp, media=(string)audio, clock-rate=(int)16000, channels=(int)1, payload=(int)96"), NULL);

	/// Create the empty pipeline
	RpiData_ReceiveAudio->pipeline = gst_pipeline_new ("receive-audio-pipeline");

	if (!RpiData_ReceiveAudio->pipeline || !nicesrc ||!caps || !rtpL16depay || !audioconvert || !autoaudiosink) {
	g_printerr ("Not all elements could be created.\n");
	return -1;
	}

	/// Build the pipeline
	gst_bin_add_many (GST_BIN (RpiData_ReceiveAudio->pipeline), nicesrc, caps, rtpL16depay, audioconvert, autoaudiosink, NULL);
	if (gst_element_link_many (nicesrc, caps, rtpL16depay, audioconvert, autoaudiosink,  NULL) != TRUE)
	{
	g_printerr ("Elements could not be linked.\n");
	gst_object_unref (RpiData_ReceiveAudio->pipeline);
	return -1;
	}

	RpiData_ReceiveAudio->bus = gst_element_get_bus (RpiData_ReceiveAudio->pipeline);
  	gst_bus_enable_sync_message_emission (RpiData_ReceiveAudio->bus);
  	gst_bus_add_signal_watch (RpiData_ReceiveAudio->bus);

	g_signal_connect (RpiData_ReceiveAudio->bus, "message::error",
      		(GCallback) on_error, NULL);

	/// Start playing
	ret = gst_element_set_state (RpiData_ReceiveAudio->pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
	g_printerr ("Unable to set the pipeline to the playing state.\n");
	gst_object_unref (RpiData_ReceiveAudio->pipeline);
	return -1;
	}

}

void  _audio_receive_cb_candidate_gathering_done(NiceAgent *agent, guint stream_id, gpointer data)
{
	gchar *line = NULL;
	int rval;
	int RetVal = 0;
	gboolean ret = TRUE;
	flag_trans = 0;

	// Candidate gathering is done. Send our local candidates on stdout
	//printf("Copy this line to remote client[Video Send]:\n");
	//printf("\n  ");
	_audio_receive_print_local_data(agent, stream_id, 1);

	printf("Connected to server\n");
	// Wait until another client connect to server
	connectThread = g_thread_create(_audio_receive_ClientThread, NULL, FALSE, NULL);
	while (flag_trans != 1)
	{
		usleep(10000);
	}

	rval = _audio_receive_parse_remote_data(agent, stream_id, 1, AndroidInfo_ReceiveAudio);
	if (rval == EXIT_SUCCESS) {
		// Return FALSE so we stop listening to stdin since we parsed the
		// candidates correctly
		ret = FALSE;
		g_debug("waiting for state READY or FAILED signal...");
	} else {
		fprintf(stderr, "ERROR: failed to parse remote data\n");
		printf("Enter remote data (single line, no wrapping):\n");
		printf("> ");
		fflush (stdout);
	}

	receive_audio_gathering_done = TRUE;
	fprintf(stderr, "Gathering done!");
}

int  _audio_receive_print_local_data(NiceAgent *agent, guint stream_id, guint component_id)
{
	int result = EXIT_FAILURE;
	gchar *local_ufrag = NULL;
	gchar *local_password = NULL;
	gchar ipaddr[INET6_ADDRSTRLEN];
	GSList *cands = NULL, *item;

	if (!nice_agent_get_local_credentials(agent, stream_id,
	&local_ufrag, &local_password))
	goto end;

	cands = nice_agent_get_local_candidates(agent, stream_id, component_id);
	if (cands == NULL)
	goto end;

	mInfo_ReceiveAudio = (gchar*)malloc(181*sizeof(gchar));

	//printf("%s %s", local_ufrag, local_password);
	sprintf(mInfo_ReceiveAudio, "%s %s", local_ufrag, local_password);
	for (item = cands; item; item = item->next) {
	NiceCandidate *c = (NiceCandidate *)item->data;

		nice_address_to_string(&c->addr, ipaddr);

		// (foundation),(prio),(addr),(port),(type)
		/*printf(" %s,%u,%s,%u,%s",
		c->foundation,
		c->priority,
		ipaddr,
		nice_address_get_port(&c->addr),
		candidate_type_name[c->type]);*/

		sprintf(mInfo_ReceiveAudio + strlen(mInfo_ReceiveAudio), " %s,%u,%s,%u,%s",
		c->foundation,
		c->priority,
		ipaddr,
		nice_address_get_port(&c->addr),
		candidate_type_name[c->type]);
	}
	printf("\n");

	//printf("\nmInfo_ReceiveAudio:\n");
	//printf("%s\n", mInfo_ReceiveAudio);
	result = EXIT_SUCCESS;

	end:
	if (local_ufrag)
		g_free(local_ufrag);
	if (local_password)
		g_free(local_password);
	if (cands)
		g_slist_free_full(cands, (GDestroyNotify)&nice_candidate_free);

	return result;
}

int  _audio_receive_parse_remote_data(NiceAgent *agent, guint streamID,
    guint component_id, char *line)
{
	GSList *remote_candidates = NULL;
	gchar **line_argv = NULL;
	const gchar *ufrag = NULL;
	const gchar *passwd = NULL;
	int result = EXIT_FAILURE;
	int i;

	line_argv = g_strsplit_set (line, " \t\n", 0);
	for (i = 0; line_argv && line_argv[i]; i++) {
	if (strlen (line_argv[i]) == 0)
	continue;

	// first two args are remote ufrag and password
	if (!ufrag) {
	ufrag = line_argv[i];
	} else if (!passwd) {
	passwd = line_argv[i];
	} else {
	// Remaining args are serialized canidates (at least one is required)
	NiceCandidate *c = _audio_receive_parse_candidate(line_argv[i], streamID);

	if (c == NULL) {
	g_message("failed to parse candidate: %s", line_argv[i]);
	goto end;
	}
	remote_candidates = g_slist_prepend(remote_candidates, c);
	}
	}
	if (ufrag == NULL || passwd == NULL || remote_candidates == NULL) {
	g_message("line must have at least ufrag, password, and one candidate");
	goto end;
	}

	if (!nice_agent_set_remote_credentials(agent, streamID, ufrag, passwd)) {
	g_message("failed to set remote credentials");
	goto end;
	}

	// Note: this will trigger the start of negotiation.
	if (nice_agent_set_remote_candidates(agent, streamID, component_id,
	remote_candidates) < 1) {
	g_message("failed to set remote candidates");
	goto end;
	}

	result = EXIT_SUCCESS;

	end:
	if (line_argv != NULL)
	g_strfreev(line_argv);
	if (remote_candidates != NULL)
	g_slist_free_full(remote_candidates, (GDestroyNotify)&nice_candidate_free);

	return result;
}

NiceCandidate* _audio_receive_parse_candidate(char *scand, guint streamID)
{
	NiceCandidate *cand = NULL;
	NiceCandidateType ntype;
	gchar **tokens = NULL;
	guint i;

	tokens = g_strsplit (scand, ",", 5);
	for (i = 0; tokens && tokens[i]; i++);
	if (i != 5)
	goto end;

	for (i = 0; i < G_N_ELEMENTS (candidate_type_name); i++) {
		if (strcmp(tokens[4], candidate_type_name[i]) == 0) {
			ntype = i;
			break;
		}
	}
	if (i == G_N_ELEMENTS (candidate_type_name))
		goto end;

	cand = nice_candidate_new(ntype);
	cand->component_id = 1;
	cand->stream_id = streamID;
	cand->transport = NICE_CANDIDATE_TRANSPORT_UDP;
	strncpy(cand->foundation, tokens[0], NICE_CANDIDATE_MAX_FOUNDATION);
	cand->priority = atoi (tokens[1]);

	if (!nice_address_set_from_string(&cand->addr, tokens[2])) {
		g_message("failed to parse addr: %s", tokens[2]);
		nice_candidate_free(cand);
		cand = NULL;
		goto end;
	}

	nice_address_set_port(&cand->addr, atoi (tokens[3]));

	end:
	g_strfreev(tokens);

	return cand;
}

static void _audio_receive_cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
    guint len, gchar *buf, gpointer data)
{
	if (len == 1 && buf[0] == '\0')
	g_main_loop_quit (gloop);
	printf("%.*s", len, buf);
	fflush(stdout);
}

static int _audio_receive_ClientThread()
{
	char *header, *init , *dest, *data;
	char buffer[181] = {0};
	char temp[181]={0};
	char temp1[181]={0};
	char combine[181]={0};

	char receiver[181] = {0};
	char sender[181] = {0};
	int rc;

	// Send ice ifo to android
	memcpy(temp,mInfo_ReceiveAudio,sizeof(temp));
	sprintf(combine,"002$%s$%s$%s",destBuf,originBuf,temp);
	rc = Base64Encode(combine, sender, BUFFFERLEN);
	send(global_socket,sender,181,NULL);
	printf ("=============== Receive Audio ===============\n");
	printf("[receive audio] send = %s\n", combine);
	printf("[receive audio] send[Encode] = %s\n", sender);

	//Receive ice info from android
	if(recv(global_socket, buffer, 181, NULL))
	{
		rc = Base64Decode(buffer, receiver, BUFFFERLEN);
		printf("[receive audio] receive = %s\n", buffer);
		printf("[receive audio] receive[Decode] = %s\n", receiver);
		header = strtok (receiver,"$");
		init = strtok (NULL,"$");
		dest = strtok (NULL,"$");
		data = strtok (NULL,"$");
		//printf("%s - %s\n",header ,data);

		int j=0;
		if (data!=NULL)
			while(j<strlen(data))
			{
				temp1[j] = *(data+j);
				j++;
			}

		if(!strcmp(header,"002"))
		{
			AndroidInfo_ReceiveAudio = (gchar*)malloc(sizeof(gchar)*181);
			memcpy(AndroidInfo_ReceiveAudio, temp1, sizeof(temp1));
			flag_trans = 1;
			return 0;
		}
	}

}
