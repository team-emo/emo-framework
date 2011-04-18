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
SQInteger emoPlayChannel(HSQUIRRELVM v);
SQInteger emoPauseChannel(HSQUIRRELVM v);
SQInteger emoStopChannel(HSQUIRRELVM v);
SQInteger emoSeekChannel(HSQUIRRELVM v);
SQInteger emoCloseChannel(HSQUIRRELVM v);
SQInteger emoCloseAudioEngine(HSQUIRRELVM v);

SQInteger emoGetChannelVolume(HSQUIRRELVM v);
SQInteger emoSetChannelVolume(HSQUIRRELVM v);
SQInteger emoGetChannelMaxVolume(HSQUIRRELVM v);
SQInteger emoGetChannelMinVolume(HSQUIRRELVM v);

SQInteger emoSetChannelLooping(HSQUIRRELVM v);
SQInteger emoGetChannelLooping(HSQUIRRELVM v);
SQInteger emoGetChannelState(HSQUIRRELVM v);
