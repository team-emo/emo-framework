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
-(BOOL)resumeChannel:(NSInteger)index;
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
