//
//  MAVBackView.m
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import "MAVBackView.h"

@implementation MAVBackView

@synthesize color = color_;
@synthesize fileName = fileName_;

- (void)drawRect:(NSRect)dirtyRect
{
    if (color_){
        [color_ set];
        NSRectFill(dirtyRect);
    }
    else if (fileName_){
        [[NSGraphicsContext currentContext]setImageInterpolation: NSImageInterpolationHigh];
        NSImage *image = [NSImage imageNamed:fileName_];
        [image drawInRect: self.bounds fromRect: NSZeroRect operation: NSCompositeSourceOver fraction: 1.0];
    }
}

-(id)initWithFrame:(NSRect)frameRect color:(NSColor*)color
{
    self = [super initWithFrame:frameRect];
    if(self)
    {
        color_ =  [color retain];
    }
    return self;
}

-(id)initWithFrame:(NSRect)frameRect file:(NSString*)file
{
    self = [super initWithFrame:frameRect];
    if(self)
    {
        fileName_ =  [file retain];
    }
    return self;
}

-(id)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if(self)
    {
        CGFloat r = ( arc4random() % 256 / 256.0 );
        CGFloat g = ( arc4random() % 128 / 256.0 ) + 0.5;
        CGFloat b = ( arc4random() % 128 / 256.0 ) + 0.5;
//        color_ =  [[NSColor colorWithHue:hue saturation:saturation brightness:brightness alpha:1] retain]; //support from 10.9
        color_ =  [[NSColor colorWithSRGBRed:r green:g blue:b alpha:1] retain];
    }
    return self;
}

-(void)dealloc
{

    [color_ release];
    color_ = nil;
    [fileName_ release];
    fileName_ = nil;
    [super dealloc];
}

@end
