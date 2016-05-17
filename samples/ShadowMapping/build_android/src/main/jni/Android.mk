LOCAL_PATH := $(call my-dir)

FRAMEWORK_PATH := $(realpath $(LOCAL_PATH))/../../../../../../../Framework
PALLEON_PATH   := $(realpath $(LOCAL_PATH))/../../../../../../
PROJECT_PATH   := $(realpath $(LOCAL_PATH))/../../../../

include $(PROJECT_PATH)/../ExternalDependencies.mk

include $(CLEAR_VARS)

LOCAL_MODULE			:= libFramework
LOCAL_SRC_FILES 		:= $(FRAMEWORK_PATH)/build_android/obj/local/$(TARGET_ARCH_ABI)/libFramework.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE			:= libPalleon
LOCAL_SRC_FILES 		:= $(PALLEON_PATH)/build_android/obj/local/$(TARGET_ARCH_ABI)/libPalleon.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE			:= libPalleonApplication
LOCAL_SRC_FILES			:=	$(PROJECT_PATH)/src/Application.cpp \
							$(PROJECT_PATH)/src/TouchFreeCamera.cpp
LOCAL_C_INCLUDES		:= $(BOOST_PATH) $(FRAMEWORK_PATH)/include $(PALLEON_PATH)/include
LOCAL_CPP_FEATURES		:= exceptions rtti
LOCAL_LDLIBS 			:= -lz -llog -landroid -lGLESv3 -lEGL
LOCAL_STATIC_LIBRARIES	:= libPalleon libFramework

include $(BUILD_SHARED_LIBRARY)
