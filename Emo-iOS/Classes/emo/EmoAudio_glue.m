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
    SQInteger useLoop;
	
    if (sq_gettype(v, 2) != OT_NULL && sq_gettype(v, 3) == OT_INTEGER) {
        sq_getinteger(v, 2, &channelIndex);
        sq_getinteger(v, 3, &useLoop);
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
        sq_pushinteger(v, EMO_NO);
        return 1;
    }
	
    SQInteger channelIndex;
	
    if (sq_gettype(v, 2) != OT_NULL) {
        sq_getinteger(v, 2, &channelIndex);
    } else {
        sq_pushinteger(v, EMO_NO);
        LOGE("emoGetAudioChannelLooping: invalid parameter type");
        return 1;
    }
	
    if (channelIndex >= [engine.audioManager getChannelCount]) {
        LOGE("emoGetAudioChannelLooping: invalid channel index");
        sq_pushinteger(v, EMO_NO);
        return 1;
    }
	
    if ([engine.audioManager getChannelLooping:channelIndex]) {
        sq_pushinteger(v, EMO_YES);
    } else {
        sq_pushinteger(v, EMO_NO);
    }
	
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