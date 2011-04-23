#include "JavaGlue.h"
#include "Constants.h"
#include "Engine.h"
#include "Runtime.h"
#include "VmFunc.h"

extern emo::Engine* engine;

/*
 * Class:     com_emo_framework_EmoActivity
 * Method:    callback
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_emo_1framework_EmoActivity_callback
              (JNIEnv* env, jobject thiz, jstring jname, jstring jvalue) {

    char* name;
    char* value;

    const char* cname = env->GetStringUTFChars(jname, NULL);
    name = strdup(cname);
    env->ReleaseStringUTFChars(jname, cname);

    const char* cvalue = env->GetStringUTFChars(jvalue, NULL);
    value = strdup(cvalue);
    env->ReleaseStringUTFChars(jvalue, cvalue);

    callSqFunction_Bool_TwoStrings(engine->sqvm, EMO_NAMESPACE, EMO_FUNC_ONCALLBACK, name, value, false);

    free(name);
    free(value);
}

std::string javaEcho(std::string echo) {
    std::string echoValue;

    JNIEnv* env;
    JavaVM* vm = engine->app->activity->vm;

    vm->AttachCurrentThread(&env, NULL);

    jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
    jmethodID methodj = env->GetMethodID(clazz, "echo", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring jstr = (jstring)env->CallObjectMethod(engine->app->activity->clazz, methodj,  env->NewStringUTF(echo.c_str()));
    if (jstr != NULL) {
        const char* str = env->GetStringUTFChars(jstr, NULL);
        echoValue = str;
        env->ReleaseStringUTFChars(jstr, str);
    }
    vm->DetachCurrentThread();

    return echoValue;
}

static int registerNativeMethods(JNIEnv* env, 
                              JNINativeMethod* gMethods, int numMethods) {

    jclass clazz = env->GetObjectClass(engine->app->activity->clazz);

    if (clazz == NULL) {
        return JNI_FALSE;
    } else  if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;

}

static JNINativeMethod methods[] = {

    { "callback", "(Ljava/lang/String;Ljava/lang/String;)V",
                  (void *)Java_com_emo_1framework_EmoActivity_callback }

};

bool registerJavaGlue() {

    JNIEnv* env;
    JavaVM* vm = engine->app->activity->vm;
    vm->AttachCurrentThread(&env, NULL);

    int result = registerNativeMethods(env, 
                             methods, sizeof(methods) / sizeof(methods[0]));

    vm->DetachCurrentThread();

    return result == JNI_TRUE;
}

void initJavaGlueFunctions() {
    engine->registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "nativeEcho",   emoJavaEcho);
}

/*
 * echo using jni
 */
SQInteger emoJavaEcho(HSQUIRRELVM v) {
	const SQChar *str;
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
    	if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);
            sq_poptop(v);
    	}
    }
	
	if (str != NULL) {
        std::string value = javaEcho(str);
		sq_pushstring(v, value.c_str(), -1);
	}
	
	return 1;
}

