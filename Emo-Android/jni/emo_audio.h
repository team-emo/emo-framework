#include <SLES/OpenSLES.h>
#include "SLES/OpenSLES_Android.h"
#include <squirrel.h>

void createAudioEngine(int playerCount);
bool createAudioChannelFromAsset(const char* fname, AudioChannel* channel);
bool setAudioChannelState(AudioChannel* channel, SLuint32 state);
SLuint32 getAudioChannelState(AudioChannel* channel);
AudioChannel* getAudioChannel(int index);
void closeAudioChannel(AudioChannel* channel);
void closeAudioEngine();
bool isAudioEngineRunning();

SLmillibel getAudioChannelVolume(AudioChannel* channel);
SLmillibel setAudioChannelVolume(AudioChannel* channel, SLmillibel volumeLevel);
SLmillibel getAudioChannelMaxVolume(AudioChannel* channel);
SQInteger emoGetAudioChannelCount(HSQUIRRELVM v);

bool seekAudioChannel(AudioChannel* channel, int pos);
bool playAudioChannel(AudioChannel* channel);
bool pauseAudioChannel(AudioChannel* channel);
bool stopAudioChannel(AudioChannel* channel);
AudioChannel* loadAudio(int channelIndex, const char* name);

bool getAudioChannelLooping(AudioChannel* channel);
bool setAudioChannelLooping(AudioChannel* channel, SQInteger enable);

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
