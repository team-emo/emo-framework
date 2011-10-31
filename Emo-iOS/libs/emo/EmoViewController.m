// Copyright (c) 2011 emo-framework project
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the project nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
#import "QuartzCore/QuartzCore.h"

#import "EmoViewController.h"
#import "EmoView.h"
#import "Constants.h"
#import "EmoEngine_glue.h"

EmoEngine* engine;

@interface EmoViewController ()
- (void)updateTouchId;
- (void)fireMotionEvent:(NSSet *)touches withEvent:(UIEvent *) event withAction:(NSInteger) action;

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) CADisplayLink *displayLink;
@end

@implementation EmoViewController

@synthesize animating, context, displayLink;
@synthesize runtimeScript, mainScript;

- (void)awakeFromNib
{
    EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    
    if (!aContext)
        LOGE("Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        LOGE("Failed to set ES context current");
    
	self.context = aContext;
	[aContext release];
	
	EmoView* eview = (EmoView *)self.view;
	
	[eview enableRetina:TRUE];
    [eview setContext:context];
    [eview setFramebuffer];
        
    animating = FALSE;
    animationFrameInterval = 1;
    self.displayLink = nil;
	
	// enable user interaction (touch event)
	[self.view setUserInteractionEnabled:TRUE];
	[self.view setMultipleTouchEnabled:TRUE];
	eview.eventDelegate = self;
    touchIdMaster = [[NSMutableDictionary alloc] init];
	nextTouchId = 0;
	
	engine = [[EmoEngine alloc]init];
}

- (void)dealloc
{
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    
	[runtimeScript release];
	[mainScript release];
	
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
	engine = nil;
	
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
	[engine onDrawFrame:((EmoView*)self.view).framebuffer];
		
    [(EmoView *)self.view presentFramebuffer];
}

- (void)didReceiveMemoryWarning
{
	[engine onLowMemory];
    [super didReceiveMemoryWarning];
}

- (void)onLoad {
	
	if (runtimeScript == nil) {
		runtimeScript = @SQUIRREL_RUNTIME_SCRIPT;
	}
	if (mainScript == nil) {
		mainScript = @SQUIRREL_MAIN_SCRIPT;
	}
	
	if (![engine startEngine:((EmoView *)self.view).width withHeight:((EmoView *)self.view).height]) {
		NSLOGE(@"Failed to start engine");
	}
	
	if ([engine loadScriptFromResource:[runtimeScript UTF8String] vm:engine.sqvm] != EMO_NO_ERROR) {
		NSLOGE(@"Failed to load runtime script");
	}

	if ([engine loadScriptFromResource:[mainScript UTF8String] vm:engine.sqvm] != EMO_NO_ERROR) {
		NSLOGE(@"Failed to load main script");
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
	[self fireMotionEvent: touches withEvent:event withAction: MOTION_EVENT_ACTION_UP];
	for (UITouch *touch in touches) {
		if ([touchIdMaster objectForKey:[NSValue valueWithPointer:touch]] != nil) {
			[touchIdMaster removeObjectForKey:[NSValue valueWithPointer:touch]];
		}
	}
	[self updateTouchId];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
	[self fireMotionEvent: touches withEvent:event withAction: MOTION_EVENT_ACTION_CANCEL];
	for (UITouch *touch in touches) {
		if ([touchIdMaster objectForKey:[NSValue valueWithPointer:touch]] != nil) {
			[touchIdMaster removeObjectForKey:[NSValue valueWithPointer:touch]];
		}
	}
	[self updateTouchId];
}

/*
 * Fire motion event
 */
- (void)fireMotionEvent:(NSSet *)touches withEvent:(UIEvent *)event withAction:(NSInteger) action {
	for (UITouch *touch in touches) {
		NSNumber *touchId = [touchIdMaster objectForKey:[NSValue valueWithPointer:touch]];
		CGPoint location = [touch locationInView:self.view];
		
		if (((EmoView*)self.view).isRetina) {
			location.x = location.x * RETINA_SCALE_FACTOR;
			location.y = location.y * RETINA_SCALE_FACTOR;
		}
		
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
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	if (engine != nil) {
		return [engine shouldAutorotateToInterfaceOrientation:interfaceOrientation];
	}
	return [super shouldAutorotateToInterfaceOrientation:interfaceOrientation];
}
-(NSInteger)getOrientationOption {
    if (engine == nil) {
        return OPT_ORIENTATION_UNSPECIFIED;
    }
    return engine.currentOrientation;
}
@end
