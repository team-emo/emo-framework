//
//  Emo_iOSAppDelegate.m
//  Emo-iOS
//
//  Created by Kota Iguchi on 11/03/16.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "Emo_iOSAppDelegate.h"
#import "Emo_iOSViewController.h"

@implementation Emo_iOSAppDelegate

@synthesize window;
@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [self.window addSubview:self.viewController.view];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	[self.viewController onLostFocus];
    [self.viewController stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	[self.viewController onGainedFocus];
    [self.viewController startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [self.viewController stopAnimation];
}

- (void)dealloc
{
    [viewController release];
    [window release];
    
    [super dealloc];
}

@end
