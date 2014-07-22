#include "stream.h"
#include "../Login/login.h"

#define PIEZO_COMMAND 0x01
#define PUMP_COMMAND  0x02
#define SERVO_COMMAND 0x03
#define SERVO_01 1
#define SERVO_02 2
#define DEGREE_PER_ROTATE 8
#define GETTEMP_COMMAND 0x05

CustomData* mData;

void controller_receiver (NiceAgent *agent,
						   guint stream_id,
						   guint component_id,
						   guint len,
						   gchar *buf,
						   gpointer data);
/*
 * 1. Create agent
 * 2. Create stream_id
 * 3. Set receiver
 * 4. Wait until send_audio done
 * 5. Start gathering candidate
 */
void*  controller (CustomData *data)
{
	// Use for later
	mData = data;

	// Create the nice agent
	data->agent = libnice_create_NiceAgent_without_gstreamer(controller_gathering_done,
												 	 		 data->context);

	// Create a new stream with one component
	data->stream_id = libnice_create_stream_id (data->agent);

	// Set receiver function
	set_receiver (data->agent, data->stream_id, data->context);

	// Wait until send_audio done
	while(*send_audio_gathering_done == FALSE)
		usleep(100);

	__android_log_print (ANDROID_LOG_INFO, "tutorial-3", "start controller");

	// Start gathering candidates
	libnice_start_gather_candidate (data->agent,
									data->stream_id,
									data->context);
}


void set_receiver (	NiceAgent *agent,
					guint stream_id,
					GMainContext *context)
{
	nice_agent_attach_recv(	agent,
							stream_id,
							1,
							context,
							controller_receiver,
							NULL);
}
/*
 * Servo Controller
 */
void rotate_servo (JNIEnv* env, jobject thiz, jint direction)
{
	// id [1], servo id[1,2], direction[+,-], dgree, end-of-string
	 //__android_log_print (ANDROID_LOG_DEBUG, "tutorial-3", "Send servo ... !");
	/*
	 * If Transmit component doesn't done.
	 *  -> Exit
	 */
	if (controller_gathering_done == FALSE)
		return;

	gchar *command = (gchar*)malloc(sizeof(gchar)*5);

	/*
	 * Build Command
	 */

	/*
	 * Command ID
	 */
	command[0] = SERVO_COMMAND;

	/*
	 *  Servo number
	 */
	if (direction == 0 || direction == 1 )
		command[1] = SERVO_01;
	else if (direction == 2 || direction == 3 )
		command[1] = SERVO_02;

	/*
	 *  Direction
	 */
	if (direction == 0 || direction == 2) // right to left, top to bottom
		command[2] = '+';
	if (direction == 1 || direction == 3) // left to right, bottom to top
		command[2] = '-';

	/*
	 *  Degree
	 */
	command[3] = DEGREE_PER_ROTATE;
	command[4] = '\0';

	/*
	 *  Send command to RPI
	 */
	nice_agent_send(mData->agent, mData->stream_id, 1, sizeof(command), command);

	// __android_log_print (ANDROID_LOG_DEBUG, "tutorial-3", "Send servo done!");
}

/*
 *  Temperature Controller
 */
void getTemperature (JNIEnv* env, jobject thiz)
{
	/*
	 * If Transmit component doesn't done.
	 *  -> Exit
	 */
	if (controller_gathering_done == FALSE)
		return;

	//__android_log_print (ANDROID_LOG_DEBUG, "tutorial-3", "Send to get Temp ... !");
	gchar *command = (gchar*)malloc(sizeof(gchar)*5);

	/*
	 * Build Command
	 */
	command[0] = GETTEMP_COMMAND;
	command[1] = 0xff;
	command[2] = 0xff;
	command[3] = 0xff;
	command[4] = '\0';

	/*
	 * Send command to RPI
	 */
	nice_agent_send(mData->agent, mData->stream_id, 1, sizeof(command), command);
	//__android_log_print (ANDROID_LOG_DEBUG, "tutorial-3", "Send to get temp done!");
}

/*
 * Piezosiren controller
 */
void controlPiezosiren(JNIEnv* env, jobject thiz, jint status)
{
	//__android_log_print (ANDROID_LOG_DEBUG, "tutorial-3", "Send to control piezosiren !");

	/*
	 * If Transmit component doesn't done.
	 *  -> Exit
	 */
	if (controller_gathering_done == FALSE)
		return;

	gchar *command = (gchar*) malloc(sizeof(gchar)*5);

	/*
	 *  Build command
	 */
	command[0] = PIEZO_COMMAND;
	command[1] = status;
	command[2] = 0xff;
	command[3] = 0xff;
	command[4] = '\0';

	/*
	 *  Send command to RPI
	 */
	nice_agent_send(mData->agent, mData->stream_id, 1, sizeof(command), command);
	//__android_log_print (ANDROID_LOG_DEBUG, "tutorial-3", "Send to control piezosiren done... !");
}

/**
 *  Pump controller
 */
void pumpController (JNIEnv* env, jobject thiz, jint status)
{
	/*
	 * If Transmit component doesn't done.
	 *  -> Exit
	 */
	if (controller_gathering_done == FALSE)
		return;

	gchar *command = (gchar*) malloc(sizeof(gchar)*5);

	/*
	 * Build command
	 */
	command[0] = PUMP_COMMAND;
	command[1] = status;
	command[2] = 0xff;
	command[3] = 0xff;
	command[4] = '\0';

	/*
	 * Send command to RPI
	 */
	nice_agent_send(mData->agent, mData->stream_id, 1, sizeof(command), command);
	//__android_log_print (ANDROID_LOG_DEBUG, "tutorial-3", "Send to control pumb done... !");
}

void controller_receiver (	NiceAgent *agent,
							guint stream_id,
							guint component_id,
							guint len,
							gchar *buf,
							gpointer data)
{
	JNIEnv *env = get_jni_env ();
	char message[5]={0};
	jstring jmessage;
	int i;

	if (len == 1 && buf[0] == '\0')
		g_main_loop_quit (mData->main_loop);

	for (i = 0; i < 4; i++)
		__android_log_print (ANDROID_LOG_ERROR, "tutorial-3", "%d_", buf[i]);

	switch(buf[0])
	{
		case GETTEMP_COMMAND:
			sprintf(message, "%d", buf[1]);
			jmessage = (*env)->NewStringUTF(env, message);
			(*env)->CallVoidMethod (env, mData->app, set_message_method_id, jmessage);
			if ((*env)->ExceptionCheck (env))
			{
				(*env)->ExceptionDescribe(env);
				    (*env)->ExceptionClear (env);
			}
			(*env)->DeleteLocalRef (env, jmessage);
			break;
		default:
			break;
	}
}
