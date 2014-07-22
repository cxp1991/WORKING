LOCAL_PATH := $(call my-dir)

# Libnice API
include $(CLEAR_VARS)
LOCAL_MODULE := lib_nice
LOCAL_SRC_FILES := libnice.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOGIN := Login
GST_LIBNICE := GstLibnice
STREAM := Stream
LOCAL_MODULE := Main
UTILS := utils

UTILS_SRC_FILES := $(UTILS)/base64.c
LOGIN_SRC_FILES := $(LOGIN)/login.c
GST_LIBNICE_SRC_FILES := $(GST_LIBNICE)/gstnice.c $(GST_LIBNICE)/gstnicesink.c $(GST_LIBNICE)/gstnicesrc.c
STREAM_SRC_FILES := $(STREAM)/receive_audio.c $(STREAM)/controller.c $(STREAM)/receive_video.c \
					$(STREAM)/send_audio.c $(STREAM)/stream.c $(STREAM)/libnice_initialize.c

LOCAL_SRC_FILES :=  Main.c $(LOGIN_SRC_FILES) $(GST_LIBNICE_SRC_FILES) $(STREAM_SRC_FILES) $(UTILS_SRC_FILES)
					
LOCAL_SHARED_LIBRARIES := gstreamer_android
LOCAL_STATIC_LIBRARIES := lib_nice
LOCAL_LDLIBS := -llog -landroid
LOCAL_CFLAGS := -I/home/cxphong/Desktop/build_glib/Android_System/sysroot/usr/include/nice
GSTREAMER_SDK_ROOT_ANDROID=/home/cxphong/adt-bundle-linux-x86-20131030/gstreamer-sdk-android-arm-release-2013.6

include $(BUILD_SHARED_LIBRARY)

ifndef GSTREAMER_SDK_ROOT
ifndef GSTREAMER_SDK_ROOT_ANDROID
$(error GSTREAMER_SDK_ROOT_ANDROID is not defined!)
endif
GSTREAMER_SDK_ROOT := $(GSTREAMER_SDK_ROOT_ANDROID)
endif
GSTREAMER_NDK_BUILD_PATH := $(GSTREAMER_SDK_ROOT)/share/gst-android/ndk-build/
include $(GSTREAMER_NDK_BUILD_PATH)/plugins.mk
GSTREAMER_PLUGINS := $(GSTREAMER_PLUGINS_PLAYBACK) $(GSTREAMER_PLUGINS_CORE) $(GSTREAMER_PLUGINS_SYS) $(GSTREAMER_PLUGINS_EFFECTS) $(GSTREAMER_PLUGINS_NET) $(GSTREAMER_PLUGINS_CODECS_RESTRICTED) $(GSTREAMER_PLUGINS_CODECS)
GSTREAMER_EXTRA_DEPS := gstreamer-interfaces-0.10 gstreamer-video-0.10
include $(GSTREAMER_NDK_BUILD_PATH)/gstreamer.mk

