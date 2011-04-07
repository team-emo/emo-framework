#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@interface EmoImage : NSObject {
    NSString* filename;
    GLuint   textureId;
    int      width;
    int      height;
	int      glWidth;
	int      glHeight;
    GLubyte* data;
    BOOL     hasAlpha;
}
@property (copy, readwrite) NSString* filename;
@property (readwrite) GLuint textureId;
@property (readwrite) int    width, height, glWidth, glHeight;
@property (readwrite) GLubyte* data;
@property (readwrite) BOOL hasAlpha;

-(BOOL)loadPng:(NSString*)file;
-(void)genTextures;
-(void)doUnload;
@end
