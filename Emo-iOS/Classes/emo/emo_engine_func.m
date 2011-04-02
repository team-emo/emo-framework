#import "emo_engine_func.h"
#import "common.h"

/*
 * Logging
 */
void LOGI(const char* msg) {
	NSLog(@"%s INFO %s", EMO_LOG_TAG, msg);
}

void LOGE(const char* msg) {
	NSLog(@"%s ERROR %s", EMO_LOG_TAG, msg);
}

void LOGW(const char* msg) {
	NSLog(@"%s WARN %s", EMO_LOG_TAG, msg);
}

void NSLOGI(NSString* msg) {
	LOGI([msg UTF8String]);	
}

void NSLOGE(NSString* msg) {
	LOGE([msg UTF8String]);	
}

void NSLOGW(NSString* msg) {
	LOGW([msg UTF8String]);	
}

void* GetOpenALAudioData(CFURLRef fileURL, ALsizei* dataSize,
						 ALenum* dataFormat, ALsizei *sampleRate) {
    OSStatus    err;
    UInt32      size;
    
    ExtAudioFileRef audioFile;
    err = ExtAudioFileOpenURL(fileURL, &audioFile);
    if (err) {
		LOGE("GetOpenALAudioData: failed to open audioFile");
        goto Exit;
    }
    
    AudioStreamBasicDescription fileFormat;
    size = sizeof(fileFormat);
    err = ExtAudioFileGetProperty(
								  audioFile, kExtAudioFileProperty_FileDataFormat, &size, &fileFormat);
    if (err) {
		LOGE("GetOpenALAudioData: failed to get fileFormat");
        goto Exit;
    }
    
    AudioStreamBasicDescription outputFormat;
    outputFormat.mSampleRate = fileFormat.mSampleRate;
    outputFormat.mChannelsPerFrame = fileFormat.mChannelsPerFrame;
    outputFormat.mFormatID = kAudioFormatLinearPCM;
    outputFormat.mBytesPerPacket = 2 * outputFormat.mChannelsPerFrame;
    outputFormat.mFramesPerPacket = 1;
    outputFormat.mBytesPerFrame = 2 * outputFormat.mChannelsPerFrame;
    outputFormat.mBitsPerChannel = 16;
    outputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
    err = ExtAudioFileSetProperty(
								  audioFile, kExtAudioFileProperty_ClientDataFormat, sizeof(outputFormat), &outputFormat);
    if (err) {
		LOGE("GetOpenALAudioData: failed to set outputFormat");
        goto Exit;
    }
    
    SInt64  fileLengthFrames = 0;
    size = sizeof(fileLengthFrames);
    err = ExtAudioFileGetProperty(
								  audioFile, kExtAudioFileProperty_FileLengthFrames, &size, &fileLengthFrames);
    if (err) {
		LOGE("GetOpenALAudioData: failed to get fileLengthFrames");
        goto Exit;
    }
    
    UInt32          bufferSize;
    void*           data;
    AudioBufferList dataBuffer;
    bufferSize = fileLengthFrames * outputFormat.mBytesPerFrame;;
    data = malloc(bufferSize);
    dataBuffer.mNumberBuffers = 1;
    dataBuffer.mBuffers[0].mDataByteSize = bufferSize;
    dataBuffer.mBuffers[0].mNumberChannels = outputFormat.mChannelsPerFrame;
    dataBuffer.mBuffers[0].mData = data;
    
    err = ExtAudioFileRead(audioFile, (UInt32*)&fileLengthFrames, &dataBuffer);
    if (err) {
		LOGE("GetOpenALAudioData: failed to read audioFile");
        free(data);
        goto Exit;
    }
    
    *dataSize = (ALsizei)bufferSize;
    *dataFormat = (outputFormat.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
    *sampleRate = (ALsizei)outputFormat.mSampleRate;
    
Exit:
    if (audioFile) {
        ExtAudioFileDispose(audioFile);
    }
    
    return data;
}