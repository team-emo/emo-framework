#import "squirrel.h"
#import "OpenAL/al.h"
#import "OpenAL/alc.h"
#import <AudioToolbox/AudioToolbox.h>

@interface EmoAudioManager : NSObject {
	BOOL audioEngineCreated;
	BOOL  *  loaded;
    ALuint*  buffers;
    ALuint*  sources;
	NSInteger audioChannelCount;
}
@property (readwrite) NSInteger audioChannelCount;

-(BOOL)createChannels:(NSInteger)count;
-(BOOL)loadChannelFromAsset:(NSInteger)index withFile:(const SQChar *)fname;
-(BOOL)closeChannel:(NSInteger)index;
-(void)closeEngine;
-(BOOL)isAudioEngineRunning;
-(NSInteger)getChannelCount;

-(BOOL)seekChannel:(NSInteger)index withOffset:(ALfloat)offset;
-(BOOL)playChannel:(NSInteger)index;
-(BOOL)pauseChannel:(NSInteger)index;
-(BOOL)stopChannel:(NSInteger)index;
-(BOOL)channelLoaded:(NSInteger)index;

-(ALfloat)getChannelVolume:(NSInteger)index;
-(ALfloat)setChannelVolume:(NSInteger)index withVolume:(ALfloat)volumeLevel;
-(ALfloat)getChannelMaxVolume:(NSInteger)index;
-(ALfloat)getChannelMinVolume:(NSInteger)index;

@end

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