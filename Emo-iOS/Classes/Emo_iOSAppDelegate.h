#import <UIKit/UIKit.h>

@class EmoViewController;

@interface Emo_iOSAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    EmoViewController *viewController;
}

@property (nonatomic, strong) IBOutlet UIWindow *window;
@property (nonatomic, strong) IBOutlet EmoViewController *viewController;

@end

