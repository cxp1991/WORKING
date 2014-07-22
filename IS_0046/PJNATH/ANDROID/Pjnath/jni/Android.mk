LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

PJDIR = /home/cxphong/Desktop/pjproject-2.2.1

# Get PJ build settings
include /home/cxphong/Desktop/pjproject-2.2.1/build/common.mak
include /home/cxphong/Desktop/pjproject-2.2.1/build.mak

LOCAL_SRC_FILES := jni_init.c icedemo_android.c login.c base64.c
LOCAL_MODULE := icedemo

LOCAL_LDLIBS := -llog -landroid $(APP_LDLIBS)
LOCAL_LDFLAGS   := $(APP_LDFLAGS)
LOCAL_STATIC_LIBRARIES := lib_pjnath

LOCAL_C_INCLUDES := $(LOCAL_PATH)/pjnath_output/include 
LOCAL_CFLAGS := -Werror $(APP_CFLAGS) -frtti
 
include $(BUILD_SHARED_LIBRARY)


