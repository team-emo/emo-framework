#import "EmoEngine_glue.h"
#import "Constants.h"
#import "png.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

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

/*
 * clear all OpenGL errors
 */
void clearGLErrors(const char* msg) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        if (error != GL_NO_ERROR) {
            LOGI(msg);
            char str[128];
            sprintf(str, "err code=0x%x", error);
            LOGI(str);
        }
    }
}

BOOL printGLErrors(const char* msg) {
    BOOL result = TRUE;
    for (GLint error = glGetError(); error; error = glGetError()) {
        if (error != GL_NO_ERROR) {
            LOGE(msg);
            char str[128];
            sprintf(str, "err code=0x%x", error);
            LOGE(str);
            result = FALSE;
        }
    }
    return result;
}

BOOL loadPngSizeFromAsset(NSString* filename, int *width, int *height) {
	NSString* path = [[NSBundle mainBundle] pathForResource:filename ofType:nil];
	if (path == nil) {
		LOGE("loadPngFromResource: resource does not found");
		NSLOGE(filename);
		return FALSE;
	}
	
	FILE* fp = fopen([path cStringUsingEncoding:1], "r");
	
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
	
    if (info_ptr == NULL) {
		fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return FALSE;
    }
	
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
        return FALSE;
    }
	
	png_init_io(png_ptr, fp);
	
    unsigned int sig_read = 0;
    png_set_sig_bytes(png_ptr, sig_read);
	
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, NULL);
	
    *width  = info_ptr->width;
    *height = info_ptr->height;
		
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
	
    return TRUE;
}

/* 
 * load png image from resource
 */
BOOL loadPngFromResource(NSString* filename, EmoImage* imageInfo) {
	NSString* path = [[NSBundle mainBundle] pathForResource:filename ofType:nil];
	if (path == nil) {
		LOGE("loadPngFromResource: resource does not found");
		NSLOGE(filename);
		return FALSE;
	}
	
	FILE* fp = fopen([path cStringUsingEncoding:1], "r");
	
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
	
    if (info_ptr == NULL) {
		fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return FALSE;
    }
	
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
        return FALSE;
    }

	png_init_io(png_ptr, fp);
	
    unsigned int sig_read = 0;
    png_set_sig_bytes(png_ptr, sig_read);
	
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, NULL);
	
    imageInfo.textureId = -1;
    imageInfo.filename = filename;
    imageInfo.width  = info_ptr->width;
    imageInfo.height = info_ptr->height;
	
    switch (info_ptr->color_type) {
        case PNG_COLOR_TYPE_RGBA:
            imageInfo.hasAlpha = true;
            break;
        case PNG_COLOR_TYPE_RGB:
            imageInfo.hasAlpha = false;
            break;
        default:
            LOGE("loadPngFromAsset: unsupported color type");
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			fclose(fp);
            return FALSE;
    }
    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    GLubyte* data = (unsigned char*) malloc(row_bytes * imageInfo.height);
	
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
	
    for (int i = 0; i < imageInfo.height; i++) {
        memcpy(data+(row_bytes * (imageInfo.height-1-i)), row_pointers[i], row_bytes);
    }
	
	imageInfo.data = data;
	
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
	
    return TRUE;
}

void* GetOpenALAudioData(CFURLRef fileURL, ALsizei* dataSize,
						 ALenum* dataFormat, ALsizei *sampleRate, bool *loaded) {
    OSStatus    err;
    UInt32      size;
    
	*loaded = false;
	
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
    
	*loaded = true;
Exit:
    if (audioFile) {
        ExtAudioFileDispose(audioFile);
    }
    
    return data;
}