LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := samp
LOCAL_LDLIBS := -llog -lGLESv2

# samp
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/net/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/util/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/RW/RenderWare.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/gui/*.cpp)

# vendor
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/ini/config.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/inih/cpp/INIReader.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/inih/ini.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/SAMP/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/imgui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/hash/md5.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/simpleHttp/*.cpp)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_CPPFLAGS := -w -s -fvisibility=hidden -pthread -Wall -fpack-struct=1 -Os -std=c++14 -fexceptions -fdata-sections -ffunction-sections
#LOCAL_CPPFLAGS := -g -rdynamic -funwind-tables -w -pthread -Wall -O2 -std=c++14 -fno-omit-frame-pointer
# debug
#LOCAL_CPPFLAGS := -g -rdynamic -funwind-tables -w -pthread -Wall -O2 -std=c++14 -fno-omit-frame-pointer -funwind-tables
include $(BUILD_SHARED_LIBRARY)