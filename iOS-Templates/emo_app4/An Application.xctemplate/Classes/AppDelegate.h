//
//  ___PROJECTNAMEASIDENTIFIER___AppDelegate.h
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//  Copyright ___ORGANIZATIONNAME___ ___YEAR___. All rights reserved.
//

#import <UIKit/UIKit.h>

@class EmoViewController;

@interface ___PROJECTNAMEASIDENTIFIER___AppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    EmoViewController *viewController;
}

@property (nonatomic, strong) IBOutlet UIWindow *window;
@property (nonatomic, strong) IBOutlet EmoViewController *viewController;

@end

