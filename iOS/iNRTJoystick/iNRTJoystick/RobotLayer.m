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
        leftJoy.position = ccp(110, size.height/2);
        leftJoy.backgroundSprite = [ColoredCircleSprite circleWithColor:ccc4(255, 0, 0, 128) radius:75];
        leftJoy.thumbSprite = [ColoredCircleSprite circleWithColor:ccc4(0, 0, 255, 200) radius:40];
        leftJoy.joystick = [[[SneakyJoystick alloc] initWithRect:CGRectMake(0, 0, 100, 100)] autorelease];
        leftJoystick = [leftJoy.joystick retain];
        leftJoystick.deadRadius = 10;
        leftJoystick.hasDeadzone = YES;
        [self addChild:leftJoy];
        [leftJoy release];
        [leftJoystick release];

        SneakyJoystickSkinnedBase *rightJoy = [[SneakyJoystickSkinnedBase alloc] init];
        rightJoy.position = ccp(size.width-110, size.height/2);
        rightJoy.backgroundSprite = [ColoredCircleSprite circleWithColor:ccc4(255, 0, 0, 128) radius:75];
        rightJoy.thumbSprite = [ColoredCircleSprite circleWithColor:ccc4(0, 0, 255, 200) radius:40];
        rightJoy.joystick = [[[SneakyJoystick alloc] initWithRect:CGRectMake(0, 0, 100, 100)] autorelease];
        rightJoystick = [rightJoy.joystick retain];
        rightJoystick.deadRadius = 10;
        rightJoystick.hasDeadzone = YES;
        [self addChild:rightJoy];
        [rightJoy release];
        [rightJoystick release];
        
		[self scheduleUpdate];
    }
    return self;
}

-(void) update: (ccTime) dt
{
    if (SUDP_IsOpen())
    {
        // write out the left.x, left.y, right.x, right.y positions
        char data[6];
        
        // the radius of the joysticks is 75
        
        data[0] = 's';
        data[1] = (((leftJoystick.stickPosition.x / 75.0) + 1.0)/2) * 255;
        data[2] = (((leftJoystick.stickPosition.y / 75.0) + 1.0)/2) * 255;
        data[3] = (((rightJoystick.stickPosition.x / 75.0) + 1.0)/2) * 255;
        data[4] = (((rightJoystick.stickPosition.y / 75.0) + 1.0)/2) * 255;
        data[5] = 'e';

        SUDP_SendMsg(data, 6);
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
