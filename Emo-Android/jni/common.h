#include <EGL/egl.h>

#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,  "EmoFramework", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN,  "EmoFramework", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "EmoFramework", __VA_ARGS__))

#define _UNICODE

/**
 * Our saved state data.
 */
struct saved_state {
    int32_t x;
    int32_t y;
};

/**
 * Shared state for our app.
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
    
    struct saved_state state;
};
