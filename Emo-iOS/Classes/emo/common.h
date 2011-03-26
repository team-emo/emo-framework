#define _UNICODE

#define EMO_LOG_TAG "EmoFramework"

#define SQUIRREL_VM_INITIAL_STACK_SIZE 1024
#define SQUIRREL_RUNTIME_SCRIPT "emoruntime.nut"
#define SQUIRREL_MAIN_SCRIPT "emomain.nut"

#define EMO_NAMESPACE  "emo"
#define EMO_RUNTIME_CLASS  "Runtime"
#define EMO_EVENT_CLASS    "Event"
#define EMO_DRAWABLE_CLASS "Drawable"

#define EMO_FUNC_ONLOAD         "onLoad"
#define EMO_FUNC_ONGAINED_FOUCS "onGainedFocus"
#define EMO_FUNC_ONLOST_FOCUS "  onLostFocus"
#define EMO_FUNC_ONDISPOSE      "onDispose"
#define EMO_FUNC_ONERROR        "onError"
#define EMO_FUNC_ONDRAW_FRAME   "onDrawFrame"
#define EMO_FUNC_ONLOW_MEMORY   "onLowMemory"
#define EMO_FUNC_MOTIONEVENT    "_OnMotionEvent"
#define EMO_FUNC_KEYEVENT       "_OnKeyEvent"

#define MOTION_EVENT_PARAMS_SIZE 8
#define KEY_EVENT_PARAMS_SIZE    8

#define ACCELEROMETER_EVENT_PARAMS_SIZE 4

#define EMO_NO_ERROR    0x0000
#define EMO_ERROR       0x0001

#define ERR_SCRIPT_LOAD      0x0100
#define ERR_SCRIPT_OPEN      0x0101
#define ERR_SCRIPT_COMPILE   0x0102
#define ERR_SCRIPT_CALL_ROOT 0x0103

#define OPT_ENABLE_PERSPECTIVE_NICEST   0x1000
#define OPT_ENABLE_PERSPECTIVE_FASTEST  0x1001
#define OPT_WINDOW_FORCE_FULLSCREEN     0x1002
#define OPT_WINDOW_KEEP_SCREEN_ON       0x1003

#define MOTION_EVENT_ACTION_DOWN            0
#define MOTION_EVENT_ACTION_UP              1
#define MOTION_EVENT_ACTION_MOVE            2
#define MOTION_EVENT_ACTION_CANCEL          3
#define MOTION_EVENT_ACTION_OUTSIDE         4
#define MOTION_EVENT_ACTION_POINTER_DOWN    5
#define MOTION_EVENT_ACTION_POINTER_UP      6

#define KEY_EVENT_ACTION_DOWN               0
#define KEY_EVENT_ACTION_UP                 1
#define KEY_EVENT_ACTION_MULTIPLE           2

#define META_NONE              0
#define META_ALT_ON            0x02
#define META_ALT_LEFT_ON       0x10
#define META_ALT_RIGHT_ON      0x20
#define META_SHIFT_ON          0x01
#define META_SHIFT_LEFT_ON     0x40
#define META_SHIFT_RIGHT_ON    0x80
#define META_SYM_ON            0x04

#define SENSOR_TYPE_ACCELEROMETER      1
#define SENSOR_TYPE_MAGNETIC_FIELD     2
#define SENSOR_TYPE_GYROSCOPE          4
#define SENSOR_TYPE_LIGHT              5
#define SENSOR_TYPE_PROXIMITY          8

#define SENSOR_STATUS_UNRELIABLE       0
#define SENSOR_STATUS_ACCURACY_LOW     1
#define SENSOR_STATUS_ACCURACY_MEDIUM  2
#define SENSOR_STATUS_ACCURACY_HIGH    3

#define SENSOR_STANDARD_GRAVITY            (9.80665f)
#define SENSOR_MAGNETIC_FIELD_EARTH_MAX    (60.0f)
#define SENSOR_MAGNETIC_FIELD_EARTH_MIN    (30.0f)
