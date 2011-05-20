#import "EmoAudio.h"
#import "EmoEngine.h"
#import "Constants.h"
#import "EmoEngine_glue.h"

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
-(BOOL)loadChannelFromAsset:(NSInteger)index withFile:(const char *)fname {
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
	
	bool    audioLoaded;
	void*   audioData;
	ALsizei dataSize;
	ALenum  dataFormat;
	ALsizei sampleRate;
	audioData = GetOpenALAudioData((CFURLRef)[NSURL fileURLWithPath:path], &dataSize, &dataFormat, &sampleRate, &audioLoaded);

	if (!audioLoaded) return FALSE;
		
	alBufferData(buffers[index], dataFormat, audioData, dataSize, sampleRate);
	if ([self isALError:alGetError() at:@"alBufferData"]) {
		free(audioData);
		LOGE(fname);
		return FALSE;
	}
	alSourcei(sources[index], AL_BUFFER, buffers[index]);
	if ([self isALError:alGetError() at:@"alSourcei"]) {
		free(audioData);
		LOGE(fname);
		return FALSE;
	}
	
	free(audioData);
	
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
		} else if (error == AL_INVALID_OPERATION) {
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

-(BOOL)getChannelLooping:(NSInteger)index {
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
	ALint enabled;
	alGetSourcei(sources[index], AL_LOOPING, &enabled);
	
	return enabled != 0;
}

-(BOOL)setChannelLooping:(NSInteger)index withLoop:(BOOL) enable {
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
	alSourcei(sources[index], AL_LOOPING, enable ? AL_TRUE : AL_FALSE);
	return TRUE;
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
