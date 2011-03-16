//
//  Emo_iOSViewController.h
//  Emo-iOS
//
//  Created by Kota Iguchi on 11/03/16.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <engine.h>

@interface Emo_iOSViewController : UIViewController
{
    EAGLContext *context;
    
    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *displayLink;
	Engine *engine;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;
@property (nonatomic, retain) Engine *engine;

- (void)onLoad;
- (void)startAnimation;
- (void)onGainedFocus;
- (void)onLostFocus;
- (void)stopAnimation;
- (void)onDispose;

@end
