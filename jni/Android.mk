LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

include ../includeOpenCV.mk

ifeq ("$(wildcard $(OPENCV_MK_PATH))","")

#try to load OpenCV.mk from default install location

include $(TOOLCHAIN_PREBUILT_ROOT)/user/share/OpenCV/OpenCV.mk

else

include $(OPENCV_MK_PATH)

endif



LOCAL_MODULE    := WatermarkFilter
LOCAL_LDLIBS    := -llog
##LOCAL_STATIC_LIBRARIES := libyuv_static
 
LOCAL_SRC_FILES := WatermarkFilter.cpp \
 ScrambledMarkFilter.cpp 
 
LOCAL_MODULE_TAGS := optional
 
include $(BUILD_SHARED_LIBRARY)


