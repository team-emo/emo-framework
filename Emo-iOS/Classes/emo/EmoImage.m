#import "EmoImage.h"
#import "emo_engine_func.h"

@implementation EmoImage
@synthesize filename;
@synthesize textureId;
@synthesize width;
@synthesize height;
@synthesize data;
@synthesize hasAlpha;
-(BOOL)loadPng:(NSString*)file {
	return loadPngFromResource(file, self);
}
@end
