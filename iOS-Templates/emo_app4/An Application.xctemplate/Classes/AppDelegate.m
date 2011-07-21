//
//  ___PROJECTNAMEASIDENTIFIER___AppDelegate.m
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//  Copyright ___ORGANIZATIONNAME___ ___YEAR___. All rights reserved.
//

#import "AppDelegate.h"
#import "EmoViewController.h"
#import "EmoView.h"

@implementation ___PROJECTNAMEASIDENTIFIER___AppDelegate

@synthesize window;
@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	
    viewController = [[EmoViewController alloc] init];
	viewController.view = [[EmoView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	[viewController awakeFromNib];
	
    [self.window addSubview:self.viewController.view];
	
	[window makeKeyAndVisible];

	[self.viewController onLoad];
	
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
	[self.viewController onLostFocus];
    [self.viewController stopAnimation];
	[self.viewController onDispose];
}

- (void)dealloc
{
    [viewController release];
    [window release];
    [super dealloc];
}

@end
