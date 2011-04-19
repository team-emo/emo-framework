#import "squirrel.h"

SQInteger emoLoadAudio(HSQUIRRELVM v);
SQInteger emoCreateAudioEngine(HSQUIRRELVM v);
SQInteger emoPlayAudioChannel(HSQUIRRELVM v);
SQInteger emoPauseAudioChannel(HSQUIRRELVM v);
SQInteger emoStopAudioChannel(HSQUIRRELVM v);
SQInteger emoSeekAudioChannel(HSQUIRRELVM v);
SQInteger emoCloseAudioChannel(HSQUIRRELVM v);
SQInteger emoCloseAudioEngine(HSQUIRRELVM v);

SQInteger emoGetAudioChannelCount(HSQUIRRELVM v);
SQInteger emoGetAudioChannelVolume(HSQUIRRELVM v);
SQInteger emoSetAudioChannelVolume(HSQUIRRELVM v);
SQInteger emoGetAudioChannelMaxVolume(HSQUIRRELVM v);
SQInteger emoGetAudioChannelMinVolume(HSQUIRRELVM v);

SQInteger emoSetAudioChannelLooping(HSQUIRRELVM v);
SQInteger emoGetAudioChannelLooping(HSQUIRRELVM v);
SQInteger emoGetAudioChannelState(HSQUIRRELVM v);
