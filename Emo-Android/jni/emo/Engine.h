#include <android_native_app_glue.h>
#include <android/sensor.h>

#include <hash_map>
#include <sys/timeb.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include "squirrel.h"

#include "Constants.h"
#include "Stage.h"
#include "Audio.h"

struct char_comparator {
    bool operator()(const char* s1, const char* s2) const {
        return strcmp(s1, s2) == 0;
    }
};

namespace emo {
    class Engine {
    public:
        Engine();
        ~Engine();

        int32_t event_handle_input(android_app* app, AInputEvent* event);
        void    event_handle_cmd(android_app* app, int32_t cmd);

        void setApp(android_app* app);

        int32_t onInitDisplay();
        void    onInitGLSurface();

        void onInitEngine();
        void onSensorEvent(ASensorEvent* event);
        void onDispose();

        void onTerminateDisplay();

        void onDrawFrame();
        void onLostFocus();
        void onGainedFocus();
        void onLowMemory();
        void updateUptime();

        int32_t onMotionEvent(android_app* app, AInputEvent* event);
        int32_t onKeyEvent(android_app* app, AInputEvent* event);

        ASensorEventQueue* getSensorEventQueue();

        void updateOptions(int value);
        void createSensors(int value);
        void enableSensor(int sensorType, int interval);
        void disableSensor(int sensorType);

        void enableOnDrawListener(bool enable);
        void setOnDrawListenerInterval(int value);

        bool isAnimating();
        void setAnimating(bool animating);

        void loadStage();
        void loadDrawables();

        void onDrawStage();
        
        void rebindStageBuffers();
        void rebindDrawableBuffers();

        void unloadDrawables();
        void deleteDrawableBuffers();
        void deleteStageBuffer();

        android_app* getApp();
        void setLastError(int32_t error);

        HSQUIRRELVM getVm();
    protected:
        bool animating;
        bool loaded;
        bool focused;
        bool loadedCalled;
        bool initialized;

        android_app* app;
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;

        int32_t width;
        int32_t height;
        HSQUIRRELVM sqvm;
        int32_t lastError;

        timeb startTime;
        timeb uptime;

        float touchEventParamCache[MOTION_EVENT_PARAMS_SIZE];
        float keyEventParamCache[KEY_EVENT_PARAMS_SIZE];
        float accelerometerEventParamCache[ACCELEROMETER_EVENT_PARAMS_SIZE];

        int32_t onDrawFrameInterval;
        timeb   lastOnDrawInterval;

        int32_t onDrawDrawablesInterval;
        timeb   lastOnDrawDrawablesInterval;

        bool enablePerspectiveNicest;
        bool enableOnDrawFrame;
        bool enableBackKey;

        Stage* stage;
        Audio* audio;

        ASensorManager* sensorManager;
        ASensorEventQueue* sensorEventQueue;

        const ASensor* accelerometerSensor;
        const ASensor* magneticSensor;
        const ASensor* gyroscopeSensor;
        const ASensor* lightSensor;
        const ASensor* proximitySensor;

        void initScriptFunctions();
        void registerClass(HSQUIRRELVM v, const char *cname);
        void registerClassFunc(HSQUIRRELVM v, const char *cname, const char *fname, SQFUNCTION func);
    };
}