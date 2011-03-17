//
//  Emo_iOSAppDelegate.h
//  Emo-iOS
//
//  Created by Kota Iguchi on 11/03/16.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class EmoViewController;

@interface Emo_iOSAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    EmoViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EmoViewController *viewController;

@end

