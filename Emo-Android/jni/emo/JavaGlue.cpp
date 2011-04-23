#include "JavaGlue.h"
#include "Constants.h"
#include "Engine.h"
#include "Runtime.h"

extern emo::Engine* engine;

/*
 * echo method for jni testing.
 */
std::string javaEcho(std::string value) {
    std::string echoValue;

    JNIEnv* env;
    JavaVM* vm = engine->app->activity->vm;
        
    vm->AttachCurrentThread(&env, NULL);

    jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
    jmethodID methodj = env->GetMethodID(clazz, "echo", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring jstr = (jstring)env->CallObjectMethod(engine->app->activity->clazz, methodj, env->NewStringUTF(value.c_str()));
    if (jstr != NULL) {
        const char* str = env->GetStringUTFChars(jstr, NULL);
        echoValue = str;
        env->ReleaseStringUTFChars(jstr, str);
    }

    vm->DetachCurrentThread();

    return echoValue;
}
