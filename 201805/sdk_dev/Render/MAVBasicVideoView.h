//
//  MAVBasicVideoView.h
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MAVCAOpenGLLayer.h"

@protocol MAVBasicVideoViewControllerDelegate <NSObject>
-(void)clickVideoView:(NSPoint)theEvent;
-(void)hoverVideoView:(BOOL)hovered;
-(BOOL)mouseDownCanMoveWindow;
@end

@interface MAVBasicVideoView : NSView
{
    MAVCAOpenGLLayer *layer_;
    NSTrackingArea * trackingArea_;
    id<MAVBasicVideoViewControllerDelegate> controllerDelegate_;
}

@property (nonatomic,assign) id<MAVBasicVideoViewControllerDelegate> controllerDelegate;

-(void)output:(unsigned char*)rgb24 width:(int)width height:(int)height angle:(int)angle type:(int)type;
-(id)initWithFrame:(NSRect)frameRect;
- (void) setColor:(NSColor *)color;
@end
