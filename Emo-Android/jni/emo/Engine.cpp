#include "Engine.h"
#include "Runtime.h"
#include "VmFunc.h"

#include <android/window.h>

namespace emo {
    Engine::Engine() {
        this->focused = false;
        this->loaded  = false;
        this->loadedCalled = false;
        this->initialized  = false;
    }

    Engine::~Engine() {
        delete this->stage;
        delete this->audio;
        delete this->drawables;
        delete this->drawablesToRemove;
    }

    void Engine::initScriptFunctions() {
        register_table(this->sqvm, EMO_NAMESPACE);

        initRuntimeFunctions();
        initDrawableFunctions();
        initAudioFunctions();
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
                    this->onDrawFrame();
                }
                break;
            case APP_CMD_TERM_WINDOW:
                if (this->animating) {
                    this->onLostFocus();
                }
                this->onTerminateDisplay();
                break;
            case APP_CMD_GAINED_FOCUS:
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

        this->sqvm = sq_open(SQUIRREL_VM_INITIAL_STACK_SIZE);
        this->lastError = EMO_NO_ERROR;

        // disable drawframe callback to improve performance (default)
        this->enableOnDrawFrame   = false;
        this->onDrawFrameInterval = 0;
        this->lastOnDrawInterval  = this->uptime;

        this->onDrawDrawablesInterval = 0;
        this->lastOnDrawDrawablesInterval = this->uptime;

        // enable perspective hint to nicest (default)
        this->enablePerspectiveNicest = SQTrue;

        // back key is enabled by default
        this->enableBackKey = true;

        // force fullscreen
        this->updateOptions(OPT_WINDOW_FORCE_FULLSCREEN);

        // create stage
        stage = new Stage();

        // create audio
        audio = new Audio();

        this->focused = false;
        this->loadedCalled = false;
        this->initialized  = false;
        this->scriptLoaded = false;

        this->drawables = new drawables_t();
        this->drawablesToRemove = new drawables_t();

        // init Squirrel VM
        initSQVM(this->sqvm);

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

        if (!this->scriptLoaded) {
            // register class and functions for script
            this->initScriptFunctions();

            // load runtime and main script
            loadScriptFromAsset(SQUIRREL_RUNTIME_SCRIPT);
            loadScriptFromAsset(SQUIRREL_MAIN_SCRIPT);

            this->scriptLoaded = true;
        }

        this->onInitGLSurface();

        return 0;
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

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

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

            this->unloadDrawables();
            this->stage->deleteBuffer();

            this->loaded = false;
        }
    }

    void Engine::onGainedFocus() {
        if (!this->loaded) return;

        this->focused = true;

        this->updateUptime();
        callSqFunction(this->sqvm, EMO_NAMESPACE, EMO_FUNC_ONGAINED_FOUCS);
        this->animating = true;
    }

    void Engine::onLostFocus() {
        if (!this->loaded) return;

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

    int32_t Engine::onSensorEvent(ASensorEvent* event) {
        if (!this->loaded) return 0;
        if (!this->focused) return 0;

        this->updateUptime();
        switch(event->sensor) {
        case ASENSOR_TYPE_ACCELEROMETER:
            this->accelerometerEventParamCache[0] = event->sensor;
            this->accelerometerEventParamCache[1] = event->acceleration.x;
            this->accelerometerEventParamCache[2] = event->acceleration.y;
            this->accelerometerEventParamCache[3] = event->acceleration.z;
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

    void Engine::onDrawFrame() {
        if (this->display == NULL) {
            return;
        }

        if (!this->loaded) return;
        if (!this->focused) return;

        this->updateUptime();

        SQFloat delta = this->getLastOnDrawDelta();
        if (this->enableOnDrawFrame && delta > this->onDrawFrameInterval) {
            this->lastOnDrawInterval  = this->uptime;
            callSqFunction_Bool_Float(this->sqvm, EMO_NAMESPACE, EMO_FUNC_ONDRAW_FRAME, delta, SQFalse);
        }

        delta = this->getLastOnDrawDrawablesDelta();
        if (delta < this->onDrawDrawablesInterval) {
            return;
        }
        this->lastOnDrawDrawablesInterval  = this->uptime;

        this->stage->onDrawFrame();
        this->onDrawDrawables();

        eglSwapBuffers(this->display, this->surface);
    }

    void Engine::addDrawable(const char* _key, Drawable* drawable) {
        const char* key = strdup(_key);
        this->drawables->insert(std::make_pair(key, drawable)); 
    }

    bool Engine::removeDrawable(const char* key) {
        drawables_t::iterator iter = this->drawables->find(key);
        if (iter != this->drawables->end()) {
            this->addDrawableToRemove(iter->first, iter->second);
            return true;
        }
        return false;
    }

    void Engine::addDrawableToRemove(const char* _key, Drawable* drawable) {
        const char* key = strdup(_key);
        this->drawablesToRemove->insert(std::make_pair(key, drawable));
    }

    Drawable* Engine::getDrawable(const char* key) {
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

    bool Engine::freeDrawable(const char* key) {
        drawables_t::iterator iter = this->drawables->find(key);
        if (iter != this->drawables->end()) {
            Drawable* drawable = iter->second;
            if (this->drawables->erase(iter->first)){
                delete drawable;
                free((char*)iter->first);
            }
            return true;
        }
        return false;
    }

    void Engine::onDrawDrawables() {
        drawables_t::iterator iter;
        if (this->drawablesToRemove->size() > 0) {
            for(iter = this->drawablesToRemove->begin(); iter != this->drawablesToRemove->end(); iter++) {
                this->freeDrawable(iter->first);
            }
            this->drawablesToRemove->clear();
        }

        for(iter = this->drawables->begin(); iter != this->drawables->end(); iter++) {
            Drawable* drawable = iter->second;
            if (drawable->loaded) {
                drawable->onDrawFrame();
            }
        }
    }

    void Engine::rebindDrawableBuffers() {
        drawables_t::iterator iter;
        for(iter = this->drawables->begin(); iter != this->drawables->end(); iter++) {
            Drawable* drawable = iter->second;
            drawable->load();
            drawable->bindVertex();
        }
    }

    void Engine::unloadDrawables() {
        drawables_t::iterator iter;
        for(iter = this->drawables->begin(); iter != this->drawables->end(); iter++) {
            Drawable* drawable = iter->second;
            delete drawable;
            free((char*)iter->first);
        }
        this->drawables->clear();
    }

    void Engine::deleteDrawableBuffers() {
        drawables_t::iterator iter;
        for(iter = this->drawables->begin(); iter != this->drawables->end(); iter++) {
            Drawable* drawable = iter->second;
            drawable->deleteBuffer();
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
            AConfiguration_setOrientation(this->app->config, ACONFIGURATION_ORIENTATION_PORT);
            break;
        case OPT_ORIENTATION_LANDSCAPE:
            AConfiguration_setOrientation(this->app->config, ACONFIGURATION_ORIENTATION_LAND);
            break;
        }
    }

    void Engine::createSensors(int value) {

        if (this->sensorManager == NULL) {
            // Get a reference to the sensor manager
            this->sensorManager = ASensorManager_getInstance();

            // Creates a new sensor event queue
            this->sensorEventQueue = ASensorManager_createEventQueue(
                this->sensorManager, this->app->looper, LOOPER_ID_USER, NULL, NULL);
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

    }


    /*
     * enable sensor
     */
    void Engine::enableSensor(int sensorType, int interval) {

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

    /*
     * Register new class for emo framework
     */
    void Engine::registerClass(HSQUIRRELVM v, const char *cname) {
        register_class_with_namespace(v, EMO_NAMESPACE, cname);
    }

    /*
     * Register class method for emo framework
     */
    void Engine::registerClassFunc(HSQUIRRELVM v, const char *cname, const char *fname, SQFUNCTION func) {
        register_class_func_with_namespace(
            v, EMO_NAMESPACE, cname, fname, func);
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
    }

}

