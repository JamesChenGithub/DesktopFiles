//
//  MAVLayoutManager.h
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MAVBasicVideoViewController.h"
#import "MAVRenderUtils.h"

//视频视图排布，控制视频视图的显示和位置等信息
@interface MAVLayoutManager : NSObject<MAVLayoutManagerDelegate>
{
    
    //标识当前是否位于纯静态
    BOOL pureState_;
    
    //视图容器
    NSMutableArray<MAVBasicVideoViewController *>* videoViewControllers_;
    
    //根视图
    NSView  *videoContentView_;
    
    MAVRenderUtils *utils_;
    
    //测试用
    NSView *videoRootView_;
    
    //变态的方案  本地屏幕分享冲突的问题
    MAVBasicVideoViewController *cacheViewController_;
    
    //屏幕的图像容器
    NSMutableArray *forbiddenVideoArray_;
    
}

-(id)initWithUtils:(MAVRenderUtils*)util;

//添加一个视频视图
-(void)addVideoView:(NSString *)uin type:(int)type action:(BOOL)bActiveOrAppend;

//去除一个视频视图
-(void)removeVideoView:(NSString *)uin type:(int)type;

//判断一个视图是否创建
-(BOOL)isViewCreated:(NSString *)uin type:(int)type;

//获取一个视频视图
-(MAVBasicVideoViewController*)getVideoView:(NSString *)uin type:(int)type;

//获取用户视频窗口信息
-(NSArray*)getAllVideoViewInfo;

//获取大图视频窗口信息
-(MAVBasicVideoViewController *)getBigViewController;

//切换到纯静态的布局方式
-(void)switchToPureState:(BOOL)pure;

//将所有的小视频信息隐藏或者显示
-(void)setSmallVideoViewVisible:(BOOL)visible;

//获取视频视图的根视图
-(NSView*)getVideoContentView;


//双人视频的时候远端界面优先显示大画面的需求
-(bool)swapVideoControllersForDoubleVideoMode;

@property(nonatomic,retain) NSView *videoRootView;

@property(nonatomic,retain)MAVBasicVideoViewController *cacheViewController;
@property(nonatomic,retain)NSMutableArray *forbiddenVideoArray;
@end



