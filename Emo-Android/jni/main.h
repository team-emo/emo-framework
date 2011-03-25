#include <sys/timeb.h>
#include <EGL/egl.h>

#include <android/sensor.h>
#include <android/log.h>

#include <common.h>

/**
 * Saved state data.
 */
struct saved_state {
    int32_t x;
    int32_t y;
};

/**
 * Shared state for application.
 */
struct engine {
    struct android_app* app;

    int animating;
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

    bool enablePerspectiveNicest;
    bool enableOnDrawFrame;

    ASensorManager* sensorManager;
    ASensorEventQueue* sensorEventQueue;

    const ASensor* accelerometerSensor;
    const ASensor* magneticSensor;
    const ASensor* gyroscopeSensor;
    const ASensor* lightSensor;
    const ASensor* proximitySensor;

    struct saved_state state;
};
