#ifndef EMO_JAVAGLUE_H
#define EMO_JAVAGLUE_H

#include <jni.h>
#include <string>
#include <squirrel.h>
#include "Types.h"

void initJavaGlueFunctions();

/*
 * Class:     com_emo_framework_EmoActivity
 * Method:    callback
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_emo_1framework_EmoActivity_callback
  (JNIEnv *, jobject, jstring, jstring);

namespace emo {
    class JavaGlue {
    public:
        std::string echo(std::string echo);
        void asyncHttpGetRequest(std::string name, jint timeout, std::string url, std::string method, kvs_t* params);
        bool registerJavaGlue();
        int registerNativeMethods(JNIEnv* env, JNINativeMethod* gMethods, int numMethods);
    };
}

SQInteger emoJavaEcho(HSQUIRRELVM v);
#endif
