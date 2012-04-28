//
//  RobotLayer.h
//  DiscoTech Controller
//
//  Created by D. Grayson Smith on 7/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "cocos2d.h"
#import "SceneManager.h"

@class SneakyJoystick;
@class SneakyButton;

@interface RobotLayer : CCLayer
{
    SneakyJoystick *leftJoystick;
    SneakyJoystick *rightJoystick;
}

+(CCScene *) scene;

@end

