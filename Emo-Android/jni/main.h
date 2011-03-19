#include <EGL/egl.h>
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

    SQBool enablePerspectiveNicest;
    SQBool enableOnDrawFrame;

    struct saved_state state;
};
