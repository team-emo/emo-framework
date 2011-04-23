#ifndef EMO_JAVAGLUE_H
#define EMO_JAVAGLUE_H

#include <jni.h>
#include <string>

/*
 * Class:     com_emo_framework_EmoActivity
 * Method:    callback
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_emo_1framework_EmoActivity_callback
  (JNIEnv *, jobject, jstring, jstring);

std::string javaEcho(std::string echo);
bool registerJavaGlue();

#endif
