//
//  SceneManager.m
//  DiscoTech Controller
//
//  Created by D. Grayson Smith on 7/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "SceneManager.h"
#import "RobotLayer.h"

@interface SceneManager()
+(void)go:(CCLayer *)layer;
+(CCScene *)wrap:(CCLayer *)layer;
@end

@implementation SceneManager


+(void)goRobot
{
    CCLayer *layer = [RobotLayer node];
    [SceneManager go:layer];
}

+(void)go:(CCLayer *)layer
{
    CCDirector *director = [CCDirector sharedDirector];
    CCScene *newScene = [SceneManager wrap:layer];
    if ([director runningScene]) {
        [director replaceScene:newScene];
    }
    else
    {
        [director runWithScene:newScene];
    }
}

+(CCScene *)wrap:(CCLayer *)layer
{
    CCScene *newScene = [CCScene node];
    [newScene addChild:layer];
    return newScene;
}

@end

