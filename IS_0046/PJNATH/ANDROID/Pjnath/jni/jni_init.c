#include "icedemo.h"

#define THIS_FILE "jni_init.c"

/* List of implemented native methods */
static JNINativeMethod native_methods[] =
{
  { "nativePjnathSetup", "()V", (void *) pj_setup_session}
};

jint JNI_OnLoad (JavaVM *vm, void *reserved)
{
	JNIEnv *env;

	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK)
	{
		__android_log_print (ANDROID_LOG_ERROR,
				THIS_FILE, "Could not retrieve JNIEnv");
		return 0;
	}

	jclass main_class = (*env)->FindClass (env, "com/example/pjnath/Main");

	(*env)->RegisterNatives (env, main_class,
			native_methods, 1);

	return JNI_VERSION_1_4;
}
