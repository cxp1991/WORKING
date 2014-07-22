#include <agent.h>
#include <gst/gst.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include "../Connect_Server/connect_to_server.h"
#include "../Utils/base64.h"

#ifndef _GLOBAL_INFO
#define _GLOBAL_INFO

GMainLoop *gloop;
GIOChannel* io_stdin;
gboolean video_send_gathering_done;
gboolean send_audio_gathering_done;
gboolean receive_audio_gathering_done;
gboolean text_gathering_done;

/* Use for free all data after Android exit*/
typedef struct CustomData {

	NiceAgent *agent;
	guint streamID;
	GstElement *pipeline;
	GstBus *bus;

} Rpi_Data;

Rpi_Data *RpiData_SendVideo;
Rpi_Data *RpiData_SendAudio;
Rpi_Data *RpiData_ReceiveAudio;
Rpi_Data *RpiData_Text;

/* STUN server */
#define STUNSR_ADDR  "107.23.150.92"
#define STUNSR_PORT 3478
#define CONTROLLING_MODE 1
#define CONTROLLED_MODE 0
#endif

int stream ();

#ifndef _video_send
#define _video_send
int  _video_send_print_local_data(NiceAgent *agent, guint stream_id,
guint component_id);

int  _video_send_parse_remote_data(NiceAgent *agent, guint stream_id,
guint component_id, char *line);

void  _video_send_cb_candidate_gathering_done(NiceAgent *agent, guint stream_id,
gpointer data);

gboolean  _video_send_stdin_remote_info_cb (GIOChannel *source, GIOCondition cond,
gpointer data);

static NiceCandidate* _video_send_parse_candidate(char *scand, guint stream_id);
void  _video_send_init_gstreamer(NiceAgent *agent, guint streamID);
void*  _video_send_main();

static void _video_send_cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
    guint len, gchar *buf, gpointer data);


#endif

#ifndef _SEND_AUDIO
#define _SEND_AUDIO

int  _send_audio_print_local_data(NiceAgent *agent, guint stream_id,
guint component_id);

int  _send_audio_parse_remote_data(NiceAgent *agent, guint stream_id,
guint component_id, char *line);

void  _send_audio_cb_candidate_gathering_done(NiceAgent *agent, guint stream_id,
gpointer data);

gboolean  _send_audio_stdin_remote_info_cb (GIOChannel *source, GIOCondition cond,
gpointer data);

static NiceCandidate* _send_audio_parse_candidate(char *scand, guint stream_id);
void  _send_audio_init_gstreamer(NiceAgent *agent, guint streamID);
void*  _send_audio_main();

static void _send_audio_cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
    guint len, gchar *buf, gpointer data);

#endif

#ifndef _TEXT_RECEIVE
#define _TEXT_RECEIVE

static int _text_receive_print_local_data(NiceAgent *agent, guint stream_id,
    guint component_id);

static int _text_receive_parse_remote_data(NiceAgent *agent, guint stream_id,
    guint component_id, char *line);

static void _text_receive_cb_candidate_gathering_done(NiceAgent *agent, guint stream_id,
    gpointer data);

static void _text_receive_cb_component_state_changed(NiceAgent *agent, guint stream_id,
    guint component_id, guint state,
    gpointer data);

static NiceCandidate* _text_receive_parse_candidate(char *scand, guint stream_id);

static void _text_receive_cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
    guint len, gchar *buf, gpointer data);

static gboolean _text_receive_stdin_remote_info_cb (GIOChannel *source, GIOCondition cond,
    gpointer data);

static void _text_receive_cb_new_selected_pair(NiceAgent *agent, guint stream_id,
    guint component_id, gchar *lfoundation,
    gchar *rfoundation, gpointer data);

static gboolean _text_receive_stdin_send_data_cb (GIOChannel *source, GIOCondition cond,
    gpointer data);

void*  _text_receive_main();

#endif


#ifndef _RECEIVE_AUDIO
#define _RECEIVE_AUDIO

void*  _audio_receive_main();
void  _audio_receive_init_gstreamer(NiceAgent *magent, guint stream_id);
void  _audio_receive_cb_candidate_gathering_done(NiceAgent *agent, guint stream_id, gpointer data);
int  _audio_receive_print_local_data(NiceAgent *agent, guint stream_id, guint component_id);
int  _audio_receive_parse_remote_data(NiceAgent *agent, guint streamID,
    guint component_id, char *line);
NiceCandidate* _audio_receive_parse_candidate(char *scand, guint streamID);
GThread *sendToAndroid;
static int send_data_to_android(NiceAgent *agent);

#endif
