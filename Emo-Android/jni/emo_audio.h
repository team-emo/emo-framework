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
bool isAudioEngineRunning();

SLmillibel getAudioChannelVolume(struct AudioChannel* channel);
SLmillibel setAudioChannelVolume(struct AudioChannel* channel, SLmillibel volumeLevel);
SLmillibel getAudioChannelMaxVolume(struct AudioChannel* channel);
SQInteger emoGetAudioChannelCount(HSQUIRRELVM v);

bool seekAudioChannel(struct AudioChannel* channel, int pos);
bool playAudioChannel(struct AudioChannel* channel);
bool pauseAudioChannel(struct AudioChannel* channel);
bool stopAudioChannel(struct AudioChannel* channel);
struct AudioChannel* loadAudio(int channelIndex, const char* name);

bool getAudioChannelLooping(struct AudioChannel* channel);
bool setAudioChannelLooping(struct AudioChannel* channel, SQInteger enable);

SQInteger emoLoadAudio(HSQUIRRELVM v);
SQInteger emoCreateAudioEngine(HSQUIRRELVM v);
SQInteger emoPlayAudioChannel(HSQUIRRELVM v);
SQInteger emoPauseAudioChannel(HSQUIRRELVM v);
SQInteger emoStopAudioChannel(HSQUIRRELVM v);
SQInteger emoSeekAudioChannel(HSQUIRRELVM v);
SQInteger emoCloseAudioChannel(HSQUIRRELVM v);
SQInteger emoCloseAudioEngine(HSQUIRRELVM v);

SQInteger emoGetAudioChannelVolume(HSQUIRRELVM v);
SQInteger emoSetAudioChannelVolume(HSQUIRRELVM v);
SQInteger emoGetAudioChannelMaxVolume(HSQUIRRELVM v);
SQInteger emoGetAudioChannelMinVolume(HSQUIRRELVM v);

SQInteger emoSetAudioChannelLooping(HSQUIRRELVM v);
SQInteger emoGetAudioChannelLooping(HSQUIRRELVM v);
SQInteger emoGetAudioChannelState(HSQUIRRELVM v);
