#include <android_native_app_glue.h>

void    emo_init_engine(engine* engine);
void    emo_init_display(engine* engine);
void    emo_term_display(engine* engine);
void    emo_draw_frame(engine* engine);
void    emo_dispose_engine(engine* engine);
int32_t emo_event_motion(struct android_app* app, AInputEvent* event);
int32_t emo_event_key(struct android_app* app, AInputEvent* event);
int32_t emo_event_sensors(engine* engine, ASensorEvent* event);
void    emo_gained_focus(engine* engine);
void    emo_lost_focus(engine* engine);
void    emo_low_memory(engine* engine);
