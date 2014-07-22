#include "Login/login.h"
#include "Stream/stream.h"

static JavaVM *java_vm;

JNIEnv *attach_current_thread (void)
{
  JNIEnv *env;
  JavaVMAttachArgs args;

  GST_DEBUG ("Attaching thread %p", g_thread_self ());
  args.version = JNI_VERSION_1_4;
  args.name = NULL;
  args.group = NULL;
  __android_log_print (ANDROID_LOG_INFO, "tutorial-3", "java_vm = %d, env = %d, args = %d", java_vm, env, args);
  if ((*java_vm)->AttachCurrentThread (java_vm, &env, &args) < 0)
  {
	GST_ERROR ("Failed to attach current thread");
	return NULL;
  }

  return env;
}

/*
 *  Unregister this thread from the VM
 */
void detach_current_thread (void *env)
{
  GST_DEBUG ("Detaching thread %p", g_thread_self ());
  (*java_vm)->DetachCurrentThread (java_vm);
}

/* Retrieve the JNI environment for this thread */
JNIEnv *get_jni_env (void)
{
  JNIEnv *env;
  if ((env = pthread_getspecific (current_jni_env)) == NULL)
  {
    env = attach_current_thread ();
    pthread_setspecific (current_jni_env, env);
  }

  return env;
}

/* List of implemented native methods */
static JNINativeMethod native_methods[] =
{
  { "nativeInit", "(Ljava/lang/String;Ljava/lang/String;)V", (void *) gst_native_init},
  { "nativeFinalize", "()V", (void *) gst_native_finalize},
  { "nativePlay", "()V", (void *) gst_native_play},
  { "nativePause", "()V", (void *) gst_native_pause},
  { "nativeSurfaceInit", "(Ljava/lang/Object;)V", (void *) gst_native_surface_init},
  { "nativeSurfaceFinalize", "()V", (void *) gst_native_surface_finalize},
  { "nativeClassInit", "()Z", (void *) gst_native_class_init},
  { "native_request_servo_rotate", "(I)V", (void *) rotate_servo},
  { "native_get_temperature", "()V", (void *) getTemperature},
  { "native_control_piezo", "(I)V", (void *) controlPiezosiren},
  { "native_pump_controller", "(I)V", (void *) pumpController},
  { "nativeExitStreaming", "()V", (void *) exit_streaming}
};

///* List of implemented native methods */
//static JNINativeMethod lnative_methods[] =
//{
//		{ "nativeSendText", "(Ljava/lang/String;)V", (void *) gst_native_send_text}
//};

static JNINativeMethod login_methods[] =
{
		{ "nativeLogin", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)login_to_server }

};

static JNINativeMethod client_state_methods[] =
{
		{ "nativeListOnlineClient", "(Ljava/lang/String;)Ljava/lang/String;", (void *)list_online_client},
		{ "nativeCloseSocket", "()V", (void *) close_server_socket }
};

/* Library initializer */
jint JNI_OnLoad (JavaVM *vm, void *reserved)
{
	__android_log_print (ANDROID_LOG_ERROR, "tutorial-3", " JNI_OnLoad");
	JNIEnv *env = NULL;

	java_vm = vm;
	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK)
	{
		__android_log_print (ANDROID_LOG_ERROR, "tutorial-3", "Could not retrieve JNIEnv");
		return 0;
	}

	jclass main_class = (*env)->FindClass (env, "com/gst_sdk_tutorials/tutorial_3/Tutorial3");
	//jclass llass = (*env)->FindClass (env, "com/gst_sdk_tutorials/tutorial_3/Communicate_Rpi");
	jclass login_class = (*env)->FindClass (env, "com/gst_sdk_tutorials/tutorial_3/Login");
	jclass client_state_class = (*env)->FindClass (env, "com/gst_sdk_tutorials/tutorial_3/ClientState");

	(*env)->RegisterNatives (env, main_class, native_methods, G_N_ELEMENTS(native_methods));
	//(*env)->RegisterNatives (env, llass, lnative_methods, G_N_ELEMENTS(lnative_methods));
	(*env)->RegisterNatives (env, login_class, login_methods, G_N_ELEMENTS(login_methods));
	(*env)->RegisterNatives (env, client_state_class, client_state_methods, G_N_ELEMENTS(client_state_methods));

	pthread_key_create (&current_jni_env, detach_current_thread);
	return JNI_VERSION_1_4;
}
