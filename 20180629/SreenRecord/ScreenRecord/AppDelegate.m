//
//  AppDelegate.m
//  SreenRecord
//
//  Created by AlexiChen on 2018/5/29.
//  Copyright © 2018年 AlexiChen. All rights reserved.
//

#import "AppDelegate.h"

#import <ILiveSDK/ILiveLoginManager.h>
@interface AppDelegate ()<QAVRecordDelegate>

@property (nonatomic, strong) QAVScreenRecord *screenRecord;

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
    [[ILiveSDK getInstance] initSdk:kSDKAPPID accountType:kAccountType];
    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

- (BOOL)isSupportScreenRecord
{
    NSString *version = [UIDevice currentDevice].systemVersion;
    if (version.doubleValue >= 9.0 && version.doubleValue < 11.0) {
        return YES;
    } else {
        return NO;
    }
}

//=======================

/*!
 @abstract      开启录制屏幕回调
 @param         result           错误码
 QAV_OK 调用成功。只有返回QAV_OK时，才会通过block异步返回执行结果；其他情况不会回调block。
 QAV_ERR_NOT_IN_MAIN_THREAD  startRecording非主线程调用。
 QAV_ERR_CONTEXT_NOT_START   context未start。
 QAV_ERR_ROOM_NOT_EXIST      房间未创建或已销毁。
 AV_ERR_RESOURCE_IS_OCCUPIED 房间内已有人进行屏幕分享或者播放视频文件
 QAV_ERR_FAIL 其他错误
 @param         errorInfo       错误信息
 */
- (void)screenRecordDidStart:(QAVResult)result withErrorInfo:(NSString *)errorInfo
{
    self.isScreenRecording = result == QAV_OK;
    
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil message:self.isScreenRecording ? @"开始屏幕录制成功" : [NSString stringWithFormat:@"开始屏幕录制失败:%@", errorInfo] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
    [alert show];
    
    if (!self.isScreenRecording)
    {
        [self stopScreenRecord];
    }
    
}


/*!
 @abstract      停止录制屏幕回调
 @param         result           错误码
 QAV_OK 调用成功。只有返回QAV_OK时，才会通过block异步返回执行结果；其他情况不会回调block。
 QAV_ERR_NOT_IN_MAIN_THREAD  stopRecording非主线程调用。
 QAV_ERR_CONTEXT_NOT_START   context未start。
 QAV_ERR_ROOM_NOT_EXIST      房间未创建或已销毁。
 QAV_ERR_FAIL 其他错误
 @param         errorInfo       错误信息
 */
- (void)screenRecordDidStop:(QAVResult)result withErrorInfo:(NSString *)errorInfo
{
    self.isScreenRecording = NO;
    
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:nil message:self.isScreenRecording ? @"停止屏幕录成成功" : [NSString stringWithFormat:@"停止屏幕录制失败:%@", errorInfo] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
    [alert show];
    [self stopScreenRecord];
    
}

- (void)startScreenRecord:(QAVRecordPreset)mode
{
    if (self.isScreenRecording)
    {
        return;
    }
    
    self.screenRecord = [QAVScreenRecord shareInstance];
    self.screenRecord.context = [[ILiveSDK getInstance] getAVContext];
    self.screenRecord.identifier = [[ILiveLoginManager getInstance] getLoginId];
    self.screenRecord.appid = kRecordAppId;
    self.screenRecord.mode = mode;
    
    //    UIDeviceOrientationPortrait ：0
    //    UIDeviceOrientationLandscapeRight：1
    //    UIDeviceOrientationPortraitUpsideDown：2
    //    UIDeviceOrientationLandscapeLeft：3
    [self.screenRecord setRotation:0];
    [self.screenRecord setDelegate:self];
    QAVResult res = [self.screenRecord startRecord];
    
    self.isScreenRecording = res == QAV_OK;
}
- (void)stopScreenRecord
{
    [[QAVScreenRecord shareInstance] setDelegate:nil];//释放回调
    [[QAVScreenRecord shareInstance] stopRecord];//结束录屏
    [[QAVScreenRecord shareInstance] destroy]; //资源回收
    self.isScreenRecording = NO;
}

@end
