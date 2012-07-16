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
#include "Engine.h"
#include "Runtime.h"
#include "VmFunc.h"
#include "Physics.h"

#include <android/window.h>
#include <jni.h>
#include <GLES/glext.h>

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

namespace emo {
    bool drawable_z_compare(const Drawable* left, const Drawable* right) {
        return left->z < right->z;
    }

    Engine::Engine() {
        this->logLevel = LOG_INFO;

        this->focused = false;
        this->loaded  = false;
        this->loadedCalled = false;
        this->initialized  = false;
        this->finishing = false;
        this->sortOrderDirty = true;
        this->enableOnUpdate = false;
        this->sensorManager = NULL;
        this->sensorEventQueue = NULL;

        this->display = EGL_NO_DISPLAY;
        this->context = EGL_NO_CONTEXT;
        this->surface = EGL_NO_SURFACE;

        this->framebuffer = 0;
        this->offscreenFramebuffer = 0;
        this->useOffscreen = false;
        this->stopOffscreenRequested = false;
        this->canUseOffscreen = true;

        this->srcBlendFactor = GL_SRC_ALPHA;
        this->dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;

        this->nativeOrientation = OPT_ORIENTATION_PORTRAIT;

        this->useANR = false;

        this->sqvm = sq_open(SQUIRREL_VM_INITIAL_STACK_SIZE);

    }

    Engine::~Engine() {
        delete this->stage;
        delete this->audio;
        delete this->drawables;
        delete this->drawablesToRemove;
        delete this->database;
        delete this->javaGlue;
        delete this->sortedDrawables;
        delete this->imageCache;
    }

    int32_t Engine::event_handle_input(android_app* app, AInputEvent* event) {
        if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
            return this->onMotionEvent(app, event);
        } else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
            return this->onKeyEvent(app, event);
        }
        return 0;
    }

    void Engine::event_handle_cmd(android_app* app, int32_t cmd) {
        switch (cmd) {
            case APP_CMD_INIT_WINDOW:
                if (this->app->window != NULL) {
                    this->onInitDisplay();
                }
                break;
            case APP_CMD_TERM_WINDOW:
                if (this->animating) {
                    this->onLostFocus();
                }
                this->onTerminateDisplay();
                break;
            case APP_CMD_GAINED_FOCUS:
                if (this->app->window == NULL) {
                    LOGE("Application gained focus but window was collapsed.");
                    this->useANR= true;
                    break;
                }
                this->onGainedFocus();
                break;
            case APP_CMD_LOST_FOCUS:
                this->onLostFocus();
                break;
            case APP_CMD_LOW_MEMORY:
                this->onLowMemory();
                break;
        }
    }

    void Engine::onInitEngine() {
        if (this->loaded) return;

        // initialize startup time
        ftime(&this->startTime);

        // initialize uptime
        this->updateUptime();

        // init Squirrel VM
        if (this->sqvm == NULL) {
            this->sqvm = sq_open(SQUIRREL_VM_INITIAL_STACK_SIZE);
        }
        initSQVM(this->sqvm);

        // bind name space "emo"
        this->emoSpace = new Sqrat::Table(this->sqvm);
        Sqrat::RootTable(this->sqvm).Bind(EMO_NAMESPACE, *(this->emoSpace));

        this->lastError = EMO_NO_ERROR;

        // disable drawframe callback to improve performance (default)
        this->enableOnDrawFrame   = false;
        this->onDrawFrameInterval = 0;
        this->lastOnDrawInterval  = this->uptime;

        this->onDrawDrawablesInterval = 0;
        this->lastOnDrawDrawablesInterval = this->uptime;

        this->frameCount         = 0;
        this->onFpsInterval      = 0;
        this->onFpsIntervalDelta = 0;
        this->enableOnFps        = false;

        // enable perspective hint to nicest (default)
        this->enablePerspectiveNicest = SQTrue;

        // engine does not handle the back key by default
        this->enableBackKey = true;

        // force fullscreen
        this->updateOptions(OPT_WINDOW_FORCE_FULLSCREEN);

        // create stage instance
        stage = new Stage();

        // create audio instance
        audio = new Audio();

        // create database instance
        database = new Database();

        // create JavaGlue instance
        javaGlue = new JavaGlue();

        // create SquirrelGlue instance
        squirrelGlue = new SquirrelGlue();

        // create Android(mediator) instance
        android = new Android();

        this->focused = false;
        this->loadedCalled = false;
        this->initialized  = false;
        this->scriptLoaded = false;
        this->stopwatchStarted = false;
        this->stopwatchStopped = false;

        this->drawables = new drawables_t();
        this->drawablesToRemove = new drawables_t();
        this->sortedDrawables = new std::vector<Drawable*>;

        this->imageCache = new images_t();

        // register activity's native methods
        this->javaGlue->registerJavaGlue();

        // start the stop watch automatically
        this->stopwatchStart();

        this->loaded  = true;
    }


    int32_t Engine::onInitDisplay() {
        const EGLint attribs[] = {
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_NONE
        };
        EGLint w, h, format;
        EGLint numConfigs;
        EGLConfig config;
        EGLSurface surface;
        EGLContext context;

        EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        eglInitialize(display, 0, 0);

        eglChooseConfig(display, attribs, &config, 1, &numConfigs);

        eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

        ANativeWindow_setBuffersGeometry(this->app->window, 0, 0, format);

        surface = eglCreateWindowSurface(display, config, this->app->window, NULL);
        context = eglCreateContext(display, config, NULL, NULL);

        if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
            LOGW("Unable to eglMakeCurrent");
            return -1;
        }

        eglQuerySurface(display, surface, EGL_WIDTH, &w);
        eglQuerySurface(display, surface, EGL_HEIGHT, &h);

        this->display = display;
        this->context = context;
        this->surface = surface;
        this->width   = w;
        this->height  = h;

        this->nativeOrientation = this->javaGlue->getNativeOrientation(w, h);

        if (this->stage->width == 0 && this->stage->height == 0) {
            this->stage->setBufferSize(w, h);
            this->stage->setSizeAndView(w, h);
        }

        clearGLErrors("emo::Engine::onInitDisplay");

        // obtain default framebuffer id
        glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, &framebuffer);
        if (glGetError() != GL_NO_ERROR) {
            framebuffer = 0;
        }

        if (!this->scriptLoaded) {
            // register class and functions for script
            this->squirrelGlue->initScriptFunctions();

            // load runtime and main script
            loadScriptFromAsset(this->getRuntimeScriptName().c_str());
            loadScriptFromAsset(this->getMainScriptName().c_str());

            this->scriptLoaded = true;
        }
        this->onInitGLSurface();

        this->updateUptime();
        this->lastOnDrawInterval  = this->uptime;
        this->lastOnDrawDrawablesInterval = this->uptime;

        return 0;
    }

    int32_t Engine::getWidth() {
        return this->width;
    }

    int32_t Engine::getHeight() {
        return this->height;
    }

    std::string Engine::getRuntimeScriptName() {
        std::string scriptName;

        JNIEnv* env;
        JavaVM* vm = this->app->activity->vm;
        
        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(this->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, "getRuntimeScriptName", "()Ljava/lang/String;");
        jstring jstr = (jstring)env->CallObjectMethod(this->app->activity->clazz, methodj);
        if (jstr != NULL) {
            const char* str = env->GetStringUTFChars(jstr, NULL);
            scriptName = str;
            env->ReleaseStringUTFChars(jstr, str);
        } else {
            scriptName = SQUIRREL_RUNTIME_SCRIPT;
        }
        vm->DetachCurrentThread();

        return scriptName;
    }

    std::string Engine::getMainScriptName() {
        std::string scriptName;

        JNIEnv* env;
        JavaVM* vm = this->app->activity->vm;
        
        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(this->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, "getMainScriptName", "()Ljava/lang/String;");
        jstring jstr = (jstring)env->CallObjectMethod(this->app->activity->clazz, methodj);
        if (jstr != NULL) {
            const char* str = env->GetStringUTFChars(jstr, NULL);
            scriptName = str;
            env->ReleaseStringUTFChars(jstr, str);
        } else {
            scriptName = SQUIRREL_MAIN_SCRIPT;
        }
        vm->DetachCurrentThread();

        return scriptName;
    }

    std::string Engine::getJavaPackageName() {
        std::string packageName;

        JNIEnv* env;
        JavaVM* vm = this->app->activity->vm;
        
        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(this->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, "getPackageName", "()Ljava/lang/String;");
        jstring jstr = (jstring)env->CallObjectMethod(this->app->activity->clazz, methodj);
        if (jstr != NULL) {
            const char* str = env->GetStringUTFChars(jstr, NULL);
            packageName = str;
            env->ReleaseStringUTFChars(jstr, str);
        }
        vm->DetachCurrentThread();

        return packageName;
    }

    void Engine::onInitGLSurface() {
        if (!this->loaded) return;

        this->updateUptime();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_MULTISAMPLE);
        glDisable(GL_DITHER);

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDisable(GL_CULL_FACE);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);

        clearGLErrors("emo::Engine::onInitGLSurface");

        if (!this->loadedCalled) {
            callSqFunction(this->sqvm, EMO_NAMESPACE, EMO_FUNC_ONLOAD);
            this->loadedCalled = true;
        }

        // update OpenGL options
        if (this->enablePerspectiveNicest) {
            glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        } else {
            glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
        }

        if (this->initialized) {
            // return onPause so re-create buffers
            this->stage->rebindBuffer();
            this->rebindDrawableBuffers();
        } else {
            this->stage->onLoad();
            this->loadDrawables();

            this->initialized = true;
        }

        this->lastOnDrawInterval  = this->uptime;
        this->lastOnDrawDrawablesInterval  = this->uptime;
    }

    void Engine::onDispose() {
        if (this->loaded) {
            if (this->audio->isRunning()) {
                this->audio->close();
            }
            this->updateUptime();
            callSqFunction(this->sqvm, EMO_NAMESPACE, EMO_FUNC_ONDISPOSE);
            sq_close(this->sqvm);
            this->sqvm = NULL;

            this->unloadDrawables();
            this->stage->deleteBuffer();

            this->loaded = false;
        }
    }

    void Engine::onGainedFocus() {
        if (!this->loaded) return;
        if (this->focused) return;

        this->focused = true;

        this->updateUptime();
        callSqFunction(this->sqvm, EMO_NAMESPACE, EMO_FUNC_ONGAINED_FOUCS);
        this->animating = true;
    }

    void Engine::onLostFocus() {
        if (!this->loaded) return;
        if (!this->focused) return;

        this->focused = false;

        this->updateUptime();
        callSqFunction(this->sqvm, EMO_NAMESPACE, EMO_FUNC_ONLOST_FOCUS);
        this->animating = false;
    }

    void Engine::onLowMemory() {
        if (!this->loaded) return;

        this->updateUptime();
        callSqFunction(this->sqvm, EMO_NAMESPACE, EMO_FUNC_ONLOW_MEMORY);
    }

    void Engine::updateUptime() {
        timeb eventTime;
        ftime(&eventTime);

        this->uptime.time     = eventTime.time - this->startTime.time;
        this->uptime.millitm  = eventTime.millitm;
    }

    int32_t Engine::getLastOnDrawDelta() {
        int32_t deltaSec  = this->uptime.time - this->lastOnDrawInterval.time;
        int32_t deltaMsec = this->uptime.millitm - this->lastOnDrawInterval.millitm;

        return (deltaSec * 1000) + deltaMsec;
    }

    int32_t Engine::getLastOnDrawDrawablesDelta() {
        int32_t deltaSec  = this->uptime.time - this->lastOnDrawDrawablesInterval.time;
        int32_t deltaMsec = this->uptime.millitm - this->lastOnDrawDrawablesInterval.millitm;

        return (deltaSec * 1000) + deltaMsec;
    }

    int32_t Engine::getLastStopwatchElapsedDelta() {
        int32_t deltaSec  = this->stopwatchAccumulateTime.time + (this->uptime.time - this->stopwatchRestartTime.time);
        int32_t deltaMsec = this->stopwatchAccumulateTime.millitm + (this->uptime.millitm - this->stopwatchRestartTime.millitm);

        return (deltaSec * 1000) + deltaMsec;
     }

    void Engine::stopwatchStart() {
        this->updateUptime();
        this->stopwatchStartTime = this->uptime;
        this->stopwatchRestartTime = this->uptime;
        this->stopwatchStarted = true;
        this->stopwatchStopped = false;
        this->stopwatchAccumulateTime.time = 0;
        this->stopwatchAccumulateTime.millitm = 0;

    }

    void Engine::stopwatchRestart() {
        if(this->stopwatchStopped == false){
            this->stopwatchStart();
        }else{
            this->updateUptime();
            this->stopwatchRestartTime = this->uptime;
            this->stopwatchStopped = false;
        }
    }

    void Engine::stopwatchStop() {
        this->updateUptime();
        this->stopwatchElapsedTime = this->getLastStopwatchElapsedDelta();
        this->stopwatchAccumulateTime.time += (this->uptime.time - this->stopwatchRestartTime.time);
        this->stopwatchAccumulateTime.millitm = this->uptime.millitm;
        this->stopwatchStopped = true;
    }

    int32_t Engine::stopwatchElapsed() {
        if (this->stopwatchStarted == true && this->stopwatchStopped == false) {
            this->updateUptime();
            this->stopwatchElapsedTime = this->getLastStopwatchElapsedDelta();
        }

        return this->stopwatchElapsedTime;
    }

    int32_t Engine::stopwatchRealElapsed() {
        this->updateUptime();
        int32_t deltaSec  = this->uptime.time - this->stopwatchStartTime.time;
        int32_t deltaMsec = this->uptime.millitm - this->stopwatchStartTime.millitm;

        return (deltaSec * 1000) + deltaMsec;
    }

    bool Engine::stopwatchIsStopped(){
        return this->stopwatchStopped;
    }

    int32_t Engine::onSensorEvent(ASensorEvent* event) {
        if (!this->loaded) return 0;
        if (!this->focused) return 0;
        this->updateUptime();

        switch(event->type) {
        case ASENSOR_TYPE_ACCELEROMETER:
            this->accelerometerEventParamCache[0] = event->type;
            if (this->nativeOrientation == OPT_ORIENTATION_LANDSCAPE) {
                this->accelerometerEventParamCache[1] =  event->acceleration.y / -ASENSOR_STANDARD_GRAVITY;
                this->accelerometerEventParamCache[2] = -event->acceleration.x / -ASENSOR_STANDARD_GRAVITY;
            } else {
                this->accelerometerEventParamCache[1] = event->acceleration.x / -ASENSOR_STANDARD_GRAVITY;
                this->accelerometerEventParamCache[2] = event->acceleration.y / -ASENSOR_STANDARD_GRAVITY;
            }
            this->accelerometerEventParamCache[3] = event->acceleration.z / -ASENSOR_STANDARD_GRAVITY;
            if (callSqFunction_Bool_Floats(this->sqvm, EMO_NAMESPACE, EMO_FUNC_SENSOREVENT, this->accelerometerEventParamCache, ACCELEROMETER_EVENT_PARAMS_SIZE, false)) {
                return 1;
            }
            break;
        }
        return 0;
    }

    int32_t Engine::onMotionEvent(android_app* app, AInputEvent* event) {
        if (!this->loaded) return 0;
        if (!this->focused) return 0;

        size_t pointerCount =  AMotionEvent_getPointerCount(event);

        this->updateUptime();

        for (size_t i = 0; i < pointerCount; i++) {
            size_t pointerId = AMotionEvent_getPointerId(event, i);
            size_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
            size_t pointerIndex = i;
        
            if (action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_POINTER_UP) {
                pointerIndex = (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                pointerId = AMotionEvent_getPointerId(event, pointerIndex);
            }

            this->touchEventParamCache[0] = pointerId;
            this->touchEventParamCache[1] = action;
            this->touchEventParamCache[2] = AMotionEvent_getX(event, pointerIndex);
            this->touchEventParamCache[3] = AMotionEvent_getY(event, pointerIndex);
            this->touchEventParamCache[4] = this->uptime.time;
            this->touchEventParamCache[5] = this->uptime.millitm;
            this->touchEventParamCache[6] = AInputEvent_getDeviceId(event);
            this->touchEventParamCache[7] = AInputEvent_getSource(event);
            
            if (callSqFunction_Bool_Floats(this->sqvm, EMO_NAMESPACE, EMO_FUNC_MOTIONEVENT, this->touchEventParamCache, MOTION_EVENT_PARAMS_SIZE, false)) {
                return 1;
            }
        }
        return 0;
    }

    int32_t Engine::onKeyEvent(android_app* app, AInputEvent* event) {
        if (!this->loaded) return 0;
        if (!this->focused) return 0;

        this->updateUptime();

        this->keyEventParamCache[0] = AKeyEvent_getAction(event);
        this->keyEventParamCache[1] = AKeyEvent_getKeyCode(event);
        this->keyEventParamCache[2] = AKeyEvent_getRepeatCount(event);
        this->keyEventParamCache[3] = AKeyEvent_getMetaState(event);
        this->keyEventParamCache[4] = this->uptime.time;
        this->keyEventParamCache[5] = this->uptime.millitm;
        this->keyEventParamCache[6] = AInputEvent_getDeviceId(event);
        this->keyEventParamCache[7] = AInputEvent_getSource(event);

        if (callSqFunction_Bool_Floats(this->sqvm, EMO_NAMESPACE,
                    EMO_FUNC_KEYEVENT, this->keyEventParamCache, KEY_EVENT_PARAMS_SIZE, false)) {
            return 1;
        } else if (AKeyEvent_getKeyCode(event) == AKEYCODE_BACK && this->enableBackKey) {
            if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN) {
                emoRuntimeFinish(this->sqvm);
            }
            return 1;
        }

        return 0;
    }

    void Engine::onTerminateDisplay() {
        if (this->display != EGL_NO_DISPLAY) {

            if (this->loaded) {
                this->deleteDrawableBuffers();
                this->stage->deleteBuffer();
            }

            eglMakeCurrent(this->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (this->context != EGL_NO_CONTEXT) {
                eglDestroyContext(this->display, this->context);
            }
            if (this->surface != EGL_NO_SURFACE) {
                eglDestroySurface(this->display, this->surface);
            }
            eglTerminate(this->display);
        }
        this->animating = false;
        this->display = EGL_NO_DISPLAY;
        this->context = EGL_NO_CONTEXT;
        this->surface = EGL_NO_SURFACE;
    }

    bool Engine::hasDisplay() {
        return this->display != EGL_NO_DISPLAY && this->display != NULL;
    }

    void Engine::onDrawFrame() {
        if (!this->hasDisplay()) {
            return;
        }

        if (!this->loaded) return;
        if (!this->focused) return;

        this->updateUptime();

        if (this->enableOnUpdate) {
            int32_t _delta = this->getLastOnDrawDrawablesDelta();
            callSqFunction_Bool_Float(this->sqvm, EMO_NAMESPACE, EMO_FUNC_ON_UPDATE, _delta, SQFalse);
        }

        int32_t delta = this->getLastOnDrawDelta();

        if (this->enableOnDrawFrame && delta >= this->onDrawFrameInterval) {
            this->lastOnDrawInterval  = this->uptime;
            callSqFunction_Bool_Float(this->sqvm, EMO_NAMESPACE, EMO_FUNC_ONDRAW_FRAME, delta, SQFalse);
        }

        delta = this->getLastOnDrawDrawablesDelta();
        if (delta < this->onDrawDrawablesInterval) {
            return;
        }

        if (this->enableOnFps) {
            this->frameCount++;
            this->onFpsIntervalDelta += delta;
            if (this->onFpsIntervalDelta >= this->onFpsInterval) {
                float fps = 1000.0 / (this->onFpsIntervalDelta / (float)this->frameCount);
                callSqFunction_Bool_Float(this->sqvm, EMO_NAMESPACE, EMO_FUNC_ON_FPS, fps, SQFalse);
                this->onFpsIntervalDelta = 0;
                this->frameCount         = 0;
            }
        }

        this->lastOnDrawDrawablesInterval  = this->uptime;

        if (likely(!this->useOffscreen)) this->stage->onDrawFrame();
        this->onDrawDrawables(delta);

        eglSwapBuffers(this->display, this->surface);

        if (this->finishing) {
            this->onLostFocus();
            this->onTerminateDisplay();
            ANativeActivity_finish(this->app->activity);
        }
    }

    void Engine::addDrawable(std::string key, Drawable* drawable) {
        this->drawables->insert(std::make_pair(key, drawable)); 
        this->sortOrderDirty = true;
    }

    bool Engine::removeDrawable(std::string key) {
        drawables_t::iterator iter = this->drawables->find(key);
        if (iter != this->drawables->end()) {
            this->addDrawableToRemove(iter->first, iter->second);
            return true;
        }
        return false;
    }

    void Engine::addDrawableToRemove(std::string key, Drawable* drawable) {
        this->drawablesToRemove->insert(std::make_pair(key, drawable));
    }

    Drawable* Engine::getDrawable(std::string key) {
        drawables_t::iterator iter = this->drawables->find(key);
        if (iter != this->drawables->end()) {
            return iter->second;
        }
        return NULL;
    }

    void Engine::loadDrawables() {
        drawables_t::iterator iter;
        for(iter = this->drawables->begin(); iter != this->drawables->end(); iter++) {
            Drawable* drawable = iter->second;
            drawable->load();
        }
    }

    bool Engine::freeDrawable(std::string key) {
        drawables_t::iterator iter = this->drawables->find(key);
        if (iter != this->drawables->end()) {
            Drawable* drawable = iter->second;
            if (this->drawables->erase(iter->first)){
                this->sortOrderDirty = true;
                delete drawable;
            }
            return true;
        }
        return false;
    }

    void Engine::onDrawDrawables(int32_t delta) {
        drawables_t::iterator iter;
        if (this->drawablesToRemove->size() > 0) {
            for(iter = this->drawablesToRemove->begin(); iter != this->drawablesToRemove->end(); iter++) {
                this->freeDrawable(iter->first);
            }
            this->drawablesToRemove->clear();
        }

        if (this->sortOrderDirty) {
            this->sortedDrawables->clear();
            for(iter = this->drawables->begin(); iter != this->drawables->end(); iter++) {
                this->sortedDrawables->push_back(iter->second);
            }
            std::sort(this->sortedDrawables->begin(),
                      this->sortedDrawables->end(), drawable_z_compare);
            this->sortOrderDirty = false;
        }
        for (unsigned int i = 0; i < this->sortedDrawables->size(); i++) {
            Drawable* drawable = this->sortedDrawables->at(i);
            if (unlikely(useOffscreen) && i == 0 && !drawable->isScreenEntity) {
                this->bindOffscreenFramebuffer();
                stage->onDrawFrame();
                continue;
            }   
            if (drawable->loaded && drawable->independent && drawable->isVisible()) {
                if ((this->srcBlendFactor != drawable->srcBlendFactor) ||
                    (this->dstBlendFactor != drawable->dstBlendFactor)) {
                    this->srcBlendFactor = drawable->srcBlendFactor;
                    this->dstBlendFactor = drawable->dstBlendFactor;
                    glBlendFunc(this->srcBlendFactor, this->dstBlendFactor);
                }

                drawable->onDrawFrame();
            }
        }

        // render the offscreen result
        if (unlikely(useOffscreen) && this->sortedDrawables->size() > 0) {
            // restore the default framebuffer
            Drawable* drawable = this->sortedDrawables->at(0);
            if (drawable->loaded && !drawable->isScreenEntity) {
                glBindFramebufferOES(GL_FRAMEBUFFER_OES, framebuffer);
                drawable->onDrawFrame();
                if (stopOffscreenRequested) {
                    stopOffscreenRequested = false;
                    this->stopOffscreenDrawable(drawable);
                    callSqFunction_Bool_Float(sqvm, EMO_NAMESPACE, EMO_FUNC_ONSTOP_OFFSCREEN, delta, SQFalse);
                }   
            }   
        }   
    }

    /*
     * stop offscreen
     */
    void Engine::stopOffscreenDrawable(Drawable* drawable) {
        drawable->width  = stage->width;
        drawable->height = stage->height;
    
        // fix rotation and scale center
        drawable->param_rotate[1] = drawable->width  * 0.5f;
        drawable->param_rotate[2] = drawable->height * 0.5f;
        drawable->param_scale[2]  = drawable->width  * 0.5f;
        drawable->param_scale[3]  = drawable->height * 0.5f;
    
        this->disableOffscreen();
        stage->dirty = true;
    }

    void Engine::rebindDrawableBuffers() {
        drawables_t::iterator iter;
        for(iter = this->drawables->begin(); iter != this->drawables->end(); iter++) {
            Drawable* drawable = iter->second;
            drawable->reload();
            drawable->bindVertex();
        }
    }

    void Engine::unloadDrawables() {
        this->sortedDrawables->clear();
        this->sortOrderDirty = true;

        drawables_t::iterator iter;
        for(iter = this->drawables->begin(); iter != this->drawables->end(); iter++) {
            delete iter->second;
        }
        this->drawables->clear();
    }

    void Engine::deleteDrawableBuffers() {
        drawables_t::iterator iter;
        for(iter = this->drawables->begin(); iter != this->drawables->end(); iter++) {
            Drawable* drawable = iter->second;
            drawable->deleteBuffer(true);
        }
    }

    void Engine::updateOptions(int value) {
        switch(value) {
        case OPT_ENABLE_PERSPECTIVE_NICEST:
            this->enablePerspectiveNicest = true;
            break;
        case OPT_ENABLE_PERSPECTIVE_FASTEST:
            this->enablePerspectiveNicest = false;
            break;
        case OPT_WINDOW_FORCE_FULLSCREEN:
            ANativeActivity_setWindowFlags(this->app->activity, AWINDOW_FLAG_FULLSCREEN, AWINDOW_FLAG_FORCE_NOT_FULLSCREEN);
            break;
        case OPT_WINDOW_KEEP_SCREEN_ON:
            ANativeActivity_setWindowFlags(this->app->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);
            break;
        case OPT_ENABLE_BACK_KEY:
            this->enableBackKey = true;
            break;
        case OPT_DISABLE_BACK_KEY:
            this->enableBackKey = false;
            break;
        case OPT_ORIENTATION_PORTRAIT:
            this->javaGlue->setOrientationPortrait();
            break;
        case OPT_ORIENTATION_LANDSCAPE_LEFT:
        case OPT_ORIENTATION_LANDSCAPE_RIGHT:
        case OPT_ORIENTATION_LANDSCAPE:
        	this->javaGlue->setOrientationLandscape();
            break;
        }
    }

    void Engine::createSensors(int value) {
        if (this->sensorManager == NULL) {
            // Get a reference to the sensor manager
            this->sensorManager = ASensorManager_getInstance();

            if (this->sensorManager == NULL) {
            	LOGI("Could not create sensor manager");
            	return;
            }
        }

        switch(value) {
        case SENSOR_TYPE_ACCELEROMETER:
            this->accelerometerSensor = ASensorManager_getDefaultSensor(
                this->sensorManager, ASENSOR_TYPE_ACCELEROMETER);
            break;
        case SENSOR_TYPE_MAGNETIC_FIELD:
            this->magneticSensor = ASensorManager_getDefaultSensor(
                this->sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
            break;
        case SENSOR_TYPE_GYROSCOPE:
            this->gyroscopeSensor = ASensorManager_getDefaultSensor(
                this->sensorManager, ASENSOR_TYPE_GYROSCOPE);
            break;
        case SENSOR_TYPE_LIGHT:
            this->lightSensor = ASensorManager_getDefaultSensor(
                this->sensorManager, ASENSOR_TYPE_LIGHT);
            break;
        case SENSOR_TYPE_PROXIMITY:
            this->proximitySensor = ASensorManager_getDefaultSensor(
                this->sensorManager, ASENSOR_TYPE_PROXIMITY);
            break;
        }

        if (this->sensorEventQueue == NULL) {
        	// Creates a new sensor event queue
        	this->sensorEventQueue = ASensorManager_createEventQueue(
        			this->sensorManager, this->app->looper, LOOPER_ID_USER, NULL, NULL);

        	if (this->sensorEventQueue == NULL) {
        		LOGI("Could not create sensor event queue");
        		return;
        	}
        }
    }

    /*
     * enable sensor
     */
    void Engine::enableSensor(int sensorType, int msec) {
    	if (this->sensorManager == NULL || this->sensorEventQueue == NULL) return;

        int interval = msec * 1000;

        switch(sensorType) {
        case SENSOR_TYPE_ACCELEROMETER:
            if (this->accelerometerSensor != NULL) {
                ASensorEventQueue_enableSensor(this->sensorEventQueue,
                    this->accelerometerSensor);
                ASensorEventQueue_setEventRate(this->sensorEventQueue,
                    this->accelerometerSensor, interval);
            }
            break;
        case SENSOR_TYPE_MAGNETIC_FIELD:
            if (this->magneticSensor != NULL) {
                ASensorEventQueue_enableSensor(this->sensorEventQueue,
                    this->magneticSensor);
                ASensorEventQueue_setEventRate(this->sensorEventQueue,
                    this->magneticSensor, interval);
            }
            break;
        case SENSOR_TYPE_GYROSCOPE:
            if (this->gyroscopeSensor != NULL) {
                ASensorEventQueue_enableSensor(this->sensorEventQueue,
                    this->gyroscopeSensor);
                ASensorEventQueue_setEventRate(this->sensorEventQueue,
                    this->gyroscopeSensor, interval);
            }
            break;
        case SENSOR_TYPE_LIGHT:
            if (this->lightSensor != NULL) {
                ASensorEventQueue_enableSensor(this->sensorEventQueue,
                    this->lightSensor);
                ASensorEventQueue_setEventRate(this->sensorEventQueue,
                    this->lightSensor, interval);
            }
            break;
        case SENSOR_TYPE_PROXIMITY:
            if (this->proximitySensor != NULL) {
                ASensorEventQueue_enableSensor(this->sensorEventQueue,
                    this->proximitySensor);
                ASensorEventQueue_setEventRate(this->sensorEventQueue,
                    this->proximitySensor, interval);
            }
            break;
        }
    }

    /*
     * disable sensor
     */
    void Engine::disableSensor(int sensorType) {
    	if (this->sensorManager == NULL || this->sensorEventQueue == NULL) return;
        switch(sensorType) {
        case SENSOR_TYPE_ACCELEROMETER:
            if (this->accelerometerSensor != NULL) {
                ASensorEventQueue_disableSensor(this->sensorEventQueue,
                    this->accelerometerSensor);
            }
            break;
        case SENSOR_TYPE_MAGNETIC_FIELD:
            if (this->magneticSensor != NULL) {
                ASensorEventQueue_disableSensor(this->sensorEventQueue,
                    this->magneticSensor);
            }
            break;
        case SENSOR_TYPE_GYROSCOPE:
            if (this->gyroscopeSensor != NULL) {
                ASensorEventQueue_disableSensor(this->sensorEventQueue,
                    this->gyroscopeSensor);
            }
            break;
        case SENSOR_TYPE_LIGHT:
            if (this->lightSensor != NULL) {
                ASensorEventQueue_disableSensor(this->sensorEventQueue,
                    this->lightSensor);
            }
            break;
        case SENSOR_TYPE_PROXIMITY:
            if (this->proximitySensor != NULL) {
                ASensorEventQueue_disableSensor(this->sensorEventQueue,
                    this->proximitySensor);
            }
            break;
        }
    }

    ASensorEventQueue* Engine::getSensorEventQueue() {
        return sensorEventQueue;
    }

    int32_t Engine::getLastError() {
        return this->lastError;
    }

    void Engine::setLastError(int32_t error) {
        this->lastError = error;
    }

    void Engine::enableOnDrawListener(bool enable) {
        this->enableOnDrawFrame = enable;
    }

    void Engine::setOnDrawListenerInterval(int value) {
        this->onDrawFrameInterval = value;
        this->lastOnDrawInterval  = this->uptime;
    }

    void Engine::setOnDrawDrawablesInterval(int value) {
        this->onDrawDrawablesInterval     = value;
        this->lastOnDrawDrawablesInterval = this->uptime;
	}

    void Engine::enableOnUpdateListener(bool enable) {
        this->enableOnUpdate = enable;
    }

    void Engine::enableOnFpsListener(bool enable) {
        this->frameCount  = 0;
        this->enableOnFps = enable;
    }

    void Engine::setOnFpsListenerInterval(int value) {
        this->onFpsInterval = value;
    }

    bool Engine::hasCachedImage(std::string key) {
        images_t::iterator iter = this->imageCache->find(key);
        if (iter != this->imageCache->end()) {
            return true;
        }
        return false;
    }

    Image* Engine::getCachedImage(std::string key) {
        images_t::iterator iter = this->imageCache->find(key);
        if (iter != this->imageCache->end()) {
            return iter->second;
        }
        return NULL;
    }

    void Engine::addCachedImage(std::string key, Image* image) {
        this->imageCache->insert(std::make_pair(key, image));
    }

    bool Engine::removeCachedImage(std::string key) {
        return this->imageCache->erase(key);
    }

    void Engine::clearCachedImage() {
        images_t::iterator iter;
        for(iter = this->imageCache->begin(); iter != this->imageCache->end(); iter++) {
            Image* image = iter->second;
            image->clearTexture();
        }
    }

    /*
     * enable offscreen rendering
     */
    void Engine::enableOffscreen() {
        if (!canUseOffscreen) return;
        if (!useOffscreen && offscreenFramebuffer == 0) {
            glGenFramebuffersOES(1, &offscreenFramebuffer);
        }
        useOffscreen = true;
        stopOffscreenRequested = false;
    }

    /*
     * disable offscreen rendering
     */
    void Engine::disableOffscreen() {
        if (!canUseOffscreen) return;
        if (useOffscreen && offscreenFramebuffer != 0) {
            glDeleteFramebuffersOES(1, &offscreenFramebuffer);
            offscreenFramebuffer  = 0;
        }   
        useOffscreen = false;
        stopOffscreenRequested = false;
    }

    /*
     * bind offscreen framebuffer
     */
    void Engine::bindOffscreenFramebuffer() {
        if (!canUseOffscreen) return;
        if (offscreenFramebuffer > 0) {
            glBindFramebufferOES(GL_FRAMEBUFFER_OES, offscreenFramebuffer);
        }
    }
}

