//
//  Emo_iOSViewController.m
//  Emo-iOS
//
//  Created by Kota Iguchi on 11/03/16.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import "EmoViewController.h"
#import "EmoView.h"
#import "common.h"

@interface EmoViewController ()
- (void)updateTouchId;
- (void)fireMotionEvent:(NSSet *)touches withEvent:(UIEvent *) event withAction:(NSInteger) action;

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) CADisplayLink *displayLink;
@end

@implementation EmoViewController

@synthesize animating, context, displayLink;
@synthesize engine;

- (void)awakeFromNib
{
    EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!aContext)
    {
        aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    }
    
    if (!aContext)
        LOGE("Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        LOGE("Failed to set ES context current");
    
	self.context = aContext;
	[aContext release];
	
    [(EmoView *)self.view setContext:context];
    [(EmoView *)self.view setFramebuffer];
        
    animating = FALSE;
    animationFrameInterval = 1;
    self.displayLink = nil;
	
	// enable user interaction (touch event)
	[self.view setUserInteractionEnabled:TRUE];
	[self.view setMultipleTouchEnabled:TRUE];
	((EmoView *)self.view).eventDelegate = self;
    touchIdMaster = [[NSMutableDictionary alloc] init];
	nextTouchId = 0;
	
	self.engine = [[EmoEngine alloc]init];	
}

- (void)dealloc
{
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    
    [context release];
	[engine release];
	[touchIdMaster release];
    
    [super dealloc];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self startAnimation];
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopAnimation];
    [super viewWillDisappear:animated];
}

- (void)viewDidUnload
{
	[super viewDidUnload];

    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	self.context = nil;	
	self.engine = nil;
	
	[touchIdMaster removeAllObjects];
	touchIdMaster = nil;
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    /*
	 Frame interval defines how many display frames must pass between each time the display link fires.
	 The display link will only fire 30 times a second when the frame internal is two on a display that refreshes 60 times a second. The default frame interval setting of one will fire 60 times a second when the display refreshes at 60 times a second. A frame interval setting of less than one results in undefined behavior.
	 */
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;
        
        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if (!animating)
    {
        CADisplayLink *aDisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawFrame)];
        [aDisplayLink setFrameInterval:animationFrameInterval];
        [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        self.displayLink = aDisplayLink;
        
        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        [self.displayLink invalidate];
        self.displayLink = nil;
        animating = FALSE;
    }
}

- (void)drawFrame
{
    [(EmoView *)self.view setFramebuffer];

    [engine initDrawFrame];
	[engine onDrawFrame];
		
    [(EmoView *)self.view presentFramebuffer];
}

- (void)didReceiveMemoryWarning
{
	[engine onLowMemory];
    [super didReceiveMemoryWarning];
}

- (void)onLoad {
	if (![engine startEngine]) {
		NSLOGE(@"Failed to start engine");
	}
	
	if ([EmoEngine loadScriptFromResource:SQUIRREL_MAIN_SCRIPT vm:engine.sqvm] != EMO_NO_ERROR) {
		NSLOGE(@"Failed to load script");
	}
	
	/* call onLoad() */
	if (![engine onLoad]) {
		NSLOGE(@"failed to call onLoad");		
	}	
}
- (void)onGainedFocus {
	[engine onGainedFocus];
}

- (void)onLostFocus {
	[engine onLostFocus];
}

- (void)onDispose {
	[engine onDispose];
	[engine stopEngine];
}

/*
 * Handle touch events
 */
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *touch in touches) {
		if ([touchIdMaster objectForKey:touch] == nil) {
			[touchIdMaster setObject:[NSNumber numberWithInteger:nextTouchId] forKey:[NSValue valueWithPointer:touch]];
			nextTouchId++;
		}
	}
	[self fireMotionEvent: touches withEvent:event withAction: MOTION_EVENT_ACTION_DOWN];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	[self fireMotionEvent: touches withEvent:event withAction: MOTION_EVENT_ACTION_MOVE];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *touch in touches) {
		if ([touchIdMaster objectForKey:touch] != nil) {
			[touchIdMaster removeObjectForKey:touch];
		}
	}
	[self updateTouchId];
	[self fireMotionEvent: touches withEvent:event withAction: MOTION_EVENT_ACTION_UP];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	for (UITouch *touch in touches) {
		if ([touchIdMaster objectForKey:touch] != nil) {
			[touchIdMaster removeObjectForKey:touch];
		}
	}
	[self updateTouchId];
	[self fireMotionEvent: touches withEvent:event withAction: MOTION_EVENT_ACTION_CANCEL];
}

/*
 * Fire motion event
 */
- (void)fireMotionEvent:(NSSet *)touches withEvent:(UIEvent *)event withAction:(NSInteger) action {
	for (UITouch *touch in touches) {
		NSNumber *touchId = [touchIdMaster objectForKey:touch];
		CGPoint location = [touch locationInView:self.view];
		NSTimeInterval uptimeSec  = [engine uptime];
		NSTimeInterval uptimeMsec = (uptimeSec - floor(uptimeSec)) * 1000; 
		touchEventParamCache[0] = [touchId intValue];
		touchEventParamCache[1] = action;
		touchEventParamCache[2] = location.x;
		touchEventParamCache[3] = location.y;
		touchEventParamCache[4] = floor(uptimeSec);  // event time since startup (sec)
		touchEventParamCache[5] = floor(uptimeMsec); // event time since startup (msec)
		touchEventParamCache[6] = 0; // device id
		touchEventParamCache[7] = 0; // source id
		
		[engine onMotionEvent:touchEventParamCache];
	}
}
- (void)updateTouchId {
	if ([touchIdMaster count] == 0) {
		nextTouchId = 0;
	}
}
@end
