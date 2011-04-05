#import "OpenAL/al.h"
#import "OpenAL/alc.h"
#import <AudioToolbox/AudioToolbox.h>
#import "EmoImage.h"

void LOGI(const char* msg);
void LOGE(const char* msg);
void LOGW(const char* msg);

void NSLOGI(NSString* msg);
void NSLOGE(NSString* msg);
void NSLOGW(NSString* msg);

BOOL loadPngFromResource(NSString* filename, EmoImage* imageInfo);
void* GetOpenALAudioData(CFURLRef fileURL, ALsizei* dataSize,
						 ALenum* dataFormat, ALsizei *sampleRate);
