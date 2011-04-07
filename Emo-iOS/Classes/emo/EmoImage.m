#import "EmoImage.h"
#import "emo_engine_func.h"

@implementation EmoImage
@synthesize filename;
@synthesize textureId;
@synthesize width, height, glWidth, glHeight;
@synthesize data;
@synthesize hasAlpha;

-(BOOL)loadPng:(NSString*)file {
	return loadPngFromResource(file, self);
}
/*
 * assign OpenGL texture id
 */
-(void)genTextures {
	glGenTextures(1, &textureId);
}
-(void)doUnload {
	glDeleteTextures(1, &textureId);
	free(data);
}
@end
