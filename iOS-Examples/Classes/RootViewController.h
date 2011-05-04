#import <UIKit/UIKit.h>
#import "EmoViewController.h"

@interface RootViewController : UITableViewController {
	EmoViewController* viewController;
	BOOL didReturnRootView;
	NSMutableArray* examples;
	NSArray* sections;
}

@end
