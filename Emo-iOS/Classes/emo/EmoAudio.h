#import "squirrel.h"
#import "OpenAL/al.h"
#import "OpenAL/alc.h"

@interface EmoAudioChannel : NSObject {
	BOOL loaded;
}
@property (readonly) BOOL loaded;
-(BOOL)seek:(ALfloat)pos;
-(BOOL)play;
-(BOOL)pause;
-(BOOL)stop;

-(BOOL)loadFromAsset:(const SQChar *)fname;
-(ALfloat)getVolume;
-(ALfloat)setVolume:(ALfloat)volumeLevel;
-(ALfloat)getMaxVolume;
-(ALfloat)getMinVolume;
-(NSInteger)getStatus;
-(void)close;
@end


@interface EmoAudioManager : NSObject {
	BOOL audioEngineCreated;
	NSMutableArray* channels;
}

-(BOOL)createChannels:(NSInteger)count;
-(EmoAudioChannel*) getAudioChannel:(NSInteger)index;
-(void)closeEngine;
-(BOOL)isAudioEngineRunning;
-(NSInteger)getChannelCount;
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