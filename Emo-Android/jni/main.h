#include <hash_map>
#include <sys/timeb.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

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


struct ImageInfo {
    const char* filename;
    GLuint   textureId;
    int      width;
    int      height;
    GLubyte* data;
    bool     hasAlpha;
};

struct Drawable {
    const char* name;

    unsigned int vbo[3];

    float      data_positions[12];
    float      data_tex_coords[8];
    short      data_indices[4];

    bool       hasTexture;
    bool       removed;
    bool       loaded;
    bool       visible;

    float      x;
    float      y;

    float      param_translate[3];
    float      param_rotate[4];
    float      param_scale[4];
    float      param_color[4];

    ImageInfo* texture;
};

struct char_comparator {
    bool operator()(const char* s1, const char* s2) const {
        return strcmp(s1, s2) == 0;
    }
};

typedef std::hash_map <const char *, Drawable *, std::hash<const char*>, char_comparator> drawables_t;

/**
 * Shared state for application.
 */
struct engine {
    struct android_app* app;

    bool loaded;
    bool focused;
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
    bool enableBackKey;

    ASensorManager* sensorManager;
    ASensorEventQueue* sensorEventQueue;

    const ASensor* accelerometerSensor;
    const ASensor* magneticSensor;
    const ASensor* gyroscopeSensor;
    const ASensor* lightSensor;
    const ASensor* proximitySensor;

    struct saved_state state;

    drawables_t *drawables;
};
