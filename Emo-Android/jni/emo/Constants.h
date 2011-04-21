#define _UNICODE
#define PNG_NO_PEDANTIC_WARNINGS

#define EMO_LOG_TAG "EmoFramework"
#define OS_ANDROID "Android"
#define OS_IOS     "iOS"

#define SQUIRREL_VM_INITIAL_STACK_SIZE 1024
#define SQUIRREL_RUNTIME_SCRIPT "emoruntime.nut"
#define SQUIRREL_MAIN_SCRIPT "emomain.nut"

#define EMO_NAMESPACE  "emo"
#define EMO_RUNTIME_CLASS  "Runtime"
#define EMO_EVENT_CLASS    "Event"
#define EMO_STAGE_CLASS    "Stage"
#define EMO_AUDIO_CLASS    "Audio"

#define EMO_FUNC_ONLOAD         "_onLoad"
#define EMO_FUNC_ONGAINED_FOUCS "_onGainedFocus"
#define EMO_FUNC_ONLOST_FOCUS   "_onLostFocus"
#define EMO_FUNC_ONDISPOSE      "_onDispose"
#define EMO_FUNC_ONERROR        "_onError"
#define EMO_FUNC_ONDRAW_FRAME   "_onDrawFrame"
#define EMO_FUNC_ONLOW_MEMORY   "_onLowMemory"
#define EMO_FUNC_MOTIONEVENT    "_onMotionEvent"
#define EMO_FUNC_KEYEVENT       "_onKeyEvent"
#define EMO_FUNC_SENSOREVENT    "_onSensorEvent"

#define MOTION_EVENT_PARAMS_SIZE 8
#define KEY_EVENT_PARAMS_SIZE    8

#define ACCELEROMETER_EVENT_PARAMS_SIZE 4

#define DEFAULT_AUDIO_CHANNEL_COUNT 3
#define DRAWABLE_KEY_LENGTH 26

#define DEFAULT_ANIMATION_NAME "EMO::DEFAULT"
#define DEFAULT_DATABASE_NAME  "emoruntime.db"

#define EMO_NO              0
#define EMO_YES             1

#define EMO_NO_ERROR    0x0000
#define EMO_ERROR       0x0001
#define LOG_INFO        0x0002
#define LOG_ERROR       0x0003
#define LOG_WARN        0x0004

#define ERR_SCRIPT_LOAD           0x0100
#define ERR_SCRIPT_OPEN           0x0101
#define ERR_SCRIPT_COMPILE        0x0102
#define ERR_SCRIPT_CALL_ROOT      0x0103
#define ERR_ASSET_LOAD            0x0104
#define ERR_ASSET_OPEN            0x0105
#define ERR_ASSET_UNLOAD          0x0106
#define ERR_AUDIO_ENGINE_CREATED  0x0107
#define ERR_AUDIO_CHANNEL_CREATED 0x0108
#define ERR_AUDIO_ENGINE_INIT     0x0100
#define ERR_AUDIO_ASSET_INIT      0x0110
#define ERR_AUDIO_ENGINE_CLOSED   0x0111
#define ERR_AUDIO_CHANNEL_CLOSED  0x0112
#define ERR_AUDIO_ENGINE_STATUS   0x0113
#define ERR_INVALID_PARAM_COUNT   0x0114
#define ERR_INVALID_PARAM_TYPE    0x0115
#define ERR_INVALID_PARAM         0x0116
#define ERR_INVALID_ID            0x0117
#define ERR_FILE_OPEN             0x0118
#define ERR_CREATE_VERTEX         0x0119
#define ERR_NOT_SUPPORTED         0x0120

#define OPT_ENABLE_PERSPECTIVE_NICEST   0x1000
#define OPT_ENABLE_PERSPECTIVE_FASTEST  0x1001
#define OPT_WINDOW_FORCE_FULLSCREEN     0x1002
#define OPT_WINDOW_KEEP_SCREEN_ON       0x1003
#define OPT_ENABLE_BACK_KEY             0x1004
#define OPT_DISABLE_BACK_KEY            0x1005
#define OPT_ORIENTATION_PORTRAIT        0x1006
#define OPT_ORIENTATION_LANDSCAPE       0x1007

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

#define AUDIO_CHANNEL_STOPPED           1
#define AUDIO_CHANNEL_PAUSED            2
#define AUDIO_CHANNEL_PLAYING           3

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

#define MODE_PRIVATE         0x00000000
#define MODE_WORLD_READABLE  0x00000001
#define MODE_WORLD_WRITEABLE 0x00000002

bool isPowerOfTwo(int x);
int nextPowerOfTwo(int minimum);
