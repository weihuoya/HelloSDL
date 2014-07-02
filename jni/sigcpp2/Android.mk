LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libsigc++

# Add your application source files here...
LOCAL_SRC_FILES := \
    $(LOCAL_PATH)/sigc++/connection.cc \
    $(LOCAL_PATH)/sigc++/adaptors/lambda/lambda.cc \
    $(LOCAL_PATH)/sigc++/signal.cc \
    $(LOCAL_PATH)/sigc++/signal_base.cc \
    $(LOCAL_PATH)/sigc++/functors/slot.cc \
    $(LOCAL_PATH)/sigc++/functors/slot_base.cc \
    $(LOCAL_PATH)/sigc++/trackable.cc

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

include $(BUILD_SHARED_LIBRARY)
