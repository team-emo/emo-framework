#import "EmoAudio.h"
#import "EmoEngine.h"
#import "common.h"
#import "emo_engine_func.h"

extern EmoEngine* engine;

@implementation EmoAudioChannel
@synthesize loaded;

-(void)setBuffer:(ALuint)_buffer {
	buffer = _buffer;
}

-(void)setSource:(ALuint)_source {
	source = _source;
}

-(BOOL)seek:(ALfloat)offset {
    if (!loaded) {
        engine.lastError = ERR_AUDIO_CHANNEL_CLOSED;
        LOGE("emo_audio: audio channel is closed");
        return FALSE;
    }
	alSourcef(source, AL_SAMPLE_OFFSET, offset);

	return TRUE;
}

-(BOOL)play {
    if (!loaded) {
        engine.lastError = ERR_AUDIO_CHANNEL_CLOSED;
        LOGE("emo_audio: audio channel is closed");
        return FALSE;
    }
	alSourceRewind(source);
    alSourcePlay(source);
	return TRUE;
}
-(BOOL)pause {
    if (!loaded) {
        engine.lastError = ERR_AUDIO_CHANNEL_CLOSED;
        LOGE("emo_audio: audio channel is closed");
        return FALSE;
    }
	alSourcePause(source);
	return TRUE;
}
-(BOOL)stop {
    if (!loaded) {
        engine.lastError = ERR_AUDIO_CHANNEL_CLOSED;
        LOGE("emo_audio: audio channel is closed");
        return FALSE;
    }
	alSourceStop(source);
	return TRUE;	
}

-(BOOL)loadFromAsset:(const SQChar*)fname {
	loaded = TRUE;
	
	
	return TRUE; // TODO
}

-(ALfloat)getVolume {
	return  0;
}
-(ALfloat)setVolume:(ALfloat)volumeLevel {
	return 0;
}
-(ALfloat)getMaxVolume {
	return 0;
}
-(ALfloat)getMinVolume {
	return 0;
}
-(void)close {
	[self stop];
	loaded = FALSE;
}
@end

@implementation EmoAudioManager

- (id)init {
    self = [super init];
    if (self != nil) {
		audioEngineCreated = FALSE;
    }
    return self;
}

- (BOOL)createChannels:(NSInteger)count {
    if (audioEngineCreated) {
        engine.lastError = ERR_AUDIO_ENGINE_CREATED;
        LOGE("emo_audio: audio engine is already created.");
        return FALSE;
    }
	
	buffers = (ALuint *)malloc(sizeof(ALuint) * count);
	sources = (ALuint *)malloc(sizeof(ALuint) * count);
	
    alGenBuffers(count, buffers);
    alGenSources(count, sources);
	
	channels = [NSMutableArray array];
	for (int i = 0; i < count; i++) {
		EmoAudioChannel* channel = [[EmoAudioChannel alloc]init];
		[channel setBuffer:buffers[i]];
		[channel setSource:sources[i]];
		[channels addObject:channel];
	}
	audioEngineCreated = TRUE;
	return TRUE;
}

-(EmoAudioChannel*) getAudioChannel:(NSInteger)index {
	if ([channels count] <= index) {
        engine.lastError = ERR_INVALID_PARAM;
        NSLOGE(@"emo_audio: invalid audio channel index");
		return nil;
	}
	return [channels objectAtIndex:index];
}

-(void)closeEngine {
	if (audioEngineCreated) {
		for (int i = 0; i < [channels count]; i++) {
			EmoAudioChannel* channel = [channels objectAtIndex:i];
			[channel stop];
			[channel close];
			[channel release];
		}
		
		alDeleteBuffers([channels count], buffers);
		alDeleteSources([channels count], sources);
		
		free(buffers);
		free(sources);
		
		[channels removeAllObjects];
	}
	audioEngineCreated = FALSE;
}

-(BOOL)isAudioEngineRunning {
	return audioEngineCreated;
}

-(NSInteger)getChannelCount {
	return [channels count];
}

- (void)dealloc {
	[channels release];
	[super dealloc]; 
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
	
    EmoAudioChannel* channel = [engine.audioManager getAudioChannel:channelIndex];
	
    if (![channel loadFromAsset:filename]) {
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
	
    if (![[engine.audioManager getAudioChannel:channelIndex] play]) {
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
	
    if (![[engine.audioManager getAudioChannel:channelIndex] pause]) {
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
	
    if (![[engine.audioManager getAudioChannel:channelIndex] stop]) {
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
	
    if (![[engine.audioManager getAudioChannel:channelIndex] seek:seekPosition]) {
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
	
    EmoAudioChannel* channel = [engine.audioManager getAudioChannel:channelIndex];
	
    if (!channel.loaded) {
        LOGE("emo_audio: audio channel is closed");
        sq_pushinteger(v, 0);
        return 1;
    }
	
    sq_pushfloat(v, [channel getVolume]);
	
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
	
    if (![[engine.audioManager getAudioChannel:channelIndex] setVolume:channelVolume]) {
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
	
    EmoAudioChannel* channel = [engine.audioManager getAudioChannel:channelIndex];
	
    if (!channel.loaded) {
        LOGE("emo_audio: audio channel is closed");
        sq_pushinteger(v, 0);
        return 1;
    }
	
    sq_pushfloat(v, [channel getMaxVolume]);
	
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
	
    EmoAudioChannel* channel = [engine.audioManager getAudioChannel:channelIndex];
	
    if (!channel.loaded) {
        LOGE("emo_audio: audio channel is closed");
        sq_pushinteger(v, 0);
        return 1;
    }
	
    sq_pushfloat(v, [channel getMinVolume]);
	
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
	
    EmoAudioChannel* channel = [engine.audioManager getAudioChannel:channelIndex];
	
    if (!channel.loaded) {
        sq_pushinteger(v, ERR_AUDIO_CHANNEL_CLOSED);
        return 1;
    }
	
    [channel close];
	
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
