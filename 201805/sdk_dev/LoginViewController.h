//
//  LoginViewController.h
//  QAVSDKDemo
//
//  Created by dackli on 16/4/13.
//  Copyright © 2016年 qqconnect. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "Render/MAVVideoDataDispatcher.h"
#import "Render/MAVRender.h"

@protocol UIDelegate <NSObject>

@required

- (int32_t)appid;
- (int32_t)roomid;
- (int32_t)identifier;
- (const char*)role;
- (void)addCamera:(NSString*)name;
- (void)removeCamera:(NSString*)name;
- (void)addSpeaker:(NSString*)name;
- (void)removeSpeaker:(NSString*)name;
- (void)updateSpesker:(NSString*)name state:(int)state;
- (void)addMic:(NSString*)name;
- (void)removeMic:(NSString*)name;
- (void)updateMic:(NSString*)name state:(int)state;
- (void)setTips:(NSString*)tips;
- (void)onDisconnect;
- (void)outTrackAdd:(NSString *)name;
@end

@interface LoginViewController : NSViewController<UIDelegate,NSComboBoxDelegate>
{
  BOOL _isViewLoad;
  MAVRenderImpl          *_renderImpl;
  MAVVideoDataDispatcher *_frameDispatcher;
}
- (void)initUI;
@end

