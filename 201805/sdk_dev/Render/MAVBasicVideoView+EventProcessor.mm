//
//  MAVBasicVideoView+EventProcessor.m
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import "MAVBasicVideoView+EventProcessor.h"

@implementation MAVBasicVideoView (EventProcessor)

//判定鼠标是否在当前视图内
- (void)updateTrackingAreas
{
    NSLog(@"Invoke updateTrackingAreas:");
    [self removeTrackingArea:trackingArea_];
    [trackingArea_ release];
    trackingArea_ = [[NSTrackingArea alloc] initWithRect:self.bounds
                                                options: (NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveAlways)
                                                  owner:self userInfo:nil];
    [self addTrackingArea:trackingArea_];
    
    NSPoint mouseLocation = [[self window] mouseLocationOutsideOfEventStream];
    mouseLocation = [self convertPoint: mouseLocation fromView: nil];
    NSRect selfBounds = [self bounds];
    if (NSPointInRect(mouseLocation, selfBounds))
    {
        [self mouseEntered: nil];
    }
    else
    {
        [self mouseExited: nil];
    }
    
//    [super updateTrackingAreas];
}

-(void)mouseEntered:(NSEvent *)theEvent {
    NSLog(@"MAV. mouseEntered %@", theEvent);
//    [super mouseEntered:theEvent];
    [controllerDelegate_ hoverVideoView:YES];
}

-(void)mouseExited:(NSEvent *)theEvent
{
    NSLog(@"MAV. mouseExited %@", theEvent);
//    [super mouseExited:theEvent];
    [controllerDelegate_ hoverVideoView:NO];
}

- (void)mouseMoved:(NSEvent *)theEvent;
{
//    NSLog(@"MAV Mouse move %@", theEvent);
//    NSPoint location = [theEvent locationInWindow];
//    NSPoint viewlocation = [self convertPoint:location fromView:nil];
    [super mouseMoved:theEvent];
}

- (void)mouseUp:(NSEvent *)theEvent
{
    NSLog(@"MAV. mouseUp %@", theEvent);
    NSPoint location = [theEvent locationInWindow];
    NSPoint viewlocation = [self convertPoint:location fromView:nil];
    [controllerDelegate_ clickVideoView:viewlocation];
    [self.window mouseUp:theEvent];
}

static NSPoint lastDragLocation;
- (void)mouseDown:(NSEvent *) theEvent {
    lastDragLocation = [theEvent locationInWindow];
}


-(NSPoint)validPosition:(NSPoint)orpt
{
    NSRect surect = self.superview.bounds;
    NSRect slrect = self.bounds;
    
    if(slrect.size.height < surect.size.height){
        if(orpt.y  < 0.0)
        {
            orpt.y = 0.0;
        }
        else if(orpt.y  + slrect.size.height > surect.size.height)
        {
            orpt.y = surect.size.height - slrect.size.height;
        }
        
    }else{
        if(orpt.y < 0)
        {
            if(orpt.y  +  slrect.size.height <  surect.size.height)
            {
                orpt.y  = surect.size.height - slrect.size.height;
            }
        }
        else
        {
            orpt.y  = 0;
        }
    }
    
    //
    if(orpt.x  > 0.0)
    {
        orpt.x = 0.0;
    }
    if(orpt.x  + slrect.size.width < surect.size.width)
    {
        orpt.x = surect.size.width - slrect.size.width;
    }
    
    
    
    return orpt;
}

- (void)mouseDragged:(NSEvent *)theEvent {
//    NSLog(@"----mouseDragged----");
    NSPoint newDragLocation = [theEvent locationInWindow];
    NSPoint thisOrigin = [self frame].origin;
    thisOrigin.x += (-lastDragLocation.x + newDragLocation.x);
    thisOrigin.y += (-lastDragLocation.y + newDragLocation.y);
    if(![self mouseDownCanMoveWindow])
    {
        [self setFrameOrigin:[self validPosition:thisOrigin]];
    }
    lastDragLocation = newDragLocation;
}


-(BOOL)mouseDownCanMoveWindow
{
    return [controllerDelegate_ mouseDownCanMoveWindow];
}


@end
