#include <android_native_app_glue.h>

void    emo_init_engine(struct engine* engine);
void    emo_init_display(struct engine* engine);
void    emo_term_display(struct engine* engine);
void    emo_draw_frame(struct engine* engine);
void    emo_dispose_engine(struct engine* engine);
int32_t emo_event_motion(struct android_app* app, AInputEvent* event);
int32_t emo_event_key(struct android_app* app, AInputEvent* event);
int32_t emo_event_sensors(struct engine* engine, ASensorEvent* event);
void    emo_gained_focus(struct engine* engine);
void    emo_lost_focus(struct engine* engine);
void    emo_low_memory(struct engine* engine);
