#import "EmoAudio.h"
#import "EmoEngine.h"
#import "common.h"
#import "emo_engine_func.h"

extern EmoEngine* engine;

@interface EmoAudioManager (PrivateMethods)
-(BOOL)isALError:(ALenum)error at:(NSString*) at;
@end

@implementation EmoAudioManager
@synthesize audioChannelCount;

- (id)init {
    self = [super init];
    if (self != nil) {
		audioEngineCreated = FALSE;
		audioChannelCount = 0;
    }
    return self;
}

- (BOOL)createChannels:(NSInteger)count {
    if (audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CREATED;
        LOGE("emo_audio: audio engine is already created.");
        return FALSE;
    }

	// Open OpenAL device and context
    device = alcOpenDevice(NULL);
    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);
	
	buffers = (ALuint *)malloc(sizeof(ALuint) * count);
	sources = (ALuint *)malloc(sizeof(ALuint) * count);
	loaded  = (BOOL *)malloc(sizeof(BOOL) * count);
	
    alGenBuffers(count, buffers);
    alGenSources(count, sources);

	for (int i = 0; i < count; i++) {
		loaded[i] = FALSE;
	}
	audioEngineCreated = TRUE;
	audioChannelCount = count;
	return TRUE;
}
-(BOOL)loadChannelFromAsset:(NSInteger)index withFile:(const SQChar *)fname {
    if (!audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CLOSED;
        LOGE("emo_audio: audio engine is closed.");
        return FALSE;
    }
	
	// clear errors
	alGetError();
	
	if (loaded[index]) {
		[self closeChannel:index];
		// if this channel is re-loaded, new buffer needs to be reassigned.
		alGenBuffers(1, &buffers[index]);
	}

	NSString* fileName =  [[NSString alloc] initWithUTF8String:fname];
	NSString* path = [[NSBundle mainBundle] pathForResource:fileName ofType:nil];
	[fileName release];
	
	if (path == nil) {
		LOGE("Audio resource does not found:");
		LOGE(fname);
		return FALSE;
	}
	
	void*   audioData;
	ALsizei dataSize;
	ALenum  dataFormat;
	ALsizei sampleRate;
	audioData = GetOpenALAudioData((CFURLRef)[NSURL fileURLWithPath:path], &dataSize, &dataFormat, &sampleRate);
	
	alBufferData(buffers[index], dataFormat, audioData, dataSize, sampleRate);
	if ([self isALError:alGetError() at:@"alBufferData"]) {
		LOGE(fname);
		return FALSE;
	}
	alSourcei(sources[index], AL_BUFFER, buffers[index]);
	if ([self isALError:alGetError() at:@"alSourcei"]) {
		LOGE(fname);
		return FALSE;
	}
	
	loaded[index] = TRUE;
	
	return TRUE;
}
-(BOOL)isALError:(ALenum)error at:(NSString*) at {
	if (error != AL_NO_ERROR) {
		if (error == AL_OUT_OF_MEMORY) {
			LOGE("AL_OUT_OF_MEMORY at ");
		} else if (error == AL_INVALID_VALUE) {
			LOGE("AL_INVALID_VALUE at ");
		} else if (error == AL_INVALID_ENUM) {
			LOGE("AL_INVALID_ENUM at ");
		} else if (error == AL_INVALID_NAME) {
			LOGE("AL_INVALID_NAME at ");
		} else if (error = AL_INVALID_OPERATION) {
			LOGE("AL_INVALID_OPERATION at ");
		}
		NSLOGE(at);
		return TRUE;
	}
	return FALSE;
}
-(ALenum)getChannelState:(NSInteger)index {
    if (!audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CLOSED;
        LOGE("emo_audio: audio engine is closed.");
        return AL_STOPPED;
    }
	ALenum state;
	alGetSourcei(sources[index], AL_SOURCE_STATE, &state);
	return state;
}
-(BOOL)seekChannel:(NSInteger)index withOffset:(ALfloat)offset {
    if (!audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CLOSED;
        LOGE("emo_audio: audio engine is closed.");
        return FALSE;
    }
    if (!loaded[index]) {
        engine.lastError = ERR_AUDIO_CHANNEL_CLOSED;
        LOGE("emo_audio: audio channel is closed");
        return FALSE;
    }
	// fix offset because offset is millisecond basis
	alSourcef(sources[index], AL_SEC_OFFSET, offset / 1000.0f);
	
	return TRUE;
}

-(BOOL)playChannel:(NSInteger)index {
    if (!audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CLOSED;
        LOGE("emo_audio: audio engine is closed.");
        return FALSE;
    }
    if (!loaded[index]) {
        engine.lastError = ERR_AUDIO_CHANNEL_CLOSED;
        LOGE("emo_audio: audio channel is closed");
        return FALSE;
    }
	alSourceRewind(sources[index]);
    alSourcePlay(sources[index]);
	return TRUE;
}
-(BOOL)pauseChannel:(NSInteger)index {
    if (!audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CLOSED;
        LOGE("emo_audio: audio engine is closed.");
        return FALSE;
    }
    if (!loaded[index]) {
        engine.lastError = ERR_AUDIO_CHANNEL_CLOSED;
        LOGE("emo_audio: audio channel is closed");
        return FALSE;
    }
	alSourcePause(sources[index]);
	return TRUE;
}
-(BOOL)stopChannel:(NSInteger)index {
    if (!audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CLOSED;
        LOGE("emo_audio: audio engine is closed.");
        return FALSE;
    }
    if (!loaded[index]) {
        engine.lastError = ERR_AUDIO_CHANNEL_CLOSED;
        LOGE("emo_audio: audio channel is closed");
        return FALSE;
    }
	alSourceStop(sources[index]);
	return TRUE;	
}
-(ALfloat)getChannelVolume:(NSInteger)index {
    if (!audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CLOSED;
        LOGE("emo_audio: audio engine is closed.");
        return 0;
    }
    if (!loaded[index]) {
        engine.lastError = ERR_AUDIO_CHANNEL_CLOSED;
        LOGE("emo_audio: audio channel is closed");
        return 0;
    }
	ALfloat f;
	alGetSourcef(sources[index], AL_GAIN, &f);
	return  f;
}
-(ALfloat)setChannelVolume:(NSInteger)index withVolume:(ALfloat)volumeLevel {
    if (!audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CLOSED;
        LOGE("emo_audio: audio engine is closed.");
        return 0;
    }
    if (!loaded[index]) {
        engine.lastError = ERR_AUDIO_CHANNEL_CLOSED;
        LOGE("emo_audio: audio channel is closed");
        return 0;
    }
	alSourcef(sources[index], AL_GAIN, volumeLevel);
	
	return [self getChannelVolume:index];
}
-(ALfloat)getChannelMaxVolume:(NSInteger)index {
    if (!audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CLOSED;
        LOGE("emo_audio: audio engine is closed.");
        return 0;
    }
    if (!loaded[index]) {
        engine.lastError = ERR_AUDIO_CHANNEL_CLOSED;
        LOGE("emo_audio: audio channel is closed");
        return 0;
    }
	ALfloat f;
	alGetSourcef(sources[index], AL_MAX_GAIN, &f);
	return  f;
}
-(ALfloat)getChannelMinVolume:(NSInteger)index {
    if (!audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CLOSED;
        LOGE("emo_audio: audio engine is closed.");
        return 0;
    }
    if (!loaded[index]) {
        engine.lastError = ERR_AUDIO_CHANNEL_CLOSED;
        LOGE("emo_audio: audio channel is closed");
        return 0;
    }
	ALfloat f;
	alGetSourcef(sources[index], AL_MIN_GAIN, &f);
	return  f;
}

-(BOOL)closeChannel:(NSInteger)index {
    if (!audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CLOSED;
        LOGE("emo_audio: audio engine is closed.");
        return FALSE;
    }
	if (loaded[index]) {
		 [self stopChannel:index];
		 // detatch buffer from source
		 alSourcei(sources[index], AL_BUFFER, 0);
		 // delete buffer
		 alDeleteBuffers(1, &buffers[index]);
		 loaded[index] = FALSE;
		 return TRUE;
	 }
	return FALSE;
}

-(void)closeEngine {
	if (audioEngineCreated) {
		for (int i = 0; i < audioChannelCount; i++) {
			[self closeChannel:i];
		}
		
		alDeleteSources(audioChannelCount, sources);
		
		// close audio context and deivce
		alcMakeContextCurrent(NULL);
		alcDestroyContext(context);
		alcCloseDevice(device);
		
		free(buffers);
		free(sources);
		free(loaded);
	}
	audioEngineCreated = FALSE;
	audioChannelCount = 0;
}

-(BOOL)isAudioEngineRunning {
	return audioEngineCreated;
}

-(NSInteger)getChannelCount {
	return audioChannelCount;
}

-(BOOL)channelLoaded:(NSInteger)index {
	return loaded[index];
}

@end

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
	
    if (sq_gettype(v, 2) == OT_INTEGER && sq_gettype(v, 3) == OT_STRING) {
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
	
    if (sq_gettype(v, 2) == OT_INTEGER) {
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
	
    if (sq_gettype(v, 2) == OT_INTEGER) {
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
	
    if (sq_gettype(v, 2) == OT_INTEGER) {
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
	
    if (sq_gettype(v, 2) == OT_INTEGER) {
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
	
	SQObjectType vtype = sq_gettype(v, 3);
	
    if (sq_gettype(v, 2) == OT_INTEGER &&
		(vtype == OT_INTEGER || vtype == OT_FLOAT)) {
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
	
    if (sq_gettype(v, 2) == OT_INTEGER) {
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
	
	SQObjectType vtype = sq_gettype(v, 3);
	
    if (sq_gettype(v, 2) == OT_INTEGER &&
			(vtype == OT_INTEGER || vtype == OT_FLOAT)) {
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
	
    if (sq_gettype(v, 2) == OT_INTEGER) {
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
	
    if (sq_gettype(v, 2) == OT_INTEGER) {
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
	
    if (sq_gettype(v, 2) == OT_INTEGER) {
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

SQInteger emoCloseAudioEngine(HSQUIRRELVM v) {
    if (![engine.audioManager isAudioEngineRunning]) {
        sq_pushinteger(v, ERR_AUDIO_ENGINE_CLOSED);
        return 1;
    }
	
    [engine.audioManager closeEngine];
	
    sq_pushinteger(v, EMO_NO_ERROR);
	
    return 1;
}