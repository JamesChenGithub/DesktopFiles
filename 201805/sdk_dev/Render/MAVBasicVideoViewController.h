//
//  MAVBasicVideoViewController.h
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
//#import "MAVOutInfoFetcherBase.h"
#import "MAVBasicVideoView.h"
#import "MAVBackView.h"
#import "MAVRenderUtils.h"
//#import "AVGradientView.h"

#define BigViewFrameRatio_Changed @"_BigViewFrameRatio_Changed_"

@class MAVBasicVideoViewController;

@protocol MAVLayoutManagerDelegate <NSObject>
//大小画面切换
-(void)requestForLargeControllerView:(MAVBasicVideoViewController*)smallViewController;
//请求是否可以被hover(重叠的情况)
-(void)requestForHoveredControllerView:(MAVBasicVideoViewController*)viewController hovered:(BOOL)ishover;

@end

//在切换画面的时候利用上一帧的数据重新刷新界面
typedef struct
{
    int width;
    int height;
    int angle;
    uint64 bufSize;
    unsigned char *rgb24;
}VideoFrameInfo;

@class MAVEventBackView;

@interface MAVBasicVideoViewController : NSViewController<MAVBasicVideoViewControllerDelegate>
{    
    //视图
    MAVBasicVideoView *videoView_;
    BOOL visible_;
    BOOL hovered_;
    BOOL big_;
    
    //标题
    NSView *titlePanelView_;
    
    //uin 在opensdk里面要用字符串，因为账号可能是字母
    NSString * uin_;
    
    //video type:主路视频或者其它
    int type_;
    
    //根视图 self.view
    NSView  *rootView_;
    
    //背景图
    MAVBackView *backView_;
    NSTextField *tipsView_;
    
    id<MAVLayoutManagerDelegate> layoutmanagerDelegate_;
    
    //辅助类对象
    MAVRenderUtils  *utils_;
    
    //昵称
    NSTextField  *nameField_;
    
    //截取video数据存为图片的状态
    BOOL captureVideoData_;
    
    //1:1自适应状态
    BOOL adaptiveStatus_;
    
    
    //当前video数据的原始size
    NSSize videoSize_;

    
    //1：1排布的控制按钮
    MAVEventBackView *adaptiveControlView_;
    NSView *oneToOneContentView_;
    NSView *adaptiveContentView_;
    NSImageView *backImageView_;
    
    VideoFrameInfo videoFrameInfo_;
    NSTextField* businessInfo_;
    
    NSMutableArray<NSLayoutConstraint *> *constraint_;
    
    //记录大图宽高比
    CGFloat  bigViewRatio_;
    
}

@property (copy,nonatomic) NSString * uin;
@property (assign,nonatomic) int type;
@property (assign,nonatomic) BOOL visible;
@property (assign,nonatomic) BOOL hovered;
@property (assign,nonatomic) BOOL big;
@property (nonatomic,assign) id<MAVLayoutManagerDelegate> layoutmanagerDelegate;
@property (nonatomic,retain) MAVRenderUtils  *utils;
@property (assign,nonatomic) BOOL captureVideoData;
@property (assign,nonatomic) BOOL adaptiveStatus;
@property (assign,nonatomic) BOOL dataReady;
@property (assign,nonatomic) CGFloat  bigViewRatio;
//初始化
-(id)initWithUin:(NSString *)uin type:(int)type utils:(MAVRenderUtils*)utils;

//接收视频数据
-(void)output:(unsigned char*)rgb24 width:(int)width height:(int)height angle:(int)angle uin:(NSString *)uin type:(int)type;

//是否需要1:1自适应
//-(void)setSelfAdaptive:(BOOL)flag;

//切换到纯静态通知接口
-(void)switchToPureStateNotify:(BOOL)pure;

-(MAVBasicVideoView*)getBasicVideoView;

-(void)updateLayout;


-(VideoFrameInfo*)getVideoFrameInfo;

@end



@interface MAVEventBackView : NSView
{
     NSTrackingArea * trackingArea_;
     MAVBasicVideoViewController *controller_;
}
-(id)initWithFrame:(NSRect)frameRect controller:(MAVBasicVideoViewController*)controller;
@end






