//
//  MAVCAOpenGLLayer.m
//  QQ4Mac
//
//  Created by nickdai on 15/11/27.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import "MAVCAOpenGLLayer.h"
#import "MAVQuadRender.h"

@implementation MAVCAOpenGLLayer

#pragma mark Construction and unconstruction


-(void)dealloc
{
    self.color = nil;
    
    //尼玛 坑爹啊！@ dealloc的时候也需要设置上下文，不然会导致删除别的视图的GL资源
    CGLSetCurrentContext(mContext_);
    CGLLockContext(mContext_);
    if (render_ != NULL)
    {
        delete (MAVQuadRender*)render_;
        render_ = NULL;
    }
    CGLUnlockContext(mContext_);
    [super dealloc];
}

#pragma mark Config Opengl environment
// 重写父类的方法，提供自己的CGLPixelFormatObj
- (CGLPixelFormatObj)copyCGLPixelFormatForDisplayMask:(uint32_t)mask
{
    int attribs[] =  {
        
        kCGLPFADisplayMask, 0,
        kCGLPFAColorSize, 24,
        kCGLPFAAccelerated,
        kCGLPFADoubleBuffer,
        // Use OpenGL 3.2 Core Profile
        kCGLPFAOpenGLProfile, (CGLPixelFormatAttribute)kCGLOGLPVersion_3_2_Core,
        // Use multi-sample
        kCGLPFAMultisample,
        kCGLPFASampleBuffers, (CGLPixelFormatAttribute)1,
        kCGLPFASamples, (CGLPixelFormatAttribute)4,
        0
    };
    // 将kCGLPFADisplayMask设置为传递过来的display mask。这个步骤是必须的！
    attribs[1] = mask;
    CGLPixelFormatObj pixFormatObj = NULL;
    GLint numPixFormats = 0;
    CGLChoosePixelFormat((const CGLPixelFormatAttribute*)attribs, &pixFormatObj, &numPixFormats);
    return pixFormatObj;
}

#pragma mark Opengl drawing
-(void)setupContext
{
    if(render_ == NULL)
    {
        render_ = new MAVQuadRender();
    }
    //渲染范围不是全屏，而是从顶点开始，与iOS不同
    glViewport(viewPort_.origin.x,viewPort_.origin.y,viewPort_.size.width + 1,viewPort_.size.height + 1);
}

- (void)drawInCGLContext:(CGLContextObj)glContext
             pixelFormat:(CGLPixelFormatObj)pixelFormat
            forLayerTime:(CFTimeInterval)timeInterval
             displayTime:(const CVTimeStamp *)timeStamp
{
    mContext_ = glContext;
    CGLSetCurrentContext(mContext_);
    [self setupContext];
    [self render];
    CGLUnlockContext(mContext_);
}

- (void)releaseCGLPixelFormat:(CGLPixelFormatObj)pixelFormat {
    CGLDestroyPixelFormat(pixelFormat);
}

- (void)render
{
    MAVQuadRender *renderObject = (MAVQuadRender*)render_;
    glClearColor(self.color.redComponent, self.color.greenComponent, self.color.blueComponent, self.color.alphaComponent);
    glClear(GL_COLOR_BUFFER_BIT);
    if(render_ != NULL)
    {
        renderObject->Draw();
    }
    glFlush();
    CGLFlushDrawable(mContext_);
}

-(void)updateViewPortByWidth:(int)width height:(int)height angle:(int)angle
{
    //90或者270的时候需要进行宽高转换
    if((angle == 1) ||(angle == 3))
    {
        int t = width;
        width = height;
        height = t;
    }
    int viewWidth = [self frame].size.width;
    int viewHeight = [self frame].size.height;
    float viewratio =  (float)viewHeight / (float)viewWidth;
    float ratio = (float)height / (float)width;
    int portWidth, portHeight;
    
    if(ratio <= viewratio){
        portHeight =  (int)((float)viewWidth * ratio);
        portWidth = viewWidth;
        viewPort_.size.width =  portWidth;
        viewPort_.size.height = portHeight;
        viewPort_.origin.x = 0;
        viewPort_.origin.y = (viewHeight - portHeight) / 2.0;
    }
    else{
        portHeight = viewHeight;
        portWidth = (int)((float)portHeight / ratio);
        viewPort_.size.width =  portWidth;
        viewPort_.size.height = portHeight;
        viewPort_.origin.x = (viewWidth - portWidth) / 2.0;
        viewPort_.origin.y = 0;
    }
}

-(void)output:(unsigned char*)rgb24 width:(int)width height:(int)height angle:(int)angle  type:(int)type
{
    CGLSetCurrentContext(mContext_);
    CGLLockContext(mContext_);
    MAVQuadRender *renderObject = (MAVQuadRender*)render_;
    if (render_ != NULL)
    {
        renderObject->updateTextureResource(rgb24, width, height, angle, type);
    }
    [self updateViewPortByWidth:width height:height angle:(int)angle];
    CGLUnlockContext(mContext_);
}

@end
