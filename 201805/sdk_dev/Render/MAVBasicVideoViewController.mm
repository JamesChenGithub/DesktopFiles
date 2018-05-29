//
//  MAVBasicVideoViewController.m
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import "MAVBasicVideoViewController.h"
#import "MAVBackView.h"


@implementation MAVBasicVideoViewController

@synthesize uin = uin_;
@synthesize type = type_;
@synthesize visible = visible_;
@synthesize hovered = hovered_;
@synthesize big = big_;
@synthesize layoutmanagerDelegate = layoutmanagerDelegate_;
@synthesize utils = utils_;
@synthesize captureVideoData = captureVideoData_;
@synthesize adaptiveStatus = adaptiveStatus_;
@synthesize bigViewRatio = bigViewRatio_;
//初始化
-(id)initWithUin:(NSString *)uin type:(int)type utils:(MAVRenderUtils*)utils
{
    self =  [super init];
    if(self)
    {
//        uin_ = uin;
//        type_ = type;
        self.uin = uin;
        self.type = type;
        self.utils  =  utils;
        constraint_ = [NSMutableArray new];
        self.dataReady = NO;
    }
    return self;
}


-(void)dealloc
{
    self.uin = nil;
    self.layoutmanagerDelegate = nil;
    self.utils = nil;
    //辅路视频都有一个Buffer
    if(videoFrameInfo_.rgb24 != NULL)
    {
        free(videoFrameInfo_.rgb24);
        videoFrameInfo_.rgb24 = NULL;
        videoFrameInfo_.bufSize = 0;
    }
    if (constraint_)
    {
        [constraint_ release];
        constraint_ = nil;
    }

    [super dealloc];
}

-(MAVBasicVideoView*)getBasicVideoView
{
    return videoView_;
}

-(VideoFrameInfo*)getVideoFrameInfo
{
    return &videoFrameInfo_;
}

-(void)loadView
{
    rootView_ = [[[NSView alloc]initWithFrame:NSMakeRect(0, 0, 4, 3)]autorelease];
    rootView_.autoresizesSubviews = YES;
    rootView_.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    self.view = rootView_;

    NSRect contentViewRect = [self.view bounds];
    //背景视图,根据父视图缩放
    backView_ = [[[MAVBackView alloc]initWithFrame:contentViewRect file:@"black.png"]autorelease];
    backView_.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [rootView_ addSubview:backView_];
    [backView_ setHidden:NO];

    //视频视图
    videoView_ = [[[MAVBasicVideoView alloc]initWithFrame:contentViewRect]autorelease];
    videoView_.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [rootView_ addSubview:videoView_];
    [videoView_ setHidden:NO];
    [self setVideoViewConstraints];
    
//    if (![utils_ isBuddy])
        [self addBottomTitleView:videoView_];
    videoView_.controllerDelegate = self;
    //控制自适应1:1图标(屏幕分享)
    adaptiveControlView_ =   [[[MAVEventBackView alloc]initWithFrame:NSMakeRect(0, 0, 70, 27) controller:self]autorelease];
    adaptiveControlView_.translatesAutoresizingMaskIntoConstraints = NO;
    [rootView_ addSubview:adaptiveControlView_];
    
    // “正在屏幕分享”
    businessInfo_ = [[[NSTextField alloc] init] autorelease];
    [[businessInfo_ cell] setLineBreakMode:NSLineBreakByCharWrapping];
    [[businessInfo_ cell] setTruncatesLastVisibleLine:YES];
    [businessInfo_ setTranslatesAutoresizingMaskIntoConstraints:NO];
    [businessInfo_ setBordered:NO];
    [businessInfo_ setEditable:NO];
    [businessInfo_ setSelectable:NO];
    [businessInfo_ setTextColor:[NSColor colorWithRed:0.07f green:0.71f blue:0.95f alpha:1.0f]];
    [businessInfo_ setBackgroundColor:[NSColor clearColor]];
    [businessInfo_ setStringValue:@"正在屏幕分享..."];
    NSFont *fieldFont = [NSFont fontWithName:@"Arial" size:14.0f];
    businessInfo_.font = fieldFont;
    [businessInfo_ sizeToFit];
    [rootView_ addSubview:businessInfo_];
    
    NSDictionary* views = @{
                            @"adaptiveControlView_" : adaptiveControlView_,
                            @"superview" : self.view
                            };
    
    [self.view
     addConstraints:[NSLayoutConstraint
                     constraintsWithVisualFormat:@"H:[adaptiveControlView_(==70)]-5-|"
                     options:0
                     metrics:nil
                     views:views]];
    
    [self.view
     addConstraints:[NSLayoutConstraint
                     constraintsWithVisualFormat:@"V:[adaptiveControlView_(==27)]-2-|"
                     options:0
                     metrics:nil
                     views:views]];
    
    [self initAdaptiveControllerView];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(didEnterFull:)
                                                 name:NSWindowDidEnterFullScreenNotification
                                               object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(didExitFull:)
                                                 name:NSWindowDidExitFullScreenNotification
                                               object:nil];
}

-(void)setVideoViewConstraints
{
    NSDictionary* views = @{
                            @"videoView" : videoView_,
                            @"rootView" : rootView_
                            };
    
    videoView_.translatesAutoresizingMaskIntoConstraints = NO;
    [rootView_ removeConstraints:constraint_];
    [constraint_ removeAllObjects];
    //辅路没必要缩小 rogerlin
    /*
    if (big_ && type_ == 0)
    {
        CGSize maxSize = CGSizeMake(290, 170);
        CGSize frameSize = maxSize;
        
        [constraint_ addObject:[NSLayoutConstraint constraintWithItem:videoView_ attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1.0 constant:frameSize.width]];
        [constraint_ addObject:[NSLayoutConstraint constraintWithItem:videoView_ attribute:NSLayoutAttributeHeight relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1.0 constant:frameSize.height]];
        
        [constraint_ addObject:[NSLayoutConstraint constraintWithItem:videoView_ attribute:NSLayoutAttributeCenterX relatedBy:NSLayoutRelationEqual toItem:rootView_ attribute:NSLayoutAttributeCenterX multiplier:1.0 constant:0]];
        [constraint_ addObject:[NSLayoutConstraint constraintWithItem:videoView_ attribute:NSLayoutAttributeTop relatedBy:NSLayoutRelationEqual toItem:rootView_ attribute:NSLayoutAttributeTop multiplier:1.0 constant:110]];

        [constraint_ addObject:[NSLayoutConstraint constraintWithItem:businessInfo_ attribute:NSLayoutAttributeCenterX relatedBy:NSLayoutRelationEqual toItem:rootView_ attribute:NSLayoutAttributeCenterX multiplier:1.0 constant:0]];
        [constraint_ addObject:[NSLayoutConstraint constraintWithItem:businessInfo_ attribute:NSLayoutAttributeTop relatedBy:NSLayoutRelationEqual toItem:rootView_ attribute:NSLayoutAttributeTop multiplier:1.0 constant:290]];
    }
    else
    {
     */
        [constraint_ addObjectsFromArray:[NSLayoutConstraint
                                          constraintsWithVisualFormat:@"V:|-0-[videoView]-0-|"
                                          options:0
                                          metrics:nil
                                          views:views]];
        
        [constraint_ addObjectsFromArray:[NSLayoutConstraint
                                          constraintsWithVisualFormat:@"H:|-0-[videoView]-0-|"
                                          options:0
                                          metrics:nil
                                          views:views]];
//    }
    [rootView_ addConstraints:constraint_];
}

-(void)updateVideoViewConstraints
{
    [rootView_ setNeedsUpdateConstraints:YES];
}

-(void)removeVideoViewConstraints
{
    [rootView_ removeConstraints:constraint_];
    [constraint_ removeAllObjects];
    [self updateVideoViewConstraints];
}



-(void)initAdaptiveControllerView
{
    NSImage * backImage = [NSImage imageNamed:@"mav_adaptive_background.png"];
    NSRect contentRect = NSMakeRect(0, 0, 70, 27);
    backImageView_ =  [[[NSImageView alloc] initWithFrame:contentRect]autorelease];
    backImageView_.bounds = contentRect;
    backImageView_.image  = backImage;
    [backImageView_ setImageScaling:NSImageScaleProportionallyDown];
    adaptiveControlView_.autoresizesSubviews = YES;
    [adaptiveControlView_ addSubview:backImageView_];
    //
    {
        oneToOneContentView_ = [[NSView alloc]initWithFrame:contentRect];
        oneToOneContentView_.autoresizingMask = NSViewNotSizable;
        oneToOneContentView_.autoresizesSubviews = NO;
        NSImage * normalImage = [NSImage imageNamed:@"mav_adaptive_normal.png"];
        NSRect normalImageRect = NSMakeRect(10.0,8.0,13.0,10.0);
        NSImageView *normalImageView =  [[[NSImageView alloc] initWithFrame:normalImageRect]autorelease];
        normalImageView.bounds = normalImageRect;
        normalImageView.image  = normalImage;
        [normalImageView setImageScaling:NSImageScaleProportionallyDown];
        
        NSString *name  = @"1:1";
        NSFont *fieldFont = [NSFont fontWithName:@"Arial" size:14.0f];
        NSTextField* nameField = [[[NSTextField alloc] initWithFrame:NSMakeRect(25.0, 4.0, 40, 20.0)]autorelease];
        nameField.bezeled = NO;
        nameField.editable = NO;
        [nameField setWantsLayer:YES];
        nameField.stringValue = name;
        nameField.drawsBackground = NO;
        [nameField setTextColor:[NSColor whiteColor]];
        nameField.font = fieldFont;
        
        [oneToOneContentView_ addSubview:normalImageView];
        [oneToOneContentView_ addSubview:nameField];
        
        [oneToOneContentView_ setHidden:YES];
        [adaptiveControlView_ addSubview:oneToOneContentView_];

    }
    //
    {
        adaptiveContentView_ = [[NSView alloc]initWithFrame:contentRect];
        adaptiveContentView_.autoresizingMask = NSViewNotSizable;
        adaptiveContentView_.autoresizesSubviews = NO;
        
        NSImage * hilightImage = [NSImage imageNamed:@"mav_adaptive_normal.png"];
        NSRect hilightImageRect =NSMakeRect(10.0,8.0,13.0,10.0);
        NSImageView *hilightImageView =  [[[NSImageView alloc] initWithFrame:hilightImageRect]autorelease];
        hilightImageView.bounds = hilightImageRect;
        hilightImageView.image  = hilightImage;
        [hilightImageView setImageScaling:NSImageScaleProportionallyDown];
        
        NSString *name  = @"自适应";
        NSFont *fieldFont = [NSFont fontWithName:@"Arial" size:12.0f];
        NSTextField* nameField = [[[NSTextField alloc] initWithFrame:NSMakeRect(25.0, 4.0, 40, 20.0)]autorelease];
        nameField.bezeled = NO;
        nameField.editable = NO;
        [nameField setWantsLayer:YES];
        nameField.stringValue = name;
        nameField.drawsBackground = NO;
        [nameField setTextColor:[NSColor whiteColor]];
        nameField.font = fieldFont;
        
        [adaptiveContentView_ addSubview:hilightImageView];
        [adaptiveContentView_ addSubview:nameField];
        [adaptiveContentView_ setHidden:YES];
        [adaptiveControlView_ addSubview:adaptiveContentView_];
    }
    [self updateLayout];
}

-(void)setAdaptiveControllerView
{
    if ((self.type != 0)||(!big_) || ( self.uin== [utils_ getSelfUin]))        //不是辅路视频需要隐藏 uin_
    {
        [adaptiveContentView_ setHidden:YES];
        [oneToOneContentView_ setHidden:YES];
        [adaptiveControlView_ setHidden:YES];
        [backImageView_ setHidden:YES];
        [adaptiveControlView_.layer setNeedsDisplay];
        [tipsView_ setHidden:YES];
    }
    else if (adaptiveStatus_)
    {
        [adaptiveControlView_ setHidden:NO];
        
        [adaptiveContentView_ setHidden:NO];
        [oneToOneContentView_ setHidden:YES];
        [backImageView_ setHidden:NO];
        [tipsView_ setHidden:NO];
    }
    else
    {
        [adaptiveControlView_ setHidden:NO];
        
        [adaptiveContentView_ setHidden:YES];
        [oneToOneContentView_ setHidden:NO];
        [backImageView_ setHidden:NO];
        [tipsView_ setHidden:NO];
    }
    [adaptiveControlView_ setNeedsDisplay:YES];
    
    if (adaptiveStatus_)
    {
        businessInfo_.hidden = TRUE;
    }
    else if (big_ && self.type == 0 && self.uin == [utils_ getSelfUin])//uin_
    {
        businessInfo_.hidden = FALSE;
    }
    else
    {
        businessInfo_.hidden = TRUE;
    }
}

-(void)switchToPureStateNotify:(BOOL)pure
{
    if(pure)
    {
        [adaptiveControlView_ setHidden:YES];
    }
    else
    {
        [self setAdaptiveControllerView];
        [adaptiveControlView_ setHidden:NO];
    }
}

#pragma mark Title panel view
-(void)addBottomTitleView:(NSView *)contentView
{
    NSView *titleContentView = [[[NSView alloc]init]autorelease];
    titlePanelView_ = titleContentView;
    titleContentView.translatesAutoresizingMaskIntoConstraints = NO;
    [contentView addSubview:titleContentView];
    NSLayoutConstraint* _widthConstraint = [NSLayoutConstraint constraintWithItem:contentView attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationEqual toItem:titleContentView attribute:NSLayoutAttributeWidth multiplier:1.0 constant:0];
    [contentView addConstraint:_widthConstraint];
    
    NSLayoutConstraint *_heightConstraint = [NSLayoutConstraint constraintWithItem:contentView attribute:NSLayoutAttributeHeight relatedBy:NSLayoutRelationEqual toItem:titleContentView attribute:NSLayoutAttributeHeight multiplier:2.0 constant:0];
    [contentView addConstraint:_heightConstraint];
    
    titleContentView.autoresizesSubviews = YES;
    
    NSFont *fieldFont = [NSFont fontWithName:@"Arial" size:12.0f];
    NSTextField* nameField = [[[NSTextField alloc] init] autorelease];
    nameField.bezeled = NO;
    nameField.editable = NO;
    nameField.stringValue = @"haha";
    //[utils_ getNameByUin:self.uin font:fieldFont maxWidth:70];
    nameField.drawsBackground = NO;
    [nameField setTextColor:[NSColor whiteColor]];
    nameField.font = fieldFont;
    [nameField setFrame:NSMakeRect(7, 5, 200, 20)];

    nameField_ = nameField;
    //初始化的时候隐藏
//    [titlePanelView_ setHidden:YES];
}


-(void)setVisible:(BOOL)visible
{
    visible_ = visible;
//    [rootView_ setHidden:!visible_];
}

-(void)setHovered:(BOOL)hovered
{
    hovered_ = hovered;
//    [titlePanelView_ setHidden:!hovered_];
}

-(void)setBig:(BOOL)big
{
    big_ = big;
    if (big_){
        backView_.fileName = @"black.png";
        [videoView_ setColor:[NSColor colorWithRed:0.13f green:0.13f blue:0.13f alpha:1.0f]];
    }
    else{
        backView_.fileName = @"black.png";
        [videoView_ setColor:[NSColor blackColor]];
    }
    [backView_ setNeedsDisplay:YES];
    [self updateLayout];
}

#pragma mark Process video data
-(void)output:(unsigned char*)rgb24 width:(int)width height:(int)height angle:(int)angle uin:(NSString *)uin type:(int)type
{
//    NSLog(@"out put video data, width:%d, height:%d, angle:%d, uin:%llu, type:%d", width, height, angle, uin, type);
    if (rgb24 == NULL)
    {
        return;
    }
    //截图
    if (captureVideoData_)
    {
        [utils_ saveRGBDataAsImageFile:(char*)rgb24 width:width height:height file:[NSString stringWithFormat:@"%@.%d.png",uin,type]];
        captureVideoData_ = NO;
    }
    videoFrameInfo_.width = width;
    videoFrameInfo_.height = height;
    videoFrameInfo_.angle =  angle;
    
    {
        if (videoFrameInfo_.bufSize != (uint64)(width * height * 3))
        {
            if (videoFrameInfo_.rgb24 != NULL)
            {
                free(videoFrameInfo_.rgb24);
                videoFrameInfo_.rgb24 = NULL;
                videoFrameInfo_.bufSize = 0;
            }
            videoFrameInfo_.bufSize = (uint64)(width * height * 3);
            videoFrameInfo_.rgb24 = (unsigned char*)malloc(videoFrameInfo_.bufSize);
        }
        if (videoFrameInfo_.rgb24 == NULL)
        {
            NSLog(@"mav memory error!");
            return;
        }
        memcpy(videoFrameInfo_.rgb24, rgb24,  width*height * 3);
    }
    
    [self setVideoSizeWidth:width height:height angle:angle];
    self.dataReady = YES;
    
    if (big_)
    {
        backView_.color = [NSColor colorWithRed:0.13f green:0.13f blue:0.13f alpha:1.0f];
    }
    else
    {
        backView_.color = [NSColor blackColor];
    }
    [backView_ setNeedsDisplay:TRUE];
    [videoView_ setHidden:NO];
    [videoView_ output:rgb24 width:width height:height angle:angle type:type];
    [videoView_.layer setNeedsDisplay];
    
    //判断bigview ratio是否改变
    if (big_) {
        CGFloat bigVideoFrameRatio = (CGFloat)videoFrameInfo_.width / (CGFloat)videoFrameInfo_.height;
        if(bigViewRatio_ != bigVideoFrameRatio)
        {
            bigViewRatio_ = bigVideoFrameRatio;
//            [[NSNotificationCenter defaultCenter] postNotificationName:BigViewFrameRatio_Changed object:nil];
        }
    }
}

//设置旋转后当前真实的video高宽
-(void)setVideoSizeWidth:(int)width height:(int)height angle:(int)angle
{
    //90 or 270
    if((angle == 1) || (angle == 3)){
        videoSize_.width = height;
        videoSize_.height = width;
    }
    else{
        videoSize_.width = width;
        videoSize_.height = height;
    }
}


-(NSString *)description
{
    return [NSString stringWithFormat:@"uin:%@, type:%d, isbig:%@, width:%f, height:%f, videoWidth:%f, videoHeight:%f", self.uin, self.type, big_ ?@"YES":@"NO", rootView_.frame.size.width, rootView_.frame.size.height, videoView_.frame.size.width, videoView_.frame.size.height];//uin_
}

#pragma mark Process delegate of events
-(void)clickVideoView:(NSPoint)theEvent
{
     NSLog(@"click Video View(uin:%@, type:%d) at x:%f, y:%f", self.uin,self.type, theEvent.x, theEvent.y);
    //大画面不响应点击切换事件(大小画面切换)
    if (self.big)
    {
        [self setAdaptiveControllerView];
        return;
    }
    [layoutmanagerDelegate_ requestForLargeControllerView:self];
    [self setAdaptiveControllerView];
    [self updateViewConstraints];
}


- (void)hoverVideoView:(BOOL)hovered
{
    [layoutmanagerDelegate_ requestForHoveredControllerView:self hovered:hovered];
}

#pragma mark Adaptive view
-(void)setAdaptiveStatus:(BOOL)flag
{
    adaptiveStatus_ = flag;
    [self updateLayout];
}

-(void) updateLayout
{
    if (adaptiveStatus_)
    {
        [self removeVideoViewConstraints];
        [videoView_ setTranslatesAutoresizingMaskIntoConstraints:YES];
        videoView_.autoresizingMask =  NSViewMinXMargin|NSViewMinYMargin|NSViewNotSizable|NSViewMaxXMargin|NSViewMaxYMargin;
        NSRect superRect = [self.view bounds];
        float offset_x = (superRect.size.width - videoSize_.width) / 2.0;
        float offset_y = (superRect.size.height - videoSize_.height) / 2.0;
        [videoView_ setFrameOrigin:NSMakePoint(offset_x, offset_y)];
        [videoView_ setFrameSize:videoSize_];
        self.view.layer.backgroundColor = [NSColor blackColor].CGColor;
    }
    else
    {
        videoView_.autoresizingMask =  NSViewHeightSizable|NSViewWidthSizable;
        NSRect superRect = [self.view bounds];
        [videoView_ setFrame:superRect];
        [self setVideoViewConstraints];
    }
    [self setAdaptiveControllerView];
    [self output:videoFrameInfo_.rgb24 width:videoFrameInfo_.width height:videoFrameInfo_.height angle:videoFrameInfo_.angle uin:self.uin type:self.type];
    [self updateVideoViewConstraints];
}


-(BOOL)mouseDownCanMoveWindow
{
    return  !adaptiveStatus_;
}

- (void)didEnterFull:(NSNotification *)aNotification
{
    if(!self.adaptiveStatus)
    {
        return;
    }
    NSRect rectSuperView = videoView_.superview.frame;
    NSRect rectVideoView = videoView_.frame;
    NSLog(@"Enter invalid position, x:%f, y:%f, w:%f, h:%f",rectVideoView.origin.x, rectVideoView.origin.y, rectVideoView.size.width,rectVideoView.size.height);
    NSPoint orpt;
    orpt.x = -(rectVideoView.size.width - rectSuperView.size.width)/2.0;
    orpt.y = -(rectVideoView.size.height - rectSuperView.size.height)/2.0;
    [[videoView_ animator] setFrameOrigin:orpt];
}

-(void)didExitFull:(NSNotification *)aNotification
{
    if(!self.adaptiveStatus)
    {
        return;
    }
    NSRect rectSuperView = videoView_.superview.frame;
    NSRect rectVideoView = videoView_.frame;
    NSLog(@"Exit invalid position, x:%f, y:%f, w:%f, h:%f",rectVideoView.origin.x, rectVideoView.origin.y, rectVideoView.size.width,rectVideoView.size.height);
    NSPoint orpt;
    orpt.x = -(rectVideoView.size.width - rectSuperView.size.width)/2.0;
    orpt.y = -(rectVideoView.size.height - rectSuperView.size.height)/2.0;
    [[videoView_ animator] setFrameOrigin:orpt];
}

@end

@implementation MAVEventBackView

-(id)initWithFrame:(NSRect)frameRect controller:(MAVBasicVideoViewController*)controller
{
    self = [super initWithFrame:frameRect];
    if(self)
    {
        trackingArea_ = [[NSTrackingArea alloc] initWithRect:self.bounds
                                                     options: (NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveAlways)
                                                       owner:self userInfo:nil];
        [self addTrackingArea:trackingArea_];
        controller_ = controller;
    }
    return self;
}
-(void)dealloc
{
    [trackingArea_ release];
    trackingArea_ = nil;
    
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:NSWindowDidEnterFullScreenNotification
                                                  object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:NSWindowDidExitFullScreenNotification
                                                  object:nil];

    [super dealloc];
}

- (void)updateTrackingAreas
{
    [self removeTrackingArea:trackingArea_];
    [trackingArea_ release];
    trackingArea_ = [[NSTrackingArea alloc] initWithRect:self.bounds
                                                 options: (NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveAlways)
                                                   owner:self userInfo:nil];
    [self addTrackingArea:trackingArea_];

}

- (void)mouseUp:(NSEvent *)theEvent
{
    NSLog(@"MAV. mouseUp %@", theEvent);
    if ((!controller_.big)||(controller_.type != 0) || controller_.dataReady == FALSE)
    {
        return;
    }
    controller_.adaptiveStatus = !controller_.adaptiveStatus;
//    [super mouseUp:theEvent];
}

@end











