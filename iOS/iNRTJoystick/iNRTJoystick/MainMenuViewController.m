//
//  MainMenuViewController.m
//  DiscoTech Controller
//
//  Created by Sagar Pandya on 1/16/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "MainMenuViewController.h"
#import "SendUDP.h"

@implementation MainMenuViewController
@synthesize hostnameField = _hostnameField;

@synthesize rootViewController = _rootViewController;

- (IBAction)connectTapped:(id)sender
{
    SUDP_Close();
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:[self hostnameField].text forKey:@"hostname"];
    [defaults synchronize];
    SUDP_Init([[self hostnameField].text cStringUsingEncoding:NSASCIIStringEncoding]);
    
    if (_rootViewController == nil)
    {
        self.rootViewController = [[[RootViewController alloc] initWithNibName:nil bundle:nil] autorelease];
    }
    [self.navigationController pushViewController:_rootViewController animated:YES];
}

- (void)dealloc
{
    [_rootViewController release];
    _rootViewController = nil;
    [_hostnameField release];
    [super dealloc];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    if ( [defaults stringForKey:@"hostname"] != nil)
    {
        [_hostnameField setText:[defaults stringForKey:@"hostname"]];
    }
}

- (void)viewDidUnload
{
    [self setHostnameField:nil];
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    //return (interfaceOrientation == UIInterfaceOrientationPortrait);
    return UIInterfaceOrientationIsLandscape(interfaceOrientation);
}

- (void) viewWillAppear:(BOOL)animated
{
    [self.navigationController setNavigationBarHidden:YES animated:animated];
    [super viewWillAppear:animated];
}

- (BOOL)textFieldShouldReturn:(UITextField*)textField
{
    [textField resignFirstResponder];
    return YES;
}

@end
