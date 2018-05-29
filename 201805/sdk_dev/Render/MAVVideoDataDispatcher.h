//
//  MAVVideoDataDispatcher.h
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MAVBasicVideoViewController.h"
#import "MAVLayoutManager.h"

//分派视频数据到各个videoView视图
@interface MAVVideoDataDispatcher : NSObject
{
    MAVLayoutManager * layoutManager_;
}
//分派视频数据
-(void)dispatchVideoData:(unsigned char*)rgb24 width:(int)width height:(int)height angle:(int)angle uin:(NSString *)uin type:(int)type;

-(id)initWithLayoutManager:(MAVLayoutManager*)layoutManager;

@end
