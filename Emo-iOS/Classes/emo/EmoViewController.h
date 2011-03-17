#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <sqengine.h>

@interface EmoViewController : UIViewController
{
    EAGLContext *context;
    
    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *displayLink;
	EmoEngine *engine;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;
@property (nonatomic, retain) EmoEngine *engine;

- (void)onLoad;
- (void)startAnimation;
- (void)onGainedFocus;
- (void)onLostFocus;
- (void)stopAnimation;
- (void)onDispose;

@end
