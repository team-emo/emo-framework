#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

@interface EmoImage : NSObject {
    NSString* filename;
    GLuint   textureId;
    int      width;
    int      height;
	int      glWidth;
	int      glHeight;
    GLubyte* data;
    BOOL     hasAlpha;
	BOOL     loaded;
	int      referenceCount;
}
@property (copy, readwrite) NSString* filename;
@property (readwrite) GLuint textureId;
@property (readwrite) int    width, height, glWidth, glHeight;
@property (assign, readwrite) GLubyte* data;
@property (readwrite) BOOL hasAlpha;
@property (readwrite) BOOL loaded;
@property (readwrite) int referenceCount;

-(BOOL)loadPng:(NSString*)file;
-(void)genTextures;
-(void)doUnload;
@end
