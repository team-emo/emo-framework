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
include $(LOCAL_PATH)/emo/Android.mk
include $(LOCAL_PATH)/squirrel/Android.mk
include $(LOCAL_PATH)/libpng/Android.mk
include $(LOCAL_PATH)/Box2D/Android.mk
include $(LOCAL_PATH)/rapidxml/Android.mk
include $(LOCAL_PATH)/sqlite/Android.mk
include $(LOCAL_PATH)/aes/Android.mk

APP_OPTIM              := release
LOCAL_MODULE           := emo-android
LOCAL_ARM_MODE         := arm
LOCAL_LDLIBS           := -llog -landroid -lEGL -lGLESv1_CM -lOpenSLES -lz -lGLESv2 $(SQLITE_LDLIBS)
LOCAL_SHARED_LIBRARIES := $(SQLITE_SHARED_LIBRARIES) $(SQLCIPHER_SHARED_LIBRARIES)
LOCAL_CFLAGS           := $(SQUIRREL_CFLAGS) $(SQLITE_CFLAGS)
LOCAL_C_INCLUDES       := $(EMO_C_INCLUDES) $(SQUIRREL_C_INCLUDES) $(LIBPNG_C_INCLUDES) $(BOX2D_C_INCLUDES) $(SQLITE_C_INCLUDES) $(AES_C_INCLUDES)
LOCAL_SRC_FILES        := native_app_glue.c main.cpp $(EMO_SRC_FILES) $(SQUIRREL_SRC_FILES) $(LIBPNG_SRC_FILES) $(BOX2D_SRC_FILES) $(SQLITE_SRC_FILES) $(AES_SRC_FILES)

include $(BUILD_SHARED_LIBRARY)
