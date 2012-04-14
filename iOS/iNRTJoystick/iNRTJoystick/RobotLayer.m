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
int joypadRadius = 100;
int joystickRadius = 30;

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
    // screen is 320x640
    NSLog(@"initializing joysticks");
    self = [super init];
    if (self) {        
        // ask director the the window size
		CGSize size = [[CCDirector sharedDirector] winSize];

        SneakyJoystickSkinnedBase *leftJoy = [[SneakyJoystickSkinnedBase alloc] init];
        leftJoy.position = ccp(joypadRadius+20, size.height/2);
        leftJoy.backgroundSprite = [ColoredCircleSprite circleWithColor:ccc4(255, 0, 0, 128) radius:joypadRadius];
        leftJoy.thumbSprite = [ColoredCircleSprite circleWithColor:ccc4(0, 0, 255, 200) radius:joystickRadius];
        leftJoy.joystick = [[[SneakyJoystick alloc] initWithRect:CGRectMake(0, 0, 100, 100)] autorelease];
        leftJoystick = [leftJoy.joystick retain];
        leftJoystick.deadRadius = 10;
        leftJoystick.hasDeadzone = YES;
        [self addChild:leftJoy];
        [leftJoy release];
        [leftJoystick release];

        SneakyJoystickSkinnedBase *rightJoy = [[SneakyJoystickSkinnedBase alloc] init];
        rightJoy.position = ccp(size.width-joypadRadius, size.height/2);
        rightJoy.backgroundSprite = [ColoredCircleSprite circleWithColor:ccc4(255, 0, 0, 128) radius:joypadRadius];
        rightJoy.thumbSprite = [ColoredCircleSprite circleWithColor:ccc4(0, 0, 255, 200) radius:joystickRadius];
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
        // position varies from -100 to 100
        float xlef = leftJoystick.stickPosition.x;
        float ylef = leftJoystick.stickPosition.y;
        float xrig = rightJoystick.stickPosition.x;
        float yrig = rightJoystick.stickPosition.y;
        
        NSDictionary *leftDict = [NSDictionary dictionaryWithObjectsAndKeys:
                                 [NSNumber numberWithInt:0], @"joystick",
                                 [NSNumber numberWithFloat:xlef], @"x",
                                 [NSNumber numberWithFloat:ylef], @"y",
                                 nil];

        NSDictionary *rightDict = [NSDictionary dictionaryWithObjectsAndKeys:
                                  [NSNumber numberWithInt:1], @"joystick",
                                  [NSNumber numberWithFloat:xrig], @"x",
                                  [NSNumber numberWithFloat	:yrig], @"y",
                                  nil];

        NSArray *array = [NSArray arrayWithObjects:leftDict, rightDict, nil];
        
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
