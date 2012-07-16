// Copyright (c) 2011 emo-framework project
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the project nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
#include "JavaGlue.h"
#include "Constants.h"
#include "Runtime.h"
#include "VmFunc.h"
#include "Engine.h"
#include "SquirrelGlue.h"

using namespace std;

extern emo::Engine* engine;

void initJavaGlueFunctions() {
    registerClassFunc(engine->sqvm, EMO_RUNTIME_CLASS, "nativeEcho",   emoJavaEcho);

    registerClass(engine->sqvm, EMO_NET_CLASS);
    registerClassFunc(engine->sqvm, EMO_NET_CLASS, "request",   emoJavaAsyncHttpRequest);
}

// JNI function with C++ needs to be surrounded by these lines below
extern "C" {

/*
 * Class:     com_emo_framework_EmoActivity
 * Method:    checkNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_emo_1framework_EmoActivity_checkNative
  (JNIEnv *, jclass){
    LOGI("checkNative : OK");
}

/*
 * Class:     com_emo_framework_EmoActivity
 * Method:    callback
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_emo_1framework_EmoActivity_callback
  (JNIEnv* env, jobject thiz, jstring jname, jstring jvalue, jstring jerrCode, jstring jerrMsg) {

    char* name = getCStringFromJava(env, jname);
    char* value = getCStringFromJava(env, jvalue);
    char* errCode = getCStringFromJava(env, jerrCode);
    char* errMsg = getCStringFromJava(env, jerrMsg);

    callSqFunction_Bool_Strings(engine->sqvm, EMO_NAMESPACE, EMO_FUNC_ONCALLBACK, name, value, errCode, errMsg, false);

    free(name);
    free(value);
    free(errCode);
    free(errMsg);
}

/*
 * Class:     com_emo_framework_EmoActivity
 * Method:    onReturnToSquirrel
 * Signature: (IILjava/lang/String;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_emo_1framework_EmoActivity_onReturnToSquirrel
  (JNIEnv* env, jobject thiz, jint requestCode, jint resultCode, jstring jThisClassName, jobjectArray extras){

    // Set default VM
    Sqrat::DefaultVM::Set(engine->sqvm);

    // Build extra table
    Sqrat::Table* extraTable = new Sqrat::Table();
    jint extraSize = env->GetArrayLength(extras);

    for(int i = 0; i < extraSize ; i+=2){
        jstring jExtraKey = (jstring) env->GetObjectArrayElement(extras, i);
        string extraKeyWithPrefix = string( getCStringFromJava(env, jExtraKey) );
        string extraKey = extraKeyWithPrefix.substr(EXTRA_PREFIX_SIZE - 1);

        jstring jExtraValue = (jstring) env->GetObjectArrayElement(extras, i + 1);
        string extraValueString = string( getCStringFromJava(env, jExtraValue) );

        // check "int" parameter
        if(extraKeyWithPrefix.find("emo_prefix_int_") != string::npos ){
            int extraValue = atoi(extraValueString.c_str());
            extraTable->SetValue(extraKey.c_str(), extraValue);

        // check "float" parameter
        }else if(extraKeyWithPrefix.find("emo_prefix_flo_") != string::npos ){
            float extraValue = atof(extraValueString.c_str());
            extraTable->SetValue(extraKey.c_str(), extraValue);

        // check "bool" parameter
        }else if(extraKeyWithPrefix.find("emo_prefix_boo_") != string::npos ){
            bool extraValue = false;
            if(extraValueString.find("TRUE") != string::npos || extraValueString.find("true") != string::npos) extraValue = true;
            extraTable->SetValue(extraKey.c_str(), extraValue);

        // check "String Array" prefix
        }else if(extraKeyWithPrefix.find("emo_prefix_ars_") != string::npos ){
            vector<string> extraValueVector = split( extraValueString, string("|") );

            Sqrat::Array extraStringArray = Sqrat::Array();
            for(unsigned int i = 0; i < extraValueVector.size() ; i++){
                extraStringArray.Append(extraValueVector[i]);
            }
            extraTable->Bind(extraKey.c_str(), extraStringArray);

        // check "Int Array" prefix
        }else if(extraKeyWithPrefix.find("emo_prefix_ari_") != string::npos ){
            vector<string> extraValueVector = split( extraValueString, string("|") );

            Sqrat::Array extraIntArray = Sqrat::Array();
            for(unsigned int i = 0; i < extraValueVector.size() ; i++){
                extraIntArray.Append(atoi(extraValueVector[i].c_str()));
            }
            extraTable->Bind(extraKey.c_str(), extraIntArray);

        // put parameter as a "String"
        }else{
            extraTable->SetValue(extraKey.c_str(), extraValueString);

        }
    }

    Sqrat::Object returnobj = Sqrat::RootTable().GetSlot(EMO_NAMESPACE).GetSlot("_onReturn");
    if(returnobj.IsNull()){
        LOGI("onReturnToSquirrel : Function \"_onReturn\" does not exist on the Squirrel code");
    }

    Sqrat::Function returnFunc = returnobj.Cast<Sqrat::Function>();

    // get source class name this class name
    std::string srcClassName = engine->android->activityMap[requestCode];
    std::string thisClassName= string( getCStringFromJava(env, jThisClassName) );

    engine->android->extraPostTable->Bind(srcClassName.c_str(), *extraTable);

    LOGI("onReturnToSquirrel : switching onto Squirrel function");
    returnFunc.Execute(requestCode, resultCode, srcClassName, thisClassName);
}

char * getCStringFromJava(JNIEnv* env, jstring javaString ){
    char * resultChars;
    const char * cString = env->GetStringUTFChars(javaString, NULL);
    resultChars = strdup(cString);
    env->ReleaseStringUTFChars(javaString, cString);

    return resultChars;
}

}// end of extrern "C"


namespace emo {
    string JavaGlue::echo(string value) {
    	return this->callString_String("echo", value);
    }

    void JavaGlue::asyncHttpRequest(string name, jint timeout, string url, string method, kvs_t* params) {
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
            { "callback", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
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

    /*
     * Call java method with no parameter that returns void.
     */
    void JavaGlue::callVoid_Void(string methodName) {
        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;

        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, methodName.c_str(), "()V");
        env->CallVoidMethod(engine->app->activity->clazz, methodj);

        vm->DetachCurrentThread();
    }

    /*
     * Call java method with no parameter that returns string.
     */
    string JavaGlue::callVoid_String(string methodName) {
        string value;
        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;

        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, methodName.c_str(), "()Ljava/lang/String;");
        jstring jstr = (jstring)env->CallObjectMethod(engine->app->activity->clazz, methodj);
        if (jstr != NULL) {
            const char* str = env->GetStringUTFChars(jstr, NULL);
            value = str;
            env->ReleaseStringUTFChars(jstr, str);
        }
        vm->DetachCurrentThread();

        return value;
    }

    /*
     * Call java method with no parameter that returns boolean.
     */
    bool JavaGlue::callVoid_Bool(string methodName) {
    	jboolean value = false;

        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;

        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, methodName.c_str(), "()Z");
        value = env->CallBooleanMethod(engine->app->activity->clazz, methodj);
        vm->DetachCurrentThread();

        return value;
    }

    /*
     * Call java method with no parameter that returns int.
     */
    jint JavaGlue::callVoid_Int(string methodName) {
        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;

        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, methodName.c_str(), "()I");
        jint value = env->CallIntMethod(engine->app->activity->clazz, methodj);
        vm->DetachCurrentThread();

        return value;
    }

    /*  
     * Call java method with int parameter that returns int.
     */  
    jint JavaGlue::callInt_Int(string methodName, jint passValue) {
        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;

        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, methodName.c_str(), "(I)I");
        jint value = env->CallIntMethod(engine->app->activity->clazz, methodj, passValue);
        vm->DetachCurrentThread();

        return value;
    }   

    /*  
     * Call java method with int,int parameter that returns int.
     */  
    jint JavaGlue::callIntInt_Int(string methodName, jint passValue1, jint passValue2) {
        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;

        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, methodName.c_str(), "(II)I");
        jint value = env->CallIntMethod(engine->app->activity->clazz, methodj, passValue1, passValue2);
        vm->DetachCurrentThread();

        return value;
    }   

    /*
     * Call java method with one string parameter that returns string.
     */
    string JavaGlue::callString_String(string methodName, string passValue) {
        string value;

        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;

        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, methodName.c_str(), "(Ljava/lang/String;)Ljava/lang/String;");
        jstring jstr = (jstring)env->CallObjectMethod(engine->app->activity->clazz, methodj,  env->NewStringUTF(passValue.c_str()));
        if (jstr != NULL) {
            const char* str = env->GetStringUTFChars(jstr, NULL);
            value = str;
            env->ReleaseStringUTFChars(jstr, str);
        }
        vm->DetachCurrentThread();

        return value;
    }

    /*
     * Call java method with two string parameter that returns void.
     */
    void JavaGlue::callTwoString_Void(string methodName, string value1, string value2) {
        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;

        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, methodName.c_str(), "(Ljava/lang/String;Ljava/lang/String;)V");
        env->CallVoidMethod(engine->app->activity->clazz, methodj, env->NewStringUTF(value1.c_str()), env->NewStringUTF(value2.c_str()));
        vm->DetachCurrentThread();
    }

    /*
     * Call java method with (string,int) parameter that returns void.
     */
    void JavaGlue::callStringInt_Void(string methodName, string value1, jint value2) {
        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;

        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, methodName.c_str(), "(Ljava/lang/String;I)V");
        env->CallVoidMethod(engine->app->activity->clazz, methodj, env->NewStringUTF(value1.c_str()), value2);
        vm->DetachCurrentThread();
    }

    string JavaGlue::getDataFilePath(string name) {
        return this->callString_String("getDataFilePath", name);
    }

    void JavaGlue::setOrientationLandscape() {

    	int32_t orient = AConfiguration_getOrientation(engine->app->config);
    	if (orient != ACONFIGURATION_ORIENTATION_LAND) {
    		engine->stage->invertSize();
    	}

    	this->callVoid_Void("setOrientationLandscape");
    }

    void JavaGlue::setOrientationPortrait() {
    	int32_t orient = AConfiguration_getOrientation(engine->app->config);
    	if (orient != ACONFIGURATION_ORIENTATION_PORT) {
    		engine->stage->invertSize();
    	}

    	this->callVoid_Void("setOrientationPortrait");
    }

    string JavaGlue::getDeviceName() {
    	return this->callVoid_String("getDeviceName");
    }

    string JavaGlue::getOSVersion() {
    	return this->callVoid_String("getOSVersion");
    }

    int JavaGlue::getSDKVersion() {
    	return this->callVoid_Int("getSDKVersion");
    }

    int JavaGlue::getNativeOrientation(int width, int height) {
        int jorient = this->callIntInt_Int("getNativeOrientation", width, height);
        if (jorient == 0) {
            return OPT_ORIENTATION_LANDSCAPE;
        } else {
            return OPT_ORIENTATION_PORTRAIT;
        }
    }

    void JavaGlue::transit(string targetClass, kvs_t* extras, int optionFlag, int requestCode){

        engine->android->activityMap[requestCode] = targetClass;

        int count = (extras->size() * 2) + 2;

        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;
        vm->AttachCurrentThread(&env, NULL);

        jobjectArray jstrArray= (jobjectArray)env->NewObjectArray(count, env->FindClass("java/lang/String"), env->NewStringUTF(""));

        env->SetObjectArrayElement(jstrArray, 0, env->NewStringUTF("key"));
        env->SetObjectArrayElement(jstrArray, 1, env->NewStringUTF("value"));

        int i = 2;
        kvs_t::iterator iter;
        for(iter = extras->begin(); iter != extras->end(); iter++) {
           env->SetObjectArrayElement(jstrArray, i, env->NewStringUTF(iter->first.c_str()));
           i++;
           env->SetObjectArrayElement(jstrArray, i, env->NewStringUTF(iter->second.c_str()));
           i++;
        }

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, "transit", "(Ljava/lang/String;II[Ljava/lang/String;)V");
        env->CallVoidMethod(engine->app->activity->clazz, methodj,  env->NewStringUTF(targetClass.c_str()), optionFlag, requestCode, jstrArray);

        vm->DetachCurrentThread();
    }

    bool JavaGlue::isSimulator() {
    	return this->callVoid_Bool("isSimulator");
    }

    void JavaGlue::vibrate() {
    	this->callVoid_Void("vibrate");
    }

    bool JavaGlue::loadTextBitmap(Drawable* drawable, Image* image, bool forceUpdate) {
        emo::FontDrawable* fontDrawable = reinterpret_cast<emo::FontDrawable*>(drawable);

        image->filename = drawable->name;

        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;

        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, "loadTextBitmap", 
            "(Ljava/lang/String;ILjava/lang/String;ZZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)[B");
        jbyteArray src = (jbyteArray)env->CallObjectMethod(engine->app->activity->clazz, methodj,
                                 env->NewStringUTF(fontDrawable->name.c_str()),
                                 (jint)fontDrawable->fontSize,
                                 env->NewStringUTF(fontDrawable->fontFace.c_str()),
                                 fontDrawable->isBold   ? JNI_TRUE : JNI_FALSE,
                                 fontDrawable->isItalic ? JNI_TRUE : JNI_FALSE,
                                 env->NewStringUTF(fontDrawable->param1.c_str()),
                                 env->NewStringUTF(fontDrawable->param2.c_str()),
                                 env->NewStringUTF(fontDrawable->param3.c_str()),
                                 env->NewStringUTF(fontDrawable->param4.c_str()),
                                 env->NewStringUTF(fontDrawable->param5.c_str()),
                                 env->NewStringUTF(fontDrawable->param6.c_str())
                                 );
        jsize size = env->GetArrayLength(src);

        jbyte* png = env->GetByteArrayElements(src, NULL);
        loadPngFromBytes((unsigned char*)png, size, image, forceUpdate);
        env->ReleaseByteArrayElements(src, png, JNI_ABORT);
        vm->DetachCurrentThread();

        return image->width > 0 && image->height > 0;
    }
}


SQInteger emoJavaEcho(HSQUIRRELVM v) {
    SQInteger nargs = sq_gettop(v);
    for(SQInteger n = 1; n <= nargs; n++) {
		const SQChar *str;
    	if (sq_gettype(v, n) == OT_STRING) {
            sq_tostring(v, n);
            sq_getstring(v, -1, &str);
            sq_poptop(v);
        } else {
            str = NULL;
        }
    	if (str != NULL) {
            string value = engine->javaGlue->echo(str);
	    	sq_pushstring(v, value.c_str(), -1);
    	}
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

    if (nargs >= 5 && sq_gettype(v, 5) != OT_NULL) {
        sq_getinteger(v, 5, &timeout);
    }

    kvs_t* params = new kvs_t();
    string param_key;
    for(SQInteger n = 6; n <= nargs; n++) {

        const SQChar* param;
        sq_tostring(v, n);
        sq_getstring(v, -1, &param);
        sq_poptop(v);

        if (n % 2 == 0) {
            param_key = param;
        } else if (!param_key.empty()) {
            params->insert(make_pair(param_key, param));
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

