#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <squirrel.h>

namespace emo {
    class Audio {
    public:
        Audio();
        ~Audio();

        bool isRunning();
        void close();

        bool create(int channelCount);
        bool createChannelFromAsset(const char* fname, int index);
        bool setChannelState(int index, SLuint32 state);
        SLuint32 getChannelState(int index);
        void closeChannel(int index);
        int getChannelCount();

        SLmillibel getChannelVolume(int index);
        SLmillibel setChannelVolume(int index, SLmillibel volumeLevel);
        SLmillibel getChannelMaxVolume(int index);

        bool seekChannel(int index, int pos, SLuint32 seekMode);
        bool playChannel(int index);
        bool pauseChannel(int index);
        bool stopChannel(int index);

        bool isLoaded(int channelIndex);

        bool getChannelLooping(int index);
        bool setChannelLooping(int index, SQInteger enable);

    protected:
        bool running;

        int channelCount;

        struct Channel {
            SLboolean     loaded;
            SLObjectItf   playerObject;
            SLPlayItf     playerPlay;
            SLSeekItf     playerSeek;
            SLVolumeItf   playerVolume;
        };

        Channel* channels;

        SLObjectItf engineObject;
        SLEngineItf engineEngine;
        SLObjectItf outputMixObject;
    };
}

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

SQInteger emoGetAudioChannelCount(HSQUIRRELVM v);
