//
//  MAVRenderUtils.h
//  QQ4Mac
//
//  Created by nickdai on 15/11/26.
//  Copyright © 2015年 tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
@interface MAVRenderUtils : NSObject

//保存RGB数据为图片
-(void)saveRGBDataAsImageFile:(char *)myBuffer width:(int)width height:(int)height file:(NSString*)file;


@end
