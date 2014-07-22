#include "stream.h"
#include "../Login/login.h"

static GMutex gather_mutex, negotiate_mutex;
static gboolean exit_thread, candidate_gathering_done, negotiation_done;
static const gchar *candidate_type_name[] = {"host", "srflx", "prflx", "relay"};
static const gchar *state_name[] = {"disconnected", "gathering", "connecting",
                                    "connected", "ready", "failed"};
static guint stream_id;
static NiceAgent *agent;
static GThread* connectThread;
//GMutex *mutex;
//GCond *cond;
static gchar *mInfo_Text;
static gchar rpiInfo_Text[181] = {'\0'};
gboolean stopThread;
static int flag_trans=0;
//static gboolean hasdata = FALSE;

static gchar data_buf[512] = {0};
static int data_len = 0;
struct sockaddr_in addr;
static int sConnect;
CustomData *mData;
static int _text_receive_ClientThread();

void*  _text_receive_main(CustomData *data)
{
	mData = data;
	// Create the nice agent
	data->agent = nice_agent_new(data->context,
		NICE_COMPATIBILITY_RFC5245);
	if (data->agent == NULL)
		g_error("Failed to create agent");

	// Set the STUN settings and controlling mode
	g_object_set(G_OBJECT(data->agent), "stun-server", STUNSR_ADDR, NULL);
	g_object_set(G_OBJECT(data->agent), "stun-server-port", STUNSR_PORT, NULL);
	g_object_set(G_OBJECT(data->agent), "controlling-mode", 0, NULL);

	// Connect to the signals
	g_signal_connect(G_OBJECT(data->agent), "candidate-gathering-done",
		G_CALLBACK(_text_receive_cb_candidate_gathering_done), NULL);
	g_signal_connect(G_OBJECT(data->agent), "new-selected-pair",
		G_CALLBACK(_text_receive_cb_new_selected_pair), NULL);
	g_signal_connect(G_OBJECT(data->agent), "component-state-changed",
		G_CALLBACK(_text_receive_cb_component_state_changed), NULL);

	// Create a new stream with one component
	data->stream_id = nice_agent_add_stream(data->agent, 1);
	if (data->stream_id == 0)
		g_error("Failed to add stream");

	// Attach to the component to receive the data
	// Without this call, candidates cannot be gathered
	nice_agent_attach_recv(data->agent, data->stream_id, 1,
		data->context, _text_receive_cb_nice_recv, NULL);

	while(send_audio_gathering_done == FALSE)
		usleep(100);

	// Start gathering local candidates
	if (!nice_agent_gather_candidates(data->agent, data->stream_id))
		g_error("Failed to start candidate gathering");

	g_debug("waiting for candidate-gathering-done signal...");
	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "[TEXT] nice_agent_gather_candidates");
}

static void  _text_receive_cb_candidate_gathering_done(NiceAgent *agent, guint stream_id, gpointer data)
{
	gchar *line = NULL;
	int rval;
	int RetVal = 0;
	gboolean ret = TRUE;
	flag_trans = 0;

	_text_receive_print_local_data(agent, stream_id, 1);

	// Connect to server
//	do
//	{
//		__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "[TEXT] Connecting to server ...");
//		sConnect = connect_with_timeout(SERVER, SERVER_PORT, 5, 0); // Timeout = 5s
//
//	}while(sConnect == -1); // 1 = connect without timeout

	// Wait until another client connect to server
	connectThread = g_thread_create(_text_receive_ClientThread, NULL, FALSE, NULL);
	while (flag_trans != 1)
	{
		usleep(10000);
	}

	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "[TEXT] rpiInfo_Text = %s\n", rpiInfo_Text);
	rval = _text_receive_parse_remote_data(agent, stream_id, 1, rpiInfo_Text);
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

	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "[TEXT] Gathering done!");
}

static int  _text_receive_print_local_data(NiceAgent *agent, guint stream_id, guint component_id)
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

	mInfo_Text = (gchar*)malloc(181*sizeof(gchar));

	printf("%s %s", local_ufrag, local_password);
	sprintf(mInfo_Text, "%s %s", local_ufrag, local_password);
	for (item = cands; item; item = item->next) {
		NiceCandidate *c = (NiceCandidate *)item->data;

		nice_address_to_string(&c->addr, ipaddr);

		// (foundation),(prio),(addr),(port),(type)
		printf(" %s,%u,%s,%u,%s",
		c->foundation,
		c->priority,
		ipaddr,
		nice_address_get_port(&c->addr),
		candidate_type_name[c->type]);

//		__android_log_print (ANDROID_LOG_INFO, "tutorial-3", " %s,%u,%s,%u,%s",
//				c->foundation,
//				c->priority,
//				ipaddr,
//				nice_address_get_port(&c->addr),
//				candidate_type_name[c->type]);

		sprintf(mInfo_Text + strlen(mInfo_Text), " %s,%u,%s,%u,%s",
		c->foundation,
		c->priority,
		ipaddr,
		nice_address_get_port(&c->addr),
		candidate_type_name[c->type]);
	}
	printf("\n");

	//printf("\nmInfo_Text:\n");
	//printf("%s\n", mInfo_Text);
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

static gboolean _text_receive_stdin_remote_info_cb (GIOChannel *source, GIOCondition cond,
    gpointer data)
{
	NiceAgent *agent = data;
	gchar *line = NULL;
	int rval;
	gboolean ret = TRUE;

	if (g_io_channel_read_line (source, &line, NULL, NULL, NULL) ==
	G_IO_STATUS_NORMAL) {

		// Parse remote candidate list and set it on the agent
		rval = _text_receive_parse_remote_data(agent, stream_id, 1, line);
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
		g_free (line);
	}

	return ret;	
}

static int  _text_receive_parse_remote_data(NiceAgent *agent, guint stream_id,
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
	NiceCandidate *c = _text_receive_parse_candidate(line_argv[i], stream_id);

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

	if (!nice_agent_set_remote_credentials(agent, stream_id, ufrag, passwd)) {
	g_message("failed to set remote credentials");
	goto end;
	}

	// Note: this will trigger the start of negotiation.
	if (nice_agent_set_remote_candidates(agent, stream_id, component_id,
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

static NiceCandidate* _text_receive_parse_candidate(char *scand, guint streamID)
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
	cand->stream_id = stream_id;
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

static void _text_receive_cb_component_state_changed(NiceAgent *agent, guint stream_id,
    guint component_id, guint state, gpointer data)
{
	__android_log_print (ANDROID_LOG_ERROR, "tutorial-3", "SIGNAL: state changed %d %d %s[%d]\n",
	stream_id, component_id, state_name[state], state);

	if (state == NICE_COMPONENT_STATE_READY) {
		NiceCandidate *local, *remote;

		// Get current selected candidate pair and print IP address used
		if (nice_agent_get_selected_pair (agent, stream_id, component_id,
		&local, &remote)) {
			gchar ipaddr[INET6_ADDRSTRLEN];

			nice_address_to_string(&local->addr, ipaddr);
			__android_log_print (ANDROID_LOG_ERROR, "tutorial-3","\nNegotiation complete: ([%s]:%d,",
			ipaddr, nice_address_get_port(&local->addr));
			nice_address_to_string(&remote->addr, ipaddr);
			__android_log_print (ANDROID_LOG_ERROR, "tutorial-3"," [%s]:%d)\n", ipaddr, nice_address_get_port(&remote->addr));
		}

		// Listen to stdin and send data written to it
		//printf("\nSend lines to remote (Ctrl-D to quit):\n");
		//g_io_add_watch(io_stdin, G_IO_IN, _text_receive_stdin_send_data_cb, agent);
		//printf("> ");
		//fflush (stdout);
	} else if (state == NICE_COMPONENT_STATE_FAILED)
	{
		__android_log_print (ANDROID_LOG_ERROR, "tutorial-3", "[text] NICE_COMPONENT_STATE_FAILED");
		g_main_loop_quit (gloop);
	}
}

/*static gboolean _text_receive_stdin_send_data_cb (GIOChannel *source, GIOCondition cond,
    gpointer data)
{
	NiceAgent *agent = data;
	gchar *line = NULL;

	if (g_io_channel_read_line (source, &line, NULL, NULL, NULL) ==
	G_IO_STATUS_NORMAL) {
	nice_agent_send(agent, stream_id, 1, strlen(line), line);
	g_free (line);
	printf("> ");
	fflush (stdout);
	} else {
	nice_agent_send(agent, stream_id, 1, 1, "\0");
	// Ctrl-D was pressed.
	g_main_loop_quit (gloop);
	}

	return TRUE;
}*/

void gst_native_send_text (JNIEnv* env, jobject thiz, jstring text)
{
	const char *str= (*env)->GetStringUTFChars(env,text,0);
	nice_agent_send(agent, stream_id, 1, strlen(str), str);
}

static void _text_receive_cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
    guint len, gchar *buf, gpointer data)
{
	JNIEnv *env = get_jni_env ();
	if (len == 1 && buf[0] == '\0')
		g_main_loop_quit (mData->main_loop);
	 __android_log_print (ANDROID_LOG_ERROR, "tutorial-3", "Receive tex = %.*s",len,buf);
//	jstring jmessage = (*env)->NewStringUTF(env, buf);
//	(*env)->CallVoidMethod (env, mData->app, set_message_from_rpi, jmessage);
//	if ((*env)->ExceptionCheck (env))
//	{
//	    (*env)->ExceptionDescribe(env);
//	    (*env)->ExceptionClear (env);
//    }
//
//	(*env)->DeleteLocalRef (env, jmessage);
}

static void _text_receive_cb_new_selected_pair(NiceAgent *agent, guint stream_id,
    guint component_id, gchar *lfoundation,
    gchar *rfoundation, gpointer data)
{
	g_debug("SIGNAL: selected pair %s %s", lfoundation, rfoundation);
}

static int _text_receive_ClientThread()
{

	char *header, *init, *dest, *data;
		char buffer[181] = { 0 };
		char temp[181] = { 0 };
		char combine[181] = { 0 };
		int flag = 0;

		char receiver[181] = {0};
		char sender[181] = {0};
		int rc = 0;

		memcpy(temp, mInfo_Text, sizeof(temp));

		// Request to connect Rpi
		//send(global_socket, "001$ceslab$khtn", 181, NULL);

		while (1) {
			// The server will send a struct to the client
			// containing message and ID
			// But send only accepts a char as buffer parameter
			// so here we need to recv a char buffer and then
			// we copy the content of this buffer to our struct
			if (recv(global_socket, buffer, 181, NULL)) {
				__android_log_print(ANDROID_LOG_INFO, "tutorial-3",
						"[text] receive = %s", buffer);
				rc = Base64Decode(buffer, receiver, BUFFFERLEN);
				header = strtok(receiver, "$");
				init = strtok(NULL, "$");
				dest = strtok(NULL, "$");
				data = strtok(NULL, "$");
				//__android_log_print(ANDROID_LOG_INFO, "tutorial-3",
				//					"[VIDEO] data = %s", data);
				/*if(buffer[0]!='2')
				 {
				 memcpy(mInfo_Text,buffer,sizeof(buffer));
				 flag_trans = 1;
				 break;
				 }
				 else send(sConnect,temp,181,NULL);*/

				/* Receive rpi's info -> send its'info */
				if (!strcmp(header, "002") && flag < 1) {
					//cout<<"002";
					memcpy(rpiInfo_Text,data,strlen(data));
					sprintf(combine, "002$%s$%s$%s", dest, init, temp);
					rc = Base64Encode(combine, sender, BUFFFERLEN);
					send(global_socket, sender, 181, NULL);
					//cout<<data<<endl<<"Trade completed!"<<endl;
					flag_trans = 1;
					flag++;
					return 0;
				}
			}

			usleep(10000);
		}

		return 0;
}
