#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

@interface EmoStage : NSObject {
	GLuint vbo[2];

	BOOL loaded;
	BOOL dirty;
	
	short indices[4];
	float positions[12];
	
	GLint width;
	GLint height;
	GLint viewport_width;
	GLint viewport_height;
}
@property (readwrite) GLint width;
@property (readwrite) GLint height;
@property (readwrite) BOOL dirty;
@property (readwrite) GLint viewport_width;
@property (readwrite) GLint viewport_height;

-(GLuint)getPositionPointer;
-(GLuint)getIndicePointer;

-(void)setSize:(GLint)_width height:(GLint)_height;
-(BOOL)loadBuffer;
-(BOOL)onDrawFrame:(NSTimeInterval)dt;
-(void)unloadBuffer;
@end
