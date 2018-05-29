//
//  MAVLayoutManager.m
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import "MAVLayoutManager.h"
#import "MAVBasicVideoViewController.h"
#import "ConfUIDef.h"
#import "MAVRenderUtils.h"

@implementation MAVLayoutManager

@synthesize videoRootView= videoRootView_;
@synthesize cacheViewController = cacheViewController_;
@synthesize forbiddenVideoArray = forbiddenVideoArray_;

//视频最大路数配置
static const int MAX_VIDEO_COUNT = 5;

#pragma mark Initialization
-(id)initWithUtils:(MAVRenderUtils*)util;
{
    self = [super init];
    if(self)
    {
        videoViewControllers_ =  [[NSMutableArray alloc]initWithCapacity:5];
        utils_ = [util retain];
        forbiddenVideoArray_ = [[NSMutableArray alloc] init];
    }
    videoContentView_ =[[MAVBackView alloc]initWithFrame:NSMakeRect(0, 0, 4, 3) file:@"black.png"];
    videoContentView_.autoresizesSubviews = NO;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(notificationViewChanged:) name:NSViewFrameDidChangeNotification object:videoContentView_];
    return self;
}

-(void)dealloc
{
    
    [videoContentView_ release];
    videoContentView_ = nil;
    
    self.cacheViewController = nil;
    [self clearAllControllers];
    
    [utils_ release];
    utils_ = nil;
    
    [forbiddenVideoArray_ release];
    forbiddenVideoArray_ = nil;
    
    [[NSNotificationCenter defaultCenter]removeObserver:self];
    
    [super dealloc];
}

-(void)clearAllControllers
{
    for(MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        controller.layoutmanagerDelegate = nil;
    }
    [videoViewControllers_ release];
    videoViewControllers_ = nil;
    self.cacheViewController = nil;
}

#pragma mark Pure state related
//切换到纯静态的布局方式
-(void)switchToPureState:(BOOL)pure;
{
    pureState_ = pure;
    [self updateSmallViewByCurrentPureState];
    for(MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        [controller switchToPureStateNotify:pure];
    }
}

//更新当前布局-针对沉浸态
-(void)updateSmallViewByCurrentPureState
{
    [self setSmallVideoViewBottom];
}


#pragma mark Infomation of video views
//获取根视图信息
-(NSView*)getVideoContentView
{
    return videoContentView_;
}


#pragma mark Video nfo changed processing
-(NSArray*)getAllVideoViewInfo
{
    NSMutableArray *resultArray = [[[NSMutableArray alloc]init]autorelease];
   for(MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        {
            NSMutableDictionary * dic =  [[[NSMutableDictionary alloc]init]autorelease];
            NSSize frameSize = controller.view.frame.size;
            NSValue * sizeObj = [NSValue valueWithBytes:&frameSize objCType:@encode(NSSize)];
            NSString *uin = controller.uin;
            NSNumber *isMain = [[[NSNumber alloc]initWithInt:controller.type]autorelease];
            [dic setObject:uin forKey:@"uin" ];
            [dic setObject:isMain forKey:@"ismain"];
            [dic setObject:sizeObj forKey:@"size"];
            [resultArray addObject:dic];
        }
    }
    return [NSArray arrayWithArray:resultArray];
}

-(void)videoInfoChanged
{
    NSArray* arrVideoInfo = [self getAllVideoViewInfo];
    NSLog(@"videoInfoChanged %@ target action", arrVideoInfo);
    [utils_ raiseEvent:ConfUI_Evt_MainWin_VideoAIOInfoChange withObj:arrVideoInfo];
}


#pragma mark The content view of video
- (int)getVideoViewNumber
{
    return (int)[videoViewControllers_ count];
}

-(BOOL)hasSmallView
{
    for(MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        if(!controller.big)
        {
            return YES;
        }
    }
    return NO;
}

-(BOOL)hasBigView
{
    for(MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        if(controller.big)
        {
            return YES;
        }
    }
    return NO;
}


-(MAVBasicVideoViewController *)getBigViewController
{
    for(MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        if(controller.big)
        {
            return controller;
        }
    }
    return nil;
}



-(BOOL)isViewCreated:(NSString *)uin type:(int)type
{
    for(MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        NSLog(@"isViewCreated videoViewControllers count = %lu",(unsigned long)[videoViewControllers_ count]);
        if(([controller.uin isEqualToString: uin]) && (controller.type == type))
        {
            return YES;
        }
    }
    return NO;
}

-(MAVBasicVideoViewController*)getVideoView:(NSString *)uin type:(int)type
{
    for(MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        if(([controller.uin isEqualToString: uin]) && (controller.type == type))
        {
            return controller;
        }
    }
    return nil;
}

-(void)addVideoView:(NSString *)uin type:(int)type action:(BOOL)bActiveOrAppend
{
    if([self isViewCreated:uin type:type] || [forbiddenVideoArray_ containsObject:uin])
    {
        return ;
    }
    NSLog(@"MAVBasicVideoViewController start adding");
    MAVBasicVideoViewController * controller =  [[[MAVBasicVideoViewController alloc]initWithUin:uin type:type utils:utils_]autorelease];
    if ([self getVideoViewNumber] >= MAX_VIDEO_COUNT)
    {
        NSLog(@"RENDER:Have been reach the max video view number, the controller will be added to cacheViewController!");
        self.cacheViewController = controller;
        return ;
    }
    [self loadInController:controller];
    if (bActiveOrAppend && !controller.big)
    [self swapVideoControllersForDoubleVideoMode];
}

-(void)loadInController:(MAVBasicVideoViewController*)controller
{
    if (controller)
    {
        [videoViewControllers_ addObject:controller];
        [videoContentView_ addSubview:controller.view];
        if (![self hasBigView])//只有一个视图则设置为大画面视图
        {
            [controller setBig:YES];
        }
        [self updateLayout];
        controller.layoutmanagerDelegate = self;
    }
}

-(BOOL)makeSureBigViewExist
{
    if ([self hasBigView])
    {
        MAVBasicVideoViewController *big = [self getBigViewController];
        big.visible = YES;
        return YES;
    }
    else{
        for (MAVBasicVideoViewController *controller in videoViewControllers_)
        {
            if (!controller.big)
            {
                controller.visible = YES;
                [controller setBig:YES];
                return YES;
            }
        }
        
    }
    return NO;
}

-(void)removeVideoView:(NSString *)uin type:(int)type
{
    if(([cacheViewController_.uin isEqualToString: uin])&&(cacheViewController_.type == type))
    {
        self.cacheViewController = nil;
    }
    for(MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        if(([controller.uin isEqualToString:uin]) && (controller.type == type))
        {
            controller.layoutmanagerDelegate = nil;
            [controller getBasicVideoView].controllerDelegate = nil;
            [controller.view removeFromSuperview];
            
            [videoViewControllers_ removeObject:controller];
            [self makeSureBigViewExist];
            [self updateLayout];
            //如果有cached viewcontroller的话 load in
            [self loadInController:cacheViewController_];
            //在for循环里面遍历数组并删除元素，再次遍历会crash。防止再次遍历,加上break
            break;
        }
    }
}


#pragma mark Process Video View Change
-(void)setSmallVideoViewVisible:(BOOL)visible
{
    for (MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        // 小画面
        if (!controller.big)
        {
            controller.visible = visible;
        }
    }
}


-(void)requestForLargeControllerView:(MAVBasicVideoViewController*)smallViewController
{
    MAVBasicVideoViewController *bigController = nil;
    if(smallViewController.big)
    {
        return;
    }
    for(MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        if(controller.big)
        {
            bigController = controller;
            break;
        }
    }
    if(bigController.adaptiveStatus)
    {
        bigController.adaptiveStatus = NO;
    }
    [self switchVideoViewGeometryPosition:bigController withView:smallViewController];
}


//双人交换大小画面
-(bool)swapVideoControllersForDoubleVideoMode
{
    if([self getVideoViewNumber] != 2)
    {
        return NO;
    }
    
    MAVBasicVideoViewController *smallController = nil;
    for(MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        if(!controller.big)
        {
            smallController = controller;
            break;
        }
    }
    if(smallController!=nil)
    {
        [self requestForLargeControllerView:smallController];
        return YES;
    }
    return NO;
}


-(void)switchVideoViewGeometryPosition:(MAVBasicVideoViewController*)firstView withView:(MAVBasicVideoViewController*)secondView
{
    //只交换uin和type信息还是真的进行大小视图的变换（视图叠加问题）？ 这里需要研究下
    NSString * uin =  [NSString stringWithFormat: @"%@", firstView.uin];
    int type = firstView.type;
    firstView.uin = secondView.uin;
    firstView.type =  secondView.type;
    secondView.uin = uin;
    secondView.type = type;

    [self swapVideoFrameData:[firstView getVideoFrameInfo] withFrameData:[secondView getVideoFrameInfo]];
    
    [firstView updateLayout];
    [secondView updateLayout];
    
    [self videoInfoChanged];
}

-(void)swapVideoFrameData:(VideoFrameInfo *)firstInfo withFrameData:(VideoFrameInfo*)secondInfo
{
    VideoFrameInfo temp;
    memcpy(&temp, firstInfo, sizeof(VideoFrameInfo));
    memcpy(firstInfo, secondInfo, sizeof(VideoFrameInfo));
    memcpy(secondInfo, &temp, sizeof(VideoFrameInfo));
}

-(void)requestForHoveredControllerView:(MAVBasicVideoViewController*)viewController hovered:(BOOL)ishover
{
    if (ishover)
    {
        if (viewController.big)
        {
            BOOL flag = YES;
            for (MAVBasicVideoViewController *controller in videoViewControllers_)
            {
                if ((!controller.big) && (controller.hovered))
                {
                    viewController.hovered = NO;
                    flag = NO;
                    break;
                }
            }
            if (flag)
            {
                viewController.hovered = YES;
            }
        }
        else
        {
            viewController.hovered = YES;
            for (MAVBasicVideoViewController *controller in videoViewControllers_)
            {
                if(controller != viewController)
                {
                    controller.hovered = NO;
                }
            }
        }
    }
    else
    {
        viewController.hovered = NO;
        if (!viewController.big)
        {
            for (MAVBasicVideoViewController *controller in videoViewControllers_)
            {
                if (controller.big)
                {
                    controller.hovered = YES;
                }
            }
        }
    }
}



#pragma mark Layout all video views
//更新布局，对视频视图进行排布
-(void)updateLayout
{
    NSSize superFrameSize = videoContentView_.frame.size;
    //首先排布big视图
    for (MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        if(controller.big)
        {
            [controller.view setFrame:NSMakeRect(0, 0, superFrameSize.width, superFrameSize.height)];
            break;
        }
    }
    //如果有小视频窗口
    if ([self hasSmallView])
    {
        [self updateSmallViewByCurrentPureState];
    }
    [self videoInfoChanged];
}

//排布小视屏窗口
-(void)setSmallVideoViewBottom
{
    float smallViewSpace = 2;
    NSSize superFrameSize =  videoContentView_.frame.size;
    float ratioOfHeightWithWidth = superFrameSize.height/superFrameSize.width;
    float smallViewSizeHeight = (superFrameSize.height - smallViewSpace*(MAX_VIDEO_COUNT - 2))/(MAX_VIDEO_COUNT - 1);
    float smallViewSizeWidch = smallViewSizeHeight/ratioOfHeightWithWidth;
    NSSize smallViewSize = NSMakeSize(smallViewSizeWidch, smallViewSizeHeight);
    float beginX = 0;
    float beginY = superFrameSize.height - 2*smallViewSize.height;
    for(MAVBasicVideoViewController *controller in videoViewControllers_)
    {
        if(!controller.big)
        {
            [controller.view setFrame:NSMakeRect(beginX, beginY, smallViewSize.width, smallViewSize.height)];
            beginY = beginY - smallViewSize.height - smallViewSpace;
          if (beginY - smallViewSize.height <= 0) {
            beginY = superFrameSize.height - 2*smallViewSize.height;
            beginX = beginX + smallViewSize.width + smallViewSpace;
          }

        }
    }
}

- (void)notificationViewChanged:(NSNotification *)notification
{
    [self updateLayout];
}


@end
