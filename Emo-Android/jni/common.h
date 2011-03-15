#include <EGL/egl.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,  "EmoFramework", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN,  "EmoFramework", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "EmoFramework", __VA_ARGS__))

#define _UNICODE

#define ERR_SCRIPT_LOAD      0x0100
#define ERR_SCRIPT_OPEN      0x0101
#define ERR_SCRIPT_COMPILE   0x0102
#define ERR_SCRIPT_CALL_ROOT 0x0103

#define OPTION_ENABLE_PERSPECTIVE_NICEST   0x0200
#define OPTION_ENABLE_PERSPECTIVE_FASTEST  0x0201

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

    SQBool enableSQOnDrawFrame;

    struct saved_state state;
};


