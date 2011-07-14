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
#import "EmoAudio_glue.h"
#import "EmoEngine_glue.h"
#import "Constants.h"
#import "EmoEngine.h"
#import "VmFunc.h"

extern EmoEngine* engine;

void initAudioFunctions() {
	registerClass(engine.sqvm, EMO_AUDIO_CLASS);

	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "constructor",    emoCreateAudioEngine);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "load",           emoLoadAudio);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "play",           emoPlayAudioChannel);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "pause",          emoPauseAudioChannel);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "resume_play",    emoResumeAudioChannel);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "stop",           emoStopAudioChannel);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "seek",           emoSeekAudioChannel);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "getChannelCount",emoGetAudioChannelCount);

	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "getVolume",      emoGetAudioChannelVolume);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "setVolume",      emoSetAudioChannelVolume);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "getMaxVolume",   emoGetAudioChannelMaxVolume);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "getMinVolume",   emoGetAudioChannelMinVolume);

	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "setLoop",        emoSetAudioChannelLooping);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "isLoop",         emoGetAudioChannelLooping);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "getState",       emoGetAudioChannelState);

	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "close",          emoCloseAudioChannel);
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "closeEngine",    emoCloseAudioEngine);
    
	registerClassFunc(engine.sqvm, EMO_AUDIO_CLASS,    "vibrate",        emoAudioVibrate);
}

/*
 * SQInteger loadAudio(SQInteger audioIndex, SQChar* filename);
 */
SQInteger emoLoadAudio(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }
	
    SQInteger channelIndex;
    const SQChar* filename;
	
    if (sq_gettype(v, 2) != OT_NULL && sq_gettype(v, 3) == OT_STRING) {
        sq_getinteger(v, 2, &channelIndex);
        sq_tostring(v, 3);
        sq_getstring(v, -1, &filename);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (![engine.audioManager loadChannelFromAsset:channelIndex withFile:filename]) {
        sq_pushinteger(v, engine.lastError);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}

SQInteger emoCreateAudioEngine(HSQUIRRELVM v) {
    if ([engine.audioManager isAudioEngineRunning]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CREATED);
        return 1;
    }
	
    SQInteger channelCount;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getinteger(v, 2, &channelCount);
    } else {
        channelCount = DEFAULT_AUDIO_CHANNEL_COUNT;
    }
	
    if (![engine.audioManager createChannels:channelCount]) {
        sq_pushinteger(v, engine.lastError);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}

SQInteger emoPlayAudioChannel(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }
	
    SQInteger channelIndex;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (![engine.audioManager playChannel:channelIndex]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_STATUS);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}
SQInteger emoResumeAudioChannel(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }
	
    SQInteger channelIndex;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (![engine.audioManager resumeChannel:channelIndex]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_STATUS);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}

SQInteger emoPauseAudioChannel(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }
	
    SQInteger channelIndex;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (![engine.audioManager pauseChannel:channelIndex]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_STATUS);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}

SQInteger emoStopAudioChannel(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }
	
    SQInteger channelIndex;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (![engine.audioManager stopChannel:channelIndex]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_STATUS);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}


SQInteger emoSeekAudioChannel(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }
	
    SQInteger channelIndex;
    SQFloat seekPosition;
	
    if (sq_gettype(v, 2) != OT_NULL && sq_gettype(v, 3) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
        sq_getfloat(v, 3, &seekPosition);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (![engine.audioManager seekChannel:channelIndex withOffset:seekPosition]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_STATUS);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}

SQInteger emoGetAudioChannelVolume(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        LOGE("emoGetAudioChannelVolume: audio engine is closed");
        sq_pushinteger(v, 0);
        return 1;
    }
	
    SQInteger channelIndex;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, 0);
        LOGE("emoGetAudioChannelVolume: invalid parameter type");
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        LOGE("emoGetAudioChannelVolume: invalid channel index");
        sq_pushinteger(v, 0);
        return 1;
    }
	
    if (![engine.audioManager channelLoaded:channelIndex]) {
        LOGE("emo_audio: audio channel is closed");
        sq_pushinteger(v, 0);
        return 1;
    }
	
    sq_pushfloat(v, [engine.audioManager getChannelVolume:channelIndex]);
	
    return 1;
}

SQInteger emoSetAudioChannelVolume(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }
	
    SQInteger channelIndex;
    SQFloat   channelVolume;
	
    if (sq_gettype(v, 2) != OT_NULL && sq_gettype(v, 3) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
        sq_getfloat(v, 3, &channelVolume);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (![engine.audioManager setChannelVolume:channelIndex withVolume:channelVolume]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_STATUS);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}

SQInteger emoGetAudioChannelMaxVolume(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        LOGE("emoGetAudioChannelVolume: audio engine is closed");
        sq_pushinteger(v, 0);
        return 1;
    }
	
    SQInteger channelIndex;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, 0);
        LOGE("emoGetAudioChannelVolume: invalid parameter type");
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        LOGE("emoGetAudioChannelVolume: invalid channel index");
        sq_pushinteger(v, 0);
        return 1;
    }
	
    if (![engine.audioManager channelLoaded:channelIndex]) {
        LOGE("emo_audio: audio channel is closed");
        sq_pushinteger(v, 0);
        return 1;
    }
	
    sq_pushfloat(v, [engine.audioManager getChannelMaxVolume:channelIndex]);
	
    return 1;
}

SQInteger emoGetAudioChannelCount(HSQUIRRELVM v) {
    sq_pushinteger(v, [engine.audioManager getChannelCount]);
    return 1;
	
}

SQInteger emoGetAudioChannelMinVolume(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        LOGE("emoGetAudioChannelVolume: audio engine is closed");
        sq_pushinteger(v, 0);
        return 1;
    }
	
    SQInteger channelIndex;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, 0);
        LOGE("emoGetAudioChannelVolume: invalid parameter type");
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        LOGE("emoGetAudioChannelVolume: invalid channel index");
        sq_pushinteger(v, 0);
        return 1;
    }
	
    if (![engine.audioManager channelLoaded:channelIndex]) {
        LOGE("emo_audio: audio channel is closed");
        sq_pushinteger(v, 0);
        return 1;
    }
	
    sq_pushfloat(v, [engine.audioManager getChannelMinVolume:channelIndex]);
	
    return 1;
}


SQInteger emoCloseAudioChannel(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }
	
    SQInteger channelIndex;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (![engine.audioManager closeChannel:channelIndex]) {
        sq_pushinteger(v, ERR_AUDIO_CHANNEL_CLOSED);
        return 1;
	}
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}

SQInteger emoSetAudioChannelLooping(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }
	
    SQInteger channelIndex;
    SQBool useLoop;
	
    if (sq_gettype(v, 2) != OT_NULL && getBool(v, 3, &useLoop)) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM_TYPE);
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
	
    if (![engine.audioManager setChannelLooping:channelIndex withLoop:useLoop]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_STATUS);
        return 1;
    }
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}

SQInteger emoGetAudioChannelLooping(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        LOGE("emoGetAudioChannelLooping: audio engine is closed");
        sq_pushbool(v, false);
        return 1;
    }
	
    SQInteger channelIndex;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushbool(v, false);
        LOGE("emoGetAudioChannelLooping: invalid parameter type");
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        LOGE("emoGetAudioChannelLooping: invalid channel index");
        sq_pushbool(v, false);
        return 1;
    }
	
    sq_pushbool(v, [engine.audioManager getChannelLooping:channelIndex]);
	
    return 1;
	
}

SQInteger emoGetAudioChannelState(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        LOGE("emoGetAudioChannelState: audio engine is closed");
        sq_pushinteger(v, AUDIO_CHANNEL_STOPPED);
        return 1;
    }
	
    SQInteger channelIndex;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, AUDIO_CHANNEL_STOPPED);
        LOGE("emoGetAudioChannelState: invalid parameter type");
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        LOGE("emoGetAudioChannelState: invalid channel index");
        sq_pushinteger(v, AUDIO_CHANNEL_STOPPED);
        return 1;
    }
	
    ALenum state = [engine.audioManager getChannelState:channelIndex];
	
    switch(state) {
		case AL_STOPPED:
			sq_pushinteger(v, AUDIO_CHANNEL_STOPPED);
			break;
		case AL_PAUSED:
			sq_pushinteger(v, AUDIO_CHANNEL_PAUSED);
			break;
		case AL_PLAYING:
			sq_pushinteger(v, AUDIO_CHANNEL_PLAYING);
			break;
    }
	
    return 1;
	
}

SQInteger emoCloseAudioEngine(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }
	
    [engine.audioManager closeEngine];
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}

/*
 * vibrate
 */
SQInteger emoAudioVibrate(HSQUIRRELVM v) {
    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
    return 0;
}
