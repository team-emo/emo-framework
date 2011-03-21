#import "Emo_iOSAppDelegate.h"
#import "EmoViewController.h"
#import "EmoView.h"

@implementation Emo_iOSAppDelegate

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

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	[self.viewController onLostFocus];
    [self.viewController stopAnimation];
	[self.viewController onDispose];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	[self.viewController onLoad];
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
