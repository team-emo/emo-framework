# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/squirrel/Android.mk
include $(LOCAL_PATH)/libpng/Android.mk
include $(LOCAL_PATH)/sqlite/Android.mk

SQUIRREL_CFLAGS := -O2

APP_OPTIM       := release
LOCAL_MODULE    := emo-android
LOCAL_ARM_MODE  := arm
LOCAL_SRC_FILES := main.cpp common.cpp sqfunc.cpp emo_engine.cpp  emo_engine_func.cpp emo_drawable.cpp emo_drawable_func.cpp emo_audio.cpp $(SQUIRREL_SRC_FILES) $(LIBPNG_SRC_FILES) $(SQLITE_SRC_FILES)
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM -lOpenSLES -lz
LOCAL_STATIC_LIBRARIES := android_native_app_glue
LOCAL_C_INCLUDES += $(LOCAL_PATH) $(LOCAL_PATH)/squirrel/include $(LOCAL_PATH)/libpng $(LOCAL_PATH)/sqlite
LOCAL_CFLAGS    := $(SQUIRREL_CFLAGS) $(SQLITE_CFLAGS)

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

