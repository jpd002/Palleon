LOCAL_PATH := $(call my-dir)

BOOST_PATH				:= /cygdrive/c/Components/boost_trunk
FRAMEWORK_PATH			:= /cygdrive/c/Projects/Framework
PALLEON_PATH			:= /cygdrive/c/Projects/Palleon

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
LOCAL_SRC_FILES			:=	../../src/Application.cpp \
							../../src/TouchFreeCamera.cpp
LOCAL_C_INCLUDES		:= $(BOOST_PATH) $(FRAMEWORK_PATH)/include $(PALLEON_PATH)/include
LOCAL_CPP_FEATURES		:= exceptions rtti
LOCAL_LDLIBS 			:= -lz -llog -lGLESv3 -lEGL
LOCAL_STATIC_LIBRARIES	:= libPalleon libFramework

include $(BUILD_SHARED_LIBRARY)
