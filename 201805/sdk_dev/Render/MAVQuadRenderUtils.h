//
//  MAVQuadRenderUtils.hpp
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#ifndef MAVQuadRenderUtils_h
#define MAVQuadRenderUtils_h
#import <AVFoundation/AVFoundation.h>
#include <stdio.h>
#pragma mark Process image file
//以下是处理图片资源的类接口
typedef struct tagImageSize
{
    int width;
    int height;
}ImageSize;

struct IResourceManager
{
    virtual void loadImage(const NSString *file) = 0;
    virtual void* getImageData() = 0;
    virtual ImageSize getImageSize() = 0;
    virtual void  unloadImage() = 0;
};


class ResourceManagerImpl :public IResourceManager
{
public:
    void loadImage(const NSString *file);
    void* getImageData();
    ImageSize getImageSize();
    void unloadImage();
    
private:
    ImageSize _imageSize;
    void * _imageData;
    
};


extern "C" IResourceManager *createResourceManager();

#pragma mark Process file content of string type
//以下是处理文本文件的类接口
struct IResourceStringFileManager
{
    virtual const char * getFileContent(NSString *file) = 0;
};

class ResourceStringFileManagerImpl:public IResourceStringFileManager
{
    const char * getFileContent(NSString *file);
};

extern "C" IResourceStringFileManager *createResourceStringFileManager();



#endif /* MAVQuadRenderUtils_hpp */
