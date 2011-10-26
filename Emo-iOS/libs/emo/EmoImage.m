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
#import "EmoImage.h"
#import "EmoEngine_glue.h"

@implementation EmoImage
@synthesize filename;
@synthesize textureId;
@synthesize width, height, glWidth, glHeight;
@synthesize data;
@synthesize hasAlpha, loaded;
@synthesize referenceCount;
@synthesize freed;
@synthesize isPVR;

- (id)init {
    self = [super init];
    if (self != nil) {
		referenceCount = 0;
        freed = FALSE;
        data  = nil;
        isPVR = FALSE;
    }
    return self;
}

/*
 * assign OpenGL texture id
 */
-(void)genTextures {
	glGenTextures(1, &textureId);
}
-(void)doUnload {
	glDeleteTextures(1, &textureId);
    [self freeData];
	textureId = 0;
}
-(void)freeData {
    if (freed || data == nil) return;
    free(data);
    freed = TRUE;
    data = nil;
}
-(void)dealloc {
	[filename release];
	[super dealloc];
}
@end

/* 
 * load png image size from resource
 */
static BOOL loadPngSizeFromResource(NSString* filename, int *width, int *height) {
	NSString* path = [[NSBundle mainBundle] pathForResource:filename ofType:nil];
	if (path == nil) {
		LOGE("loadPngSizeFromResource: resource is not found");
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
	
    return width > 0 && height > 0;
}

/* 
 * load png image from resource
 */
static BOOL loadPngFromResource(NSString* filename, EmoImage* imageInfo) {
	NSString* path = [[NSBundle mainBundle] pathForResource:filename ofType:nil];
	if (path == nil) {
		LOGE("loadPngFromResource: resource is not found");
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

BOOL loadImageSizeFromResource(NSString* filename, int *width, int *height) {
    if ([filename hasSuffix:@".png"]) return loadPngSizeFromResource(filename, width, height);
    return FALSE;
}

BOOL loadImageFromResource(NSString* filename, EmoImage* imageInfo) {
    if ([filename hasSuffix:@".png"]) return loadPngFromResource(filename, imageInfo);
    return FALSE;
}