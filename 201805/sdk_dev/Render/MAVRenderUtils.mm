//
//  MAVRenderUtils.m
//  QQ4Mac
//
//  Created by nickdai on 15/11/26.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import "MAVRenderUtils.h"

@implementation MAVRenderUtils



//保存RGB数据为图片
-(void)saveRGBDataAsImageFile:(char *)myBuffer width:(int)width height:(int)height file:(NSString*)file
{
    if (0 == width || 0 == height) {
        return;
    }
    char* rgba = (char*)malloc(width*height*4);
    for(int i=0; i < width*height; ++i) {
        rgba[4*i] = myBuffer[3*i];
        rgba[4*i+1] = myBuffer[3*i+1];
        rgba[4*i+2] = myBuffer[3*i+2];
        rgba[4*i+3] = 0;
    }
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef bitmapContext = CGBitmapContextCreate(
                                                       rgba,
                                                       width,
                                                       height,
                                                       8, // bitsPerComponent
                                                       4*width, // bytesPerRow
                                                       colorSpace,
                                                       kCGImageAlphaNoneSkipLast);
    
    CFRelease(colorSpace);
    
    CGImageRef cgImage = CGBitmapContextCreateImage(bitmapContext);
    CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,(__bridge CFStringRef)file, kCFURLPOSIXPathStyle, false);
    
    CFStringRef type = kUTTypePNG; // or kUTTypeBMP if you like
    CGImageDestinationRef dest = CGImageDestinationCreateWithURL(url, type, 1, 0);
    
    CGImageDestinationAddImage(dest, cgImage, 0);
    
    CFRelease(cgImage);
    CFRelease(bitmapContext);
    CGImageDestinationFinalize(dest);
    CFRelease(url);
    CFRelease(dest);
    free(rgba);
}


@end
