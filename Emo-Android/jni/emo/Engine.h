#include <android_native_app_glue.h>
#include <android/sensor.h>

#include <hash_map>
#include <sys/timeb.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include "squirrel.h"

#include "Constants.h"
#include "Stage.h"
#include "Drawable.h"
#include "Drawable_glue.h"
#include "Audio.h"

struct char_comparator {
    bool operator()(const char* s1, const char* s2) const {
        return strcmp(s1, s2) == 0;
    }
};

typedef std::hash_map <const char *, emo::Drawable *, std::hash<const char*>, char_comparator> drawables_t;

namespace emo {
    class Engine {
    public:
        Engine();
        ~Engine();

        int32_t event_handle_input(android_app* app, AInputEvent* event);
        void    event_handle_cmd(android_app* app, int32_t cmd);

        int32_t onInitDisplay();
        void    onInitGLSurface();

        void onInitEngine();
        void onDispose();

        void onTerminateDisplay();

        void onDrawFrame();
        void onLostFocus();
        void onGainedFocus();
        void onLowMemory();

        void updateUptime();
        int32_t getLastOnDrawDelta();
        int32_t getLastOnDrawDrawablesDelta();

        int32_t onSensorEvent(ASensorEvent* event);
        int32_t onMotionEvent(android_app* app, AInputEvent* event);
        int32_t onKeyEvent(android_app* app, AInputEvent* event);

        ASensorEventQueue* getSensorEventQueue();

        void updateOptions(int value);
        void createSensors(int value);
        void enableSensor(int sensorType, int interval);
        void disableSensor(int sensorType);

        void enableOnDrawListener(bool enable);
        void setOnDrawListenerInterval(int value);

        void addDrawable(const char* _key, Drawable* drawable);
        void addDrawableToRemove(const char* _key, Drawable* drawable);
        bool removeDrawable(const char* key);
        bool freeDrawable(const char* key);
        Drawable* getDrawable(const char* key);

        void onDrawDrawables();
        
        void rebindDrawableBuffers();

        void loadDrawables();
        void unloadDrawables();
        void deleteDrawableBuffers();

        int32_t getLastError();
        void setLastError(int32_t error);

        HSQUIRRELVM sqvm;

        bool animating;

        android_app* app;
        Audio* audio;
        Stage* stage;
        timeb uptime;

        int32_t onDrawFrameInterval;
        int32_t onDrawDrawablesInterval;

        void registerClass(HSQUIRRELVM v, const char *cname);
        void registerClassFunc(HSQUIRRELVM v, const char *cname, const char *fname, SQFUNCTION func);
    protected:
        bool loaded;
        bool focused;
        bool loadedCalled;
        bool initialized;

        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;

        int32_t width;
        int32_t height;
        int32_t lastError;

        timeb startTime;

        float touchEventParamCache[MOTION_EVENT_PARAMS_SIZE];
        float keyEventParamCache[KEY_EVENT_PARAMS_SIZE];
        float accelerometerEventParamCache[ACCELEROMETER_EVENT_PARAMS_SIZE];

        timeb   lastOnDrawInterval;

        timeb   lastOnDrawDrawablesInterval;

        bool enablePerspectiveNicest;
        bool enableOnDrawFrame;
        bool enableBackKey;

        drawables_t* drawables;
        drawables_t* drawablesToRemove;

        ASensorManager* sensorManager;
        ASensorEventQueue* sensorEventQueue;

        const ASensor* accelerometerSensor;
        const ASensor* magneticSensor;
        const ASensor* gyroscopeSensor;
        const ASensor* lightSensor;
        const ASensor* proximitySensor;

        void initScriptFunctions();
    };
}