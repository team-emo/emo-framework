#import "EmoImage.h"
#import "EmoEngine_glue.h"

@implementation EmoImage
@synthesize filename;
@synthesize textureId;
@synthesize width, height, glWidth, glHeight;
@synthesize data;
@synthesize hasAlpha, loaded;
@synthesize referenceCount;

- (id)init {
    self = [super init];
    if (self != nil) {
		referenceCount = 0;
    }
    return self;
}

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
	textureId = 0;
}
-(void)dealloc {
	[filename release];
	[super dealloc];
}
@end
