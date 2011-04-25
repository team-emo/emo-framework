#include "JavaGlue.h"
#include "Constants.h"
#include "Runtime.h"
#include "VmFunc.h"
#include "Engine.h"

extern emo::Engine* engine;

void initJavaGlueFunctions() {
    engine->registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "nativeEcho",   emoJavaEcho);

    engine->registerClass(engine->sqvm, EMO_NET_CLASS);
    engine->registerClassFunc(engine->sqvm, EMO_NET_CLASS, "request",   emoJavaAsyncHttpRequest);
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

    void JavaGlue::asyncHttpRequest(std::string name, jint timeout, std::string url, std::string method, kvs_t* params) {
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

SQInteger emoJavaAsyncHttpRequest(HSQUIRRELVM v) {
    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    const SQChar* url;
    const SQChar* method;
    SQInteger timeout = 5000;

    if (nargs >= 3 && sq_gettype(v, 3) == OT_STRING) {
        sq_tostring(v, 3);
        sq_getstring(v, -1, &url);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    if (nargs >= 4 && sq_gettype(v, 4) == OT_STRING) {
        sq_tostring(v, 4);
        sq_getstring(v, -1, &method);
        sq_poptop(v);
    } else {
        method = "GET";
    }

    if (nargs >= 5 && sq_gettype(v, 5) == OT_INTEGER) {
        sq_getinteger(v, 5, &timeout);
    }

    kvs_t* params = new kvs_t();
    std::string param_key;
    for(SQInteger n = 6; n <= nargs; n++) {

        const SQChar* param;
        sq_tostring(v, n);
        sq_getstring(v, -1, &param);
        sq_poptop(v);

        if (n % 2 == 0) {
            param_key = param;
        } else if (!param_key.empty()) {
            params->insert(std::make_pair(param_key, param));
            param_key.clear();
        }
    }

	if (url != NULL) {
        engine->javaGlue->asyncHttpRequest(name, timeout, url, method, params);
	}
	

    delete params;

    sq_pushinteger(v, EMO_NO_ERROR);
	return 1;
}

