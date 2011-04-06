#include <jni.h>
#include <errno.h>
#include <sys/timeb.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android_native_app_glue.h>

#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdaux.h>

#include <main.h>
#include <sqfunc.h>
#include <emo_engine.h>
#include <emo_engine_func.h>

/* 
 * global pointer to application engine
 */
struct engine *g_engine;

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
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

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    context = eglCreateContext(display, config, NULL, NULL);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;

    emo_init_display(engine);
    
    return 0;
}
/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {
    if (engine->display == NULL) {
        return;
    }
    emo_draw_frame(engine);
    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {

        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        return emo_event_motion(app, event);
    } else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        return emo_event_key(app, event);
    }
    return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            engine->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)engine->app->savedState) = engine->state;
            engine->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            if (engine->app->window != NULL) {
                engine_init_display(engine);
                engine_draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            if (engine->animating) {
                emo_lost_focus(engine);
            }
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            emo_gained_focus(engine);
            break;
        case APP_CMD_LOST_FOCUS:
            emo_lost_focus(engine);
            break;
        case APP_CMD_LOW_MEMORY:
            emo_low_memory(engine);
            break;
    }
}

/*
 * Main application entry point
 */
void android_main(struct android_app* state) {
    struct engine engine;

    app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    if (state->savedState != NULL) {
        engine.state = *(struct saved_state*)state->savedState;
    }

    drawables_t drawables;
    engine.drawables = &drawables;

    g_engine = &engine;

    // Initialize the framework
    emo_init_engine(&engine);

    while (1) {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // Process the user queue.
            if (ident == LOOPER_ID_USER) {
                if (engine.sensorEventQueue != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(
                            engine.sensorEventQueue, &event, 1) > 0) {
                        emo_event_sensors(&engine, &event);
                    }
                }
            }

            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                return;
            }
        }

        if (engine.animating) {
            engine_draw_frame(&engine);
        }
    }
}
