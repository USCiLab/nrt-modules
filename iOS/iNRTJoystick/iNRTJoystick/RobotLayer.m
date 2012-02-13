//
//  RobotLayer.m
//  DiscoTech Controller
//
//  Created by D. Grayson Smith on 7/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "RobotLayer.h"

#import "SneakyButton.h"
#import "SneakyJoystick.h"
#import "SneakyButtonSkinnedBase.h"
#import "SneakyJoystickSkinnedBase.h"
#import "ColoredCircleSprite.h"
#import "ColoredSquareSprite.h"

#import "SendUDP.h"

@implementation RobotLayer
int count = 0;

+(CCScene *) scene
{
	// 'scene' is an autorelease object.
	CCScene *scene = [CCScene node];
	
	// 'layer' is an autorelease object.
	RobotLayer *layer = [RobotLayer node];
	
	// add layer as a child to scene
	[scene addChild: layer];
	
	// return the scene
	return scene;
}

- (id)init	
{
    NSLog(@"initializing joysticks");
    self = [super init];
    if (self) {        
        // ask director the the window size
		CGSize size = [[CCDirector sharedDirector] winSize];

        SneakyJoystickSkinnedBase *leftJoy = [[SneakyJoystickSkinnedBase alloc] init];
        leftJoy.position = ccp(150, size.height/2);
        leftJoy.backgroundSprite = [ColoredCircleSprite circleWithColor:ccc4(255, 0, 0, 128) radius:145];
        leftJoy.thumbSprite = [ColoredCircleSprite circleWithColor:ccc4(0, 0, 255, 200) radius:40];
        leftJoy.joystick = [[[SneakyJoystick alloc] initWithRect:CGRectMake(0, 0, 100, 100)] autorelease];
        leftJoystick = [leftJoy.joystick retain];
        leftJoystick.deadRadius = 10;
        leftJoystick.hasDeadzone = YES;
        [self addChild:leftJoy];
        [leftJoy release];
        [leftJoystick release];
        
		[self scheduleUpdate];
        
        /* request the webview url */
        NSArray *array = [NSArray arrayWithObject:@"refresh"];
        NSError *error = nil;
        id data = [NSJSONSerialization dataWithJSONObject:array options:kNilOptions error:&error];

        NSLog(@"Doing request: %s\n", [data bytes]);
        SUDP_SendMsg([data bytes], [data length]);
        
        NSLog(@"Waiting for reply...");
        char buf[512];
        SUDP_RecvMsg(buf, 512);
        NSLog(@"Got reply: %s\n", buf);
    }
    return self;
}

-(void) update: (ccTime) dt
{
    if (SUDP_IsOpen())
    {
        int x = (((leftJoystick.stickPosition.x / 145.0) + 1.0)/2) * 255;
        int y = (((leftJoystick.stickPosition.y / 145.0) + 1.0)/2) * 255;
        
        NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
                              [NSNumber numberWithInt:0], @"joystick",
                              [NSNumber numberWithInt:x], @"x",
                              [NSNumber numberWithInt:y], @"y",
                              nil];

        NSArray *array = [NSArray arrayWithObject:dict];
        
        NSError *error = nil;
        id data = [NSJSONSerialization dataWithJSONObject:array options:kNilOptions error:&error];
        
        SUDP_SendMsg([data bytes], [data length]);
    }
}

// on "dealloc" you need to release all your retained objects
- (void) dealloc
{
	// in case you have something to dealloc, do it in this method
	// in this particular example nothing needs to be released.
	// cocos2d will automatically release all the children (Label)
	
	// don't forget to call "super dealloc"
	[super dealloc];
}

@end
