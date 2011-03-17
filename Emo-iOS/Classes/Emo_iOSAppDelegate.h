#import <UIKit/UIKit.h>

@class EmoViewController;

@interface Emo_iOSAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    EmoViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EmoViewController *viewController;

@end

