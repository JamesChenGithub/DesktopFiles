//
//  MAVBasicVideoView.m
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import "MAVBasicVideoView.h"
#import "MAVCAOpenGLLayer.h"
@implementation MAVBasicVideoView
@synthesize controllerDelegate = controllerDelegate_;


-(id)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self)
    {
        [self setupCAOpenGLLayer];
        trackingArea_ = [[NSTrackingArea alloc] initWithRect:self.bounds
                                                    options: (NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveAlways)
                                                      owner:self userInfo:nil];
        [self addTrackingArea:trackingArea_];
    }
    return self;
}

-(void)dealloc
{
    self.color = nil;
    [trackingArea_ release];
    trackingArea_ = nil;
    
    [super dealloc];
}

-(void)setupCAOpenGLLayer
{
    layer_ = [MAVCAOpenGLLayer layer];
    self.layer =  layer_;
    [layer_ setNeedsDisplay];
}

- (void) setColor:(NSColor *)color
{
    [layer_ setColor:color];
}

-(void)viewDidMoveToSuperview{
    [super viewDidMoveToSuperview];
    [self setWantsLayer:YES];
}

-(void)output:(unsigned char*)rgb24 width:(int)width height:(int)height angle:(int)angle type:(int)type
{
    [layer_ output:rgb24 width:width height:height angle:angle type:type];
}



@end
