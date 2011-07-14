// Copyright (c) 2011 emo-framework project
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the project nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
#ifndef EMO_AUDIO_H
#define EMO_AUDIO_H

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
        bool resumeChannel(int index);
        bool pauseChannel(int index);
        bool stopChannel(int index);

        bool isLoaded(int channelIndex);

        bool getChannelLooping(int index);
        bool setChannelLooping(int index, SQBool enable);

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

void initAudioFunctions();

SQInteger emoLoadAudio(HSQUIRRELVM v);
SQInteger emoCreateAudioEngine(HSQUIRRELVM v);
SQInteger emoPlayAudioChannel(HSQUIRRELVM v);
SQInteger emoResumeAudioChannel(HSQUIRRELVM v);
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
SQInteger emoAudioVibrate(HSQUIRRELVM v);
#endif // EMO_AUDIO_H
