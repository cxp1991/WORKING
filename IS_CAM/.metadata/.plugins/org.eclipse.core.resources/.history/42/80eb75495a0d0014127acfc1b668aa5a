/*
 * stream.h
 *
 *  Created on: Apr 5, 2014
 *      Author: cxphong
 */

#ifndef STREAM_H_
#define STREAM_H_

#include <agent.h>
#include <gst/gst.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <gst/interfaces/xoverlay.h>
#include <gst/video/video.h>
#include <pthread.h>
#include "../GstLibnice/gstnice.h"
#include "../utils/base64.h"
#include "../Login/login.h"

#ifndef _GLOBAL_INFO
#define _GLOBAL_INFO

GST_DEBUG_CATEGORY_STATIC (debug_category);
#define GST_CAT_DEFAULT debug_category

/*
 * These macros provide a way to store the native pointer to CustomData, which might be 32 or 64 bits, into
 * a jlong, which is always 64 bits, without warnings.
 */

#if GLIB_SIZEOF_VOID_P == 8
# define GET_CUSTOM_DATA(env, thiz, fieldID) (CustomData *)(*env)->GetLongField (env, thiz, fieldID)
# define SET_CUSTOM_DATA(env, thiz, fieldID, data) (*env)->SetLongField (env, thiz, fieldID, (jlong)data)
#else
# define GET_CUSTOM_DATA(env, thiz, fieldID) (CustomData *)(jint)(*env)->GetLongField (env, thiz, fieldID)
# define SET_CUSTOM_DATA(env, thiz, fieldID, data) (*env)->SetLongField (env, thiz, fieldID, (jlong)(jint)data)
#endif


/* These global variables cache values which are not changing during execution */
static pthread_t gst_app_thread;
static pthread_key_t current_jni_env;
static jfieldID video_receive_custom_data_field_id;
static jfieldID send_audio_custom_data_field_id;
jmethodID set_message_method_id;
static jmethodID on_gstreamer_initialized_method_id;

GMainLoop *gloop;
GIOChannel* io_stdin;
jmethodID set_message_from_rpi;
jfieldID name_field_id;
gboolean *video_receive_gathering_done;
gboolean *send_audio_gathering_done;
gboolean *receive_audio_gathering_done;
gboolean *controller_gathering_done;
gchar *client_name;
gchar *rpi_name;
gboolean is_connect_to_rpi;

#define STUNSR_ADDR  "107.23.150.92"
#define STUNSR_PORT 3478
#define CONTROLLING_MODE 1
#define CONTROLLED_MODE 0

typedef struct _CustomData {
  jobject app;
  GstElement *pipeline;
  GMainContext *context;
  GMainLoop *main_loop;
  gboolean initialized;
  GstElement *video_sink;
  ANativeWindow *native_window;
  GstBus *bus;
  NiceAgent *agent;
  guint stream_id;
} CustomData;

typedef struct gst_custom_data {
	CustomData *video_receive_data;
	CustomData *receive_audio_data;
	CustomData *send_audio_data;
	CustomData *text;
}GST_CUSTOM_DATA;


#endif

#ifndef _VIDEO_RECEIVE
#define _VIDEO_RECEIVE
int  _video_receive_print_local_data(NiceAgent *agent, guint stream_id,
guint component_id);

int  _video_receive_parse_remote_data(NiceAgent *agent, guint stream_id,
guint component_id, char *line);

void  _video_receive_cb_candidate_gathering_done(NiceAgent *agent, guint stream_id,
gpointer data);

gboolean  _video_receive_stdin_remote_info_cb (GIOChannel *source, GIOCondition cond,
gpointer data);

static NiceCandidate* _video_receive_parse_candidate(char *scand, guint stream_id);
void  _video_receive_init_gstreamer(NiceAgent *agent, guint streamID, CustomData *data);
void*  _video_receive_main(CustomData *data);

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

void*  controller (CustomData *data);

void gst_native_send_text (JNIEnv* env, jobject thiz, jstring text);
void gst_native_init_receive_text (JNIEnv* env, jobject obj);
int  _send_audio_parse_remote_data(NiceAgent *agent, guint streamID,
    guint component_id, char *line);
static NiceCandidate* _send_audio_parse_candidate(char *scand, guint streamID);
void rotate_servo (JNIEnv* env, jobject thiz, jint direction);
void getTemperature (JNIEnv* env, jobject thiz);
void controlPiezosiren(JNIEnv* env, jobject thiz, jint status);
void pumpController (JNIEnv* env, jobject thiz, jint status);

#endif


#ifndef _SEND_AUDIO
#define _SEND_AUDIO

void*  _send_audio_main(CustomData *data);
void  _send_audio_init_gstreamer(NiceAgent *magent, guint streamID, CustomData *data);
void  _send_audio_cb_candidate_gathering_done(NiceAgent *agent, guint stream_id, gpointer data);
int  _send_audio_print_local_data(NiceAgent *agent, guint stream_id, guint component_id);
NiceCandidate* _send_audio_parse_candidate(char *scand, guint streamID);
void _send_audio_cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
    guint len, gchar *buf, gpointer data);
void _send_audio_cb_new_selected_pair(NiceAgent *agent, guint stream_id,
    guint component_id, gchar *lfoundation,
    gchar *rfoundation, gpointer data);

#endif

#ifndef _RECEIVE_AUDIO
#define _RECEIVE_AUDIO

int  _receive_audio_print_local_data(NiceAgent *agent, guint stream_id,
guint component_id);

int  _receive_audio_parse_remote_data(NiceAgent *agent, guint stream_id,
guint component_id, char *line);

void  _receive_audio_cb_candidate_gathering_done(NiceAgent *agent, guint stream_id,
gpointer data);

gboolean  _receive_audio_stdin_remote_info_cb (GIOChannel *source, GIOCondition cond,
gpointer data);

static NiceCandidate* _receive_audio_parse_candidate(char *scand, guint stream_id);
void  _receive_audio_init_gstreamer(NiceAgent *agent, guint streamID, CustomData *data);
void*  _receive_audio_main();

static void _receive_audio_cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
    guint len, gchar *buf, gpointer data);

#endif

// jni methods
JNIEnv *attach_current_thread (void);
void detach_current_thread (void *env);
static void set_ui_message (const gchar *message, CustomData *data);
void error_cb (GstBus *bus, GstMessage *msg, CustomData *data);
void state_changed_cb (GstBus *bus, GstMessage *msg, CustomData *data) ;
void check_initialization_complete (CustomData *data);
void *app_function (void *userdata);
void gst_native_init (JNIEnv* env, jobject thiz, jstring _client_name, jstring _rpi_name);
void gst_native_finalize (JNIEnv* env, jobject thiz);
void gst_native_play (JNIEnv* env, jobject thiz);
void gst_native_pause (JNIEnv* env, jobject thiz);
jboolean gst_native_class_init (JNIEnv* env, jclass klass);
void gst_native_surface_init (JNIEnv *env, jobject thiz, jobject surface);
void gst_native_surface_finalize (JNIEnv *env, jobject thiz);
void close_server_socket (JNIEnv *env, jobject thiz);
void exit_streaming (JNIEnv *env, jobject thiz);

#endif /* STREAM_H_ */
