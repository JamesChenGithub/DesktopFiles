//
//  MAVCAOpenGLLayer.h
//  QQ4Mac
//
//  Created by nickdai on 15/11/27.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <OpenGL/gl3.h>

@interface MAVCAOpenGLLayer : CAOpenGLLayer
{
    CGLContextObj mContext_;
    void *render_;
    NSRect viewPort_;
}

-(void)output:(unsigned char*)rgb24 width:(int)width height:(int)height angle:(int)angle type:(int)type;

@property (nonatomic, retain) NSColor* color;

@end