//
//  platform_comm.m
//  TXMBasic
//
//  Created by 曹少琨 on 2017/9/27.
//  Copyright © 2017年 Tencent. All rights reserved.
//

#include "platform_comm.h"

#import <Foundation/Foundation.h>

float publiccomponent_GetSystemVersion() {
    NSString *versionString;
    NSDictionary * sv = [NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"];
    if (nil != sv){
        versionString = [sv objectForKey:@"ProductVersion"];
        return versionString != nil ? versionString.floatValue : 0;
    }
    
    return 0;
}
