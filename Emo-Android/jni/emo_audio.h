#include <SLES/OpenSLES.h>
#include "SLES/OpenSLES_Android.h"
#include <squirrel.h>

void createAudioEngine(int playerCount);
bool createAudioChannelFromAsset(const char* fname, struct AudioChannel* channel);
bool setAudioChannelState(struct AudioChannel* channel, SLuint32 state);
SLuint32 getAudioChannelState(struct AudioChannel* channel);
struct AudioChannel* getAudioChannel(int index);
void closeAudioChannel(struct AudioChannel* channel);
void closeAudioEngine();

bool playAudioChannel(struct AudioChannel* channel);
bool pauseAudioChannel(struct AudioChannel* channel);
bool stopAudioChannel(struct AudioChannel* channel);
struct AudioChannel* loadAudio(int channelIndex, const char* name);

SQInteger emoLoadAudio(HSQUIRRELVM v);
SQInteger emoCreateAudioEngine(HSQUIRRELVM v);
SQInteger emoPlayAudioChannel(HSQUIRRELVM v);
SQInteger emoPauseAudioChannel(HSQUIRRELVM v);
SQInteger emoStopAudioChannel(HSQUIRRELVM v);
SQInteger emoCloseAudioChannel(HSQUIRRELVM v);
SQInteger emoCloseAudioEngine(HSQUIRRELVM v);

