#import <UIKit/UIKit.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

@protocol EmoViewEventHandler <NSObject>
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;
@end

@interface EmoView : UIView
{
@private
    EAGLContext *context;
    
    // The pixel dimensions of the CAEAGLLayer.
    GLint framebufferWidth;
    GLint framebufferHeight;
    
    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view.
    GLuint defaultFramebuffer, colorRenderbuffer;
	
	id<EmoViewEventHandler> eventDelegate;
}

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic,readwrite,assign) id<EmoViewEventHandler> eventDelegate;
@property (readonly) GLint width;
@property (readonly) GLint height;

- (void)setFramebuffer;
- (BOOL)presentFramebuffer;

@end
