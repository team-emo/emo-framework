#include "JavaGlue.h"
#include "Constants.h"
#include "Runtime.h"
#include "VmFunc.h"
#include "Engine.h"

extern emo::Engine* engine;

void initJavaGlueFunctions() {
    engine->registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "nativeEcho",   emoJavaEcho);
}

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

namespace emo {
    std::string JavaGlue::echo(std::string echo) {
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

    void JavaGlue::asyncHttpGetRequest(std::string name, jint timeout, std::string url, std::string method, kvs_t* params) {
        int count = (params->size() * 2) + 2;

        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;

        vm->AttachCurrentThread(&env, NULL);

        jobjectArray jstrArray= (jobjectArray)env->NewObjectArray(count,
             env->FindClass("java/lang/String"),
             env->NewStringUTF(""));

        env->SetObjectArrayElement(jstrArray, 0, env->NewStringUTF(url.c_str()));
        env->SetObjectArrayElement(jstrArray, 1, env->NewStringUTF(method.c_str()));

        int i = 2;
        kvs_t::iterator iter;
        for(iter = params->begin(); iter != params->end(); iter++) {
            env->SetObjectArrayElement(jstrArray, i, env->NewStringUTF(iter->first.c_str()));
            i++;
            env->SetObjectArrayElement(jstrArray, i, env->NewStringUTF(iter->second.c_str()));
            i++;
        }

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, "asyncHttpRequest", "(Ljava/lang/String;I[Ljava/lang/String;)V");
        env->CallVoidMethod(engine->app->activity->clazz, methodj,  env->NewStringUTF(name.c_str()), timeout, jstrArray);

        vm->DetachCurrentThread();
    }


    int JavaGlue::registerNativeMethods(JNIEnv* env, 
                              JNINativeMethod* gMethods, int numMethods) {

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);

        if (clazz == NULL) {
            return JNI_FALSE;
        } else  if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
            return JNI_FALSE;
        }

        return JNI_TRUE;

    }

    bool JavaGlue::registerJavaGlue() {

        JNINativeMethod methods[] = {
            { "callback", "(Ljava/lang/String;Ljava/lang/String;)V",
                  (void *)Java_com_emo_1framework_EmoActivity_callback }

        };

        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;
        vm->AttachCurrentThread(&env, NULL);

        int result = registerNativeMethods(env, 
                             methods, sizeof(methods) / sizeof(methods[0]));

        vm->DetachCurrentThread();

        return result == JNI_TRUE;
    }
}


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
        std::string value = engine->javaGlue->echo(str);
		sq_pushstring(v, value.c_str(), -1);
	}
	
	return 1;
}

