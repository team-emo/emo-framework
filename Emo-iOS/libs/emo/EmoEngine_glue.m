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
#import "EmoEngine.h"
#import "EmoEngine_glue.h"
#import "Constants.h"

#import "OpenGLES/ES1/gl.h"
#import "OpenGLES/ES1/glext.h"

extern EmoEngine* engine;

/*
 * Logging
 */
void LOGI(const char* msg) {
	if (engine.logLevel <= LOG_INFO) {
        if (engine.enableSimpleLogWithLevel) {
            NSLog(@"INFO %s", msg);
        } else if (engine.enableSimpleLog) {
            NSLog(@"%s", msg);
        } else {
            NSLog(@"%s INFO %s", EMO_LOG_TAG, msg);
        }
	}
}

void LOGE(const char* msg) {
	if (engine.logLevel <= LOG_ERROR) {
        if (engine.enableSimpleLogWithLevel) {
            NSLog(@"ERROR %s", msg);
        } else if (engine.enableSimpleLog) {
            NSLog(@"%s", msg);
        } else {
            NSLog(@"%s ERROR %s", EMO_LOG_TAG, msg);
        }
	}
}

void LOGW(const char* msg) {
	if (engine.logLevel <= LOG_WARN) {
        if (engine.enableSimpleLogWithLevel) {
            NSLog(@"WARN %s", msg);
        } else if (engine.enableSimpleLog) {
            NSLog(@"%s", msg);
        } else {
            NSLog(@"%s WARN %s", EMO_LOG_TAG, msg);
        }
	}
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

void* GetOpenALAudioData(CFURLRef fileURL, ALsizei* dataSize,
						 ALenum* dataFormat, ALsizei *sampleRate, bool *loaded) {
    OSStatus    err;
    UInt32      size;
    void*       data = NULL;
    
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

void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
               GLfloat centerx, GLfloat centery, GLfloat centerz,
               GLfloat upx, GLfloat upy, GLfloat upz) {
    GLfloat m[16];
    GLfloat x[3], y[3], z[3];
    GLfloat mag;
    
    /* Make rotation matrix */
    
    /* Z vector */
    z[0] = eyex - centerx;
    z[1] = eyey - centery;
    z[2] = eyez - centerz;
    mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
    if (mag) {          /* mpichler, 19950515 */
        z[0] /= mag;
        z[1] /= mag;
        z[2] /= mag;
    }
    
    /* Y vector */
    y[0] = upx;
    y[1] = upy;
    y[2] = upz;
    
    /* X vector = Y cross Z */
    x[0] = y[1] * z[2] - y[2] * z[1];
    x[1] = -y[0] * z[2] + y[2] * z[0];
    x[2] = y[0] * z[1] - y[1] * z[0];
    
    /* Recompute Y = Z cross X */
    y[0] = z[1] * x[2] - z[2] * x[1];
    y[1] = -z[0] * x[2] + z[2] * x[0];
    y[2] = z[0] * x[1] - z[1] * x[0];
    
    /* mpichler, 19950515 */
    /* cross product gives area of parallelogram, which is < 1.0 for
     * non-perpendicular unit-length vectors; so normalize x, y here
     */
    
    mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    if (mag) {
        x[0] /= mag;
        x[1] /= mag;
        x[2] /= mag;
    }
    
    mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
    if (mag) {
        y[0] /= mag;
        y[1] /= mag;
        y[2] /= mag;
    }
    
#define M(row,col)  m[col*4+row]
    M(0, 0) = x[0];
    M(0, 1) = x[1];
    M(0, 2) = x[2];
    M(0, 3) = 0.0;
    M(1, 0) = y[0];
    M(1, 1) = y[1];
    M(1, 2) = y[2];
    M(1, 3) = 0.0;
    M(2, 0) = z[0];
    M(2, 1) = z[1];
    M(2, 2) = z[2];
    M(2, 3) = 0.0;
    M(3, 0) = 0.0;
    M(3, 1) = 0.0;
    M(3, 2) = 0.0;
    M(3, 3) = 1.0;
#undef M
    glMultMatrixf(m);
    
    /* Translate Eye to Origin */
    glTranslatef(-eyex, -eyey, -eyez);
    
}

void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
    GLfloat f = 1.0f / tanf(fovy * (M_PI/360.0));
	GLfloat m [16];
    
	m[0] = f / aspect;
	m[1] = 0.0;
	m[2] = 0.0;
	m[3] = 0.0;
    
	m[4] = 0.0;
	m[5] = f;
	m[6] = 0.0;
	m[7] = 0.0;
    
	m[8] = 0.0;
	m[9] = 0.0;
	m[10] = (zFar + zNear) / (zNear - zFar);
	m[11] = -1.0;
    
	m[12] = 0.0;
	m[13] = 0.0;
	m[14] = 2.0 * zFar * zNear / (zNear - zFar);
	m[15] = 0.0;
    
	glMultMatrixf(m);
}