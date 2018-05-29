//
//  MAVQuadRenderUtils.cpp
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#include "MAVQuadRenderUtils.h"
#pragma mark Process Image file
static ImageSize zeroSize = {0,0};

void ResourceManagerImpl::loadImage(const NSString *file)
{
#define RGBA_SIZE	4
    NSUInteger				width, height;
    NSURL					*url = nil;
    CGImageSourceRef		src = nil;
    CGImageRef				image = nil;
    CGContextRef			context = nil;
    CGColorSpaceRef			colorSpace = nil;
    
    NSArray*				ns = [file componentsSeparatedByString:@"."];
    url = [NSURL fileURLWithPath: [[NSBundle mainBundle] pathForResource:[ns objectAtIndex:0] ofType:[ns objectAtIndex:1]]];
    src = CGImageSourceCreateWithURL((CFURLRef)url, NULL);
    
    if (!src) {
        NSLog(@"No image");
        return ;
    }
    image = CGImageSourceCreateImageAtIndex(src, 0, NULL);
    CFRelease(src);
    
    _imageSize.width = width = CGImageGetWidth(image);
    _imageSize.height = height = CGImageGetHeight(image);
    if(_imageData != NULL)
    {
        this->unloadImage();
    }
    _imageData = calloc(width * height * RGBA_SIZE, sizeof(GLubyte));
    colorSpace = CGColorSpaceCreateDeviceRGB();
    context = CGBitmapContextCreate(_imageData, width, height, 8, 4 * width, colorSpace, kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host);
    CGColorSpaceRelease(colorSpace);
    
    CGContextTranslateCTM(context, 0.0, 0.0);
    CGContextScaleCTM(context, 1.0, 1.0);
    CGContextSetBlendMode(context, kCGBlendModeCopy);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);
    
    CGContextRelease(context);
    CGImageRelease(image);
    
}

void* ResourceManagerImpl::getImageData()
{
    return _imageData;
}


ImageSize ResourceManagerImpl::getImageSize()
{
    return _imageSize;
}

void ResourceManagerImpl::unloadImage()
{
    
    if(_imageData != NULL)
    {
        free(_imageData);
        _imageData = NULL;
    }
    _imageSize = zeroSize;
}

//创建资源管理器的C语言接口函数
IResourceManager * createResourceManager()
{
    return new ResourceManagerImpl();
}


#pragma mark Process file content of text
const char * ResourceStringFileManagerImpl::getFileContent(NSString *file)
{
    const char *source;
    NSArray* ns = [file componentsSeparatedByString:@"."];
    NSString *filePath  = [[NSBundle mainBundle] pathForResource:[ns objectAtIndex:0] ofType:[ns objectAtIndex:1]];
    source = (GLchar *)[[NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!source)
    {
        NSLog(@"Failed to load file");
        return NULL;
    }
    return source;
}

IResourceStringFileManager *createResourceStringFileManager()
{
    return new ResourceStringFileManagerImpl();
}


