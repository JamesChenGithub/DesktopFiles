//
//  MAVBackView.h
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import <Cocoa/Cocoa.h>
//视频的背景视图，可设置为图像或者纯色
@interface MAVBackView : NSView
{
    NSColor *color_;
    NSString *fileName_;
    BOOL transparent_;
}

@property (nonatomic,retain) NSColor *color;
@property (nonatomic,retain) NSString * fileName;

-(id)initWithFrame:(NSRect)frameRect color:(NSColor*)color;
-(id)initWithFrame:(NSRect)frameRect file:(NSString*)file;

@end
