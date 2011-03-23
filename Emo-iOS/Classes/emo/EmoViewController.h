#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <common.h>
#import <EmoEngine.h>
#import <EmoView.h>

@interface EmoViewController : UIViewController <EmoViewEventHandler>
{
    EAGLContext *context;
    
    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *displayLink;
	EmoEngine *engine;
	
	NSMutableDictionary *touchIdMaster;
	float touchEventParamCache[MOTION_EVENT_PARAMS_SIZE];
	NSInteger nextTouchId;
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
