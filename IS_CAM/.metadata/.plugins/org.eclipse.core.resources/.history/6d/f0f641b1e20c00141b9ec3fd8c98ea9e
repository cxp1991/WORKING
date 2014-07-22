#include "stream.h"

void* _check_connection()
{
	printf("[check_connection]\n");
	char buffer[181] = {0};
	char receiver[181] = {0};
	char *tmp, *tmp1, *tmp2;
	int rc = 0;

	//while (text_gathering_done == FALSE) usleep(100);
	while (text_gathering_done == FALSE)
	{
		usleep(100);
	}

	printf ("[check_connection] text done!\n");
	printf ("global_socket = %d\n", global_socket);

	int ret = recv(global_socket, buffer, 181, NULL);

	if( ret != -1)// Good
	{
		rc = Base64Decode(buffer, receiver, BUFFFERLEN);
		printf("Android's username exit[Full message] = %s\n", receiver);
		tmp1 = strtok (receiver,"$");
		tmp2 = strtok (NULL,"$");
		//tmp = strtok (NULL,"$");
		//tmp2 = strtok (tmp," ");
		printf ("%s\n", tmp1);
		printf ("%s", tmp2);

		if (!strcmp(tmp2,originBuf) && !strcmp(tmp1, "005"))
		{
			printf("Android exit\n");
			g_main_loop_quit (gloop);
			return;
		}
	}
	else if (ret == -1) // Error
	{
		printf ("Receive signal Who Android phone disconnect failed!\n");
	}
}

int stream ()
{
	printf("[stream]\n");
	GThread *video_send, *send_audio, *text_receive, *audio_receive, *check_connection;

	/* Init Glib */
	g_type_init();
	gloop = g_main_loop_new(NULL, FALSE);
	io_stdin = g_io_channel_unix_new(fileno(stdin));

	/* Allocate data */
	RpiData_SendVideo = (Rpi_Data*)malloc(sizeof(Rpi_Data));
	RpiData_SendAudio = (Rpi_Data*)malloc(sizeof(Rpi_Data));
	RpiData_ReceiveAudio = (Rpi_Data*)malloc(sizeof(Rpi_Data));
	RpiData_Text = (Rpi_Data*)malloc(sizeof(Rpi_Data));

	/* Set all flag to FALSE */
	video_send_gathering_done = FALSE;
	send_audio_gathering_done = FALSE;
	receive_audio_gathering_done = FALSE;
	text_gathering_done = FALSE;

	printf("[stream]video_send_gathering_done = %d\n", video_send_gathering_done);
	printf ("		==========	STREAMING	========== \n");
	/* Init video streaming */
	video_send = g_thread_new("send video", &_video_send_main, NULL);
	send_audio = g_thread_new("send audio", &_send_audio_main, NULL);
	audio_receive = g_thread_new("receive audio", &_audio_receive_main, NULL);
	text_receive = g_thread_new("text send+receive", &_text_receive_main, NULL);
	check_connection = g_thread_new("Check if android exit or not", &_check_connection, NULL);

	/* Unalocate all object */
	g_main_loop_run (gloop);

	printf("\n[stream]g_main_loop_quit (gloop);\n");

	/* Free libnice agent & gstreamer pipeline */

	/* Free send video */
	printf("[stream] Agent = %d!\n", RpiData_SendVideo->agent);
	g_object_unref(RpiData_SendVideo->agent);
	gst_object_unref (RpiData_SendVideo->bus);
	gst_element_set_state (RpiData_SendVideo->pipeline, GST_STATE_NULL);
	gst_object_unref (RpiData_SendVideo->pipeline);

	/* Free send audio */
	g_object_unref(RpiData_SendAudio->agent);
	gst_object_unref (RpiData_SendAudio->bus);
	gst_element_set_state (RpiData_SendAudio->pipeline, GST_STATE_NULL);
	gst_object_unref (RpiData_SendAudio->pipeline);

	/* Free receive audio */
	g_object_unref(RpiData_ReceiveAudio->agent);
	gst_object_unref (RpiData_ReceiveAudio->bus);
	gst_element_set_state (RpiData_ReceiveAudio->pipeline, GST_STATE_NULL);
	gst_object_unref (RpiData_ReceiveAudio->pipeline);

	/* Free text */
	g_object_unref(RpiData_Text->agent);
	//gst_object_unref (RpiData_Text->bus);
	//gst_element_set_state (RpiData_Text->pipeline, GST_STATE_NULL);
	//gst_object_unref (RpiData_Text->pipeline);

	free(RpiData_SendVideo);
	free(RpiData_SendAudio);
	free(RpiData_ReceiveAudio);
	free(RpiData_Text);

	/* Free threads */
	g_thread_join (video_send);
	g_thread_join (send_audio);
	g_thread_join (audio_receive);
	g_thread_join (text_receive);
	g_thread_join (check_connection);
	g_main_loop_unref (gloop);

	return 0;
}
