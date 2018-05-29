//
//  MAVRender.h
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
//#import "ConfUIDef.h"
//#import "MAVOutInfoFetcherBase.h"
#import "MAVVideoDataDispatcher.h"
#import "MAVLayoutManager.h"
#import "MAVRenderUtils.h"
#import "ConfUIDef.h"

@protocol MAVRender <NSObject>

@required

//获取内部rootView
-(NSView*)view;

/**处理用户状态变化
  @param: bOpenOrClose:摄像头打开or关闭 （open：YES close：NO）
  @param: bMainOrSub:当前视频流是不是主路视频流，这个字段保留，暂时传YES
  @param: bActiveOrAppend:当控件已经有大图，是否去抢大图位置（抢：YES 不抢：NO）
 */
-(void)onUserVideoStateChange:(NSString *)uin :(bool)bOpenOrClose :(bool)bMainOrSub :(bool)bActiveOrAppend;

//分派视频数据
-(void)dispatchVideoData:(tagMAVVideoData*) data;

//隐藏或者显示小视频窗口
-(void)hideSmallVideoViews:(BOOL)show;

//获取用户视频窗口信息
-(NSArray*)getVideoInfo;

//响应切换到纯静态
-(void)setPureState:(BOOL)pure;


@end


@interface MAVRenderImpl : NSObject<MAVRender>
{
    //根视图
    NSView *rootView_;
    
    //分派视频数据对象
    MAVVideoDataDispatcher *dispatcherObject_;

    //辅助类对象
    MAVRenderUtils  *utils_;
}

@property(nonatomic,retain,readonly)MAVRenderUtils * utils;
//布局管理器
@property(nonatomic,retain)  MAVLayoutManager *layoutManager;

@end
