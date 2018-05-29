//
//  MAVVideoDataDispatcher.m
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import "MAVVideoDataDispatcher.h"
#import "MAVLayoutManager.h"

@implementation MAVVideoDataDispatcher

-(id)initWithLayoutManager:(MAVLayoutManager*)layoutManager
{
    self = [super init];
    if(self)
    {
        layoutManager_ = [layoutManager retain];
    }
    return self;
}

-(void)dealloc
{
    [layoutManager_ release];
    layoutManager_= nil;
    [super dealloc];
}

//在主线程中分发数据
-(void)dispatchVideoData:(unsigned char*)rgb24 width:(int)width height:(int)height angle:(int)angle uin:(NSString *)uin type:(int)type
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    unsigned char *vbuf = rgb24;
    NSData *dispatch_data = [NSData dataWithBytes:vbuf length:width * height * 3];
    MAVBasicVideoViewController *controller = [layoutManager_ getVideoView:uin type:type];
    dispatch_async(dispatch_get_main_queue(), ^{
        if (controller)
            [controller output:(unsigned char*)dispatch_data.bytes width:width height:height angle:angle uin:uin type:type];
    });
    free(vbuf);
    [pool release];
}

@end
