//
//  MainMenuViewController.h
//  DiscoTech Controller
//
//  Created by Sagar Pandya on 1/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RootViewController.h"

@interface MainMenuViewController : UIViewController
{
    RootViewController *_rootViewController;
}

@property (retain) RootViewController *rootViewController;
@property (retain, nonatomic) IBOutlet UITextField *hostnameField;

- (IBAction)connectTapped:(id)sender;

@end
