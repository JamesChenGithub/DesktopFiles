//
//  AppDelegate.h
//  SreenRecord
//
//  Created by AlexiChen on 2018/5/29.
//  Copyright © 2018年 AlexiChen. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QAVScreenRecordKit/QAVScreenRecord.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (nonatomic, assign) BOOL isScreenRecording;

- (BOOL)isSupportScreenRecord;
- (void)startScreenRecord:(QAVRecordPreset)mode;
- (void)stopScreenRecord;



@end

