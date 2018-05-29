//
//  MAVRender.m
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MAVRender.h"
#import "MAVBackView.h"
#import "MAVVideoDataDispatcher.h"
#import "MAVLayoutManager.h"

@implementation MAVRenderImpl
@synthesize utils = utils_;
@synthesize layoutManager = _layoutManager;
-(id)init

{
    self = [super init];
    if(self)
    {
        _layoutManager =  [[MAVLayoutManager alloc]initWithUtils:utils_];
        dispatcherObject_ = [[MAVVideoDataDispatcher alloc]initWithLayoutManager:_layoutManager];
        
        rootView_ = [[NSView alloc]initWithFrame:NSMakeRect(0, 0, 800, 600)];
        rootView_.autoresizesSubviews = YES;
        rootView_.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        [rootView_ setWantsLayer:YES];
        
        NSView *backgroundView = [[[MAVBackView alloc]initWithFrame:NSMakeRect(0, 0, 800, 600) color:[NSColor blackColor]]autorelease];
        //NSMakeRect(6, 203, 420, 315)
        backgroundView.autoresizesSubviews = YES;
        backgroundView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        NSView *videoContentView = [_layoutManager getVideoContentView];
        [backgroundView addSubview: videoContentView];
        videoContentView.translatesAutoresizingMaskIntoConstraints = NO;
        
        NSLayoutConstraint* _aspectConstraint = [NSLayoutConstraint constraintWithItem:videoContentView attribute:NSLayoutAttributeHeight relatedBy:NSLayoutRelationEqual toItem:videoContentView attribute:NSLayoutAttributeWidth multiplier:3.0/4.0 constant:0];
        [videoContentView addConstraint:_aspectConstraint];
        
        NSLayoutConstraint *_widthConstraint = [NSLayoutConstraint constraintWithItem: backgroundView attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationEqual toItem:videoContentView attribute:NSLayoutAttributeWidth multiplier:1.0 constant:0];
        [backgroundView addConstraint:_widthConstraint];
        
        NSLayoutConstraint *_heightConstraint = [NSLayoutConstraint constraintWithItem: backgroundView attribute:NSLayoutAttributeHeight relatedBy:NSLayoutRelationEqual toItem:videoContentView attribute:NSLayoutAttributeHeight multiplier:1.0 constant:0];
        [backgroundView addConstraint:_heightConstraint];
        
        [rootView_ addSubview:backgroundView];
        _layoutManager.videoRootView = backgroundView;
    }
    return self;
}

-(void)dealloc
{
    [rootView_ release];
    rootView_ = nil;
    
    [dispatcherObject_ release];
    dispatcherObject_ = nil;
    
    [_layoutManager release];
    _layoutManager = nil;
    
    [utils_ release];
    utils_ = nil;
    
    [super dealloc];
}

-(NSView*)view
{
    return rootView_;
}

-(void)onUserVideoStateChange:(NSString *)uin :(bool)bOpenOrClose :(bool)bMainOrSub :(bool)bActiveOrAppend
{
    int type = bMainOrSub? 1 : 0;
    if (bOpenOrClose)
    {
        [_layoutManager addVideoView:uin type:type action:bActiveOrAppend];
    }
    else
    {
        [_layoutManager removeVideoView:uin type:type];
    }
}


-(void)dispatchVideoData:(tagMAVVideoData*)data
{
    if(NULL != data)
    {
        unsigned char *buf = data->rgb24buf;
        NSString *uin      = [NSString stringWithCString:data->ullUin encoding:NSUTF8StringEncoding];
        int type           = data->bMainData ? 1 : 0;
        int height         = data->nHeight;
        int width          = data->nWidth;
        int angle          = data->nRotate;
        [dispatcherObject_ dispatchVideoData:buf width:width height:height angle:angle uin:uin type:type];
    }
    
}


-(void)hideSmallVideoViews:(BOOL)show
{
    [_layoutManager setSmallVideoViewVisible:show];
}


-(NSArray*)getVideoInfo
{
    return [_layoutManager getAllVideoViewInfo];
}


-(void)setPureState:(BOOL)pure
{
    [_layoutManager  switchToPureState:pure];
}

@end
