#import "OpenAL/al.h"
#import "OpenAL/alc.h"
#import <AudioToolbox/AudioToolbox.h>

@interface EmoAudioManager : NSObject {
	BOOL audioEngineCreated;
	BOOL  *  loaded;
    ALuint*  buffers;
    ALuint*  sources;
	NSInteger audioChannelCount;
    ALCdevice*  device;
    ALCcontext* context;
}
@property (readwrite) NSInteger audioChannelCount;

-(BOOL)createChannels:(NSInteger)count;
-(BOOL)loadChannelFromAsset:(NSInteger)index withFile:(const char *)fname;
-(BOOL)closeChannel:(NSInteger)index;
-(void)closeEngine;
-(BOOL)isAudioEngineRunning;
-(NSInteger)getChannelCount;

-(BOOL)seekChannel:(NSInteger)index withOffset:(ALfloat)offset;
-(BOOL)playChannel:(NSInteger)index;
-(BOOL)pauseChannel:(NSInteger)index;
-(BOOL)stopChannel:(NSInteger)index;
-(BOOL)channelLoaded:(NSInteger)index;
-(ALenum)getChannelState:(NSInteger)index;

-(ALfloat)getChannelVolume:(NSInteger)index;
-(ALfloat)setChannelVolume:(NSInteger)index withVolume:(ALfloat)volumeLevel;
-(ALfloat)getChannelMaxVolume:(NSInteger)index;
-(ALfloat)getChannelMinVolume:(NSInteger)index;

-(BOOL)getChannelLooping:(NSInteger)index;
-(BOOL)setChannelLooping:(NSInteger)index withLoop:(BOOL) enable;

@end
