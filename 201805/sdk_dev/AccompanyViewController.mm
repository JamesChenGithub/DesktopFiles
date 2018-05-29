/**
 *  Copyright (c) 2018 Tencent Inc. All rights reserved.
 *  The xcast project authors.
 */

#import "AccompanyViewController.h"
#include "xcast.h"
#include "./include/xcast_data.h"
#include "./include/xcast_variant.h"

static NSString *accompanyFile = nil;

@interface AccompanyViewController ()
{
  BOOL accompanyStarted;
  BOOL accompanyPaused;
  uint32_t accompanyDuration;
  NSTimer *progressTimer;
}

@property NSString *loopCount;

@property (weak) IBOutlet NSTextField *filePathTextField;
@property (weak) IBOutlet NSTextField *loopCountTextField;
@property (weak) IBOutlet NSButton    *startButton;
@property (weak) IBOutlet NSButton    *pauseButton;
@property (weak) IBOutlet NSButton    *localCheckboxButton;
@property (weak) IBOutlet NSButton    *remoteCheckboxButton;
@property (weak) IBOutlet NSSlider    *volumeSlider;
@property (weak) IBOutlet NSSlider    *progressSlider;
@property (weak) IBOutlet NSTextField *accompanyDurationLabel;
@end

@implementation AccompanyViewController

- (void)dealloc {
  [[NSNotificationCenter defaultCenter] removeObserver: self
                                                  name: AccompanyFileDuration
                                                object: nil];
}

- (void)viewDidLoad {
  [super viewDidLoad];

  accompanyStarted = NO;
  accompanyPaused = NO;
  accompanyDuration = UINT_MAX;
  progressTimer = nil;

  [_loopCountTextField setStringValue:@"-1"];
  self.loopCount = _loopCountTextField.stringValue;

  if (accompanyFile && accompanyFile.length > 0)
    [_filePathTextField setStringValue:accompanyFile];

  [[NSNotificationCenter defaultCenter] addObserver: self
                                           selector: @selector(updateDuration:)
                                               name: AccompanyFileDuration
                                             object: nil];
}

- (void)viewWillDisappear {
  [self destroyTimer];
}

- (void)createTimer {
  progressTimer = [NSTimer scheduledTimerWithTimeInterval:1.0
                                                   target:self
                                                 selector:@selector(refreshProgress)
                                                 userInfo:nil
                                                  repeats:YES];
}

- (void)destroyTimer {
  if (progressTimer.isValid) {
    [progressTimer invalidate];
    progressTimer = nil;
  }
}

- (IBAction)quitAction:(id)sender {

  if (accompanyStarted) {
    xcast_data_t params;
    params["enabled"] = false;
    xcast_set_property(XC_ACCOMPANY_ENABLED, params);
  }
  [self dismissController:nil];
}

- (IBAction)selectFileAction:(id)sender {
  if (accompanyStarted) {
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"确定"];
    alert.messageText = @"操作错误";
    alert.informativeText = @"请先停止当前正在播放的伴奏再选择文件";
    [alert setAlertStyle:NSAlertStyleWarning];
    [alert beginSheetModalForWindow:[self.view window] completionHandler:nil];
    return;
  }

  NSOpenPanel *openPanel = [NSOpenPanel openPanel];
  [openPanel setAllowsMultipleSelection:NO];
  [openPanel setCanChooseDirectories:NO];
  [openPanel setMessage:@"选择伴奏文件"];
  [openPanel beginSheetModalForWindow:[self.view window] completionHandler:^(NSInteger result) {
    if (result == NSFileHandlingPanelOKButton)
    {
      NSString *filepath = [[openPanel URL] path];
      [_filePathTextField setStringValue:filepath];
      accompanyFile = [NSString stringWithString:filepath];
    }
  }];
}

- (IBAction)startAccompanyAction:(id)sender {
  xcast_data_t params;

  if (accompanyStarted) {
    [self destroyTimer];

    accompanyStarted = NO;
    accompanyPaused = NO;
    _loopCountTextField.editable = YES;
    _startButton.title = @"开始";
    _pauseButton.title = @"暂停";

    params["enabled"] = false;
    xcast_set_property(XC_ACCOMPANY_ENABLED, params);
  } else {
    if (accompanyFile == nil || accompanyFile.length == 0) {
      NSAlert *alert = [[NSAlert alloc] init];
      [alert addButtonWithTitle:@"确定"];
      alert.messageText = @"操作错误";
      alert.informativeText = @"请先选择伴奏文件";
      [alert setAlertStyle:NSAlertStyleWarning];
      [alert beginSheetModalForWindow:[self.view window] completionHandler:nil];
      return;
    }

    accompanyStarted = YES;
    _loopCountTextField.editable = NO;
    _startButton.title = @"停止";
    _progressSlider.intValue = 0;

    params["enabled"] = true;
    params["path"] = [accompanyFile UTF8String];
    params["loopback"] = !!(_remoteCheckboxButton.state);
    params["loopcount"] = [self.loopCount intValue];
    xcast_set_property(XC_ACCOMPANY_ENABLED, params);

    xcast_set_property("device.accompany.enable_local", xcast_data_t(_localCheckboxButton.state == NSControlStateValueOn));
    xcast_set_property("device.accompany.enable_remote", xcast_data_t(_remoteCheckboxButton.state == NSControlStateValueOn));
    xcast_set_property(XC_ACCOMPANY_VOLUME, xcast_data_t(_volumeSlider.doubleValue));
  }
}

- (IBAction)pauseAccompanyAction:(id)sender {
  if (!accompanyStarted) {
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"确定"];
    alert.messageText = @"操作错误";
    alert.informativeText = @"启动伴奏后才能暂停";
    [alert setAlertStyle:NSAlertStyleWarning];
    [alert beginSheetModalForWindow:[self.view window] completionHandler:nil];
    return;
  }

  if (accompanyPaused) {
    accompanyPaused = NO;
    _pauseButton.title = @"暂停";
    [progressTimer setFireDate:[NSDate distantPast]];
  } else {
    accompanyPaused = YES;
    _pauseButton.title = @"恢复";
    [progressTimer setFireDate:[NSDate distantFuture]];
  }

  xcast_data_t params(!!accompanyPaused);
  xcast_set_property("device.accompany.pause", params);
}

- (IBAction)volumeAction:(id)sender {
  if (accompanyStarted) {
    xcast_data_t params(_volumeSlider.doubleValue);
    xcast_set_property(XC_ACCOMPANY_VOLUME, params);
  }
}

- (IBAction)progressAction:(id)sender {
  if (accompanyStarted) {
    xcast_data_t params(_progressSlider.intValue);
    xcast_set_property("device.accompany.seek", params);
  }
}

- (IBAction)localCheckboxAction:(id)sender {
  if (accompanyStarted) {
    xcast_data_t params(!!((NSButton *)sender).state);
    xcast_set_property("device.accompany.enable_local", params);
  }
}

- (IBAction)remoteCheckboxAction:(id)sender {
  if (accompanyStarted) {
    xcast_data_t params(!!((NSButton *)sender).state);
    xcast_set_property("device.accompany.enable_remote", params);
  }
}

- (void)updateDuration:(NSNotification *)notification {
  uint32_t duration = [[[notification userInfo] objectForKey:@"duration"] unsignedIntValue];
  if (duration == 0) {
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"确定"];
    alert.messageText = @"内部错误";
    alert.informativeText = @"文件时间返回 0";
    [alert setAlertStyle:NSAlertStyleWarning];
    [alert beginSheetModalForWindow:[self.view window] completionHandler:nil];
    return;
  }

  accompanyDuration = duration;
  _progressSlider.maxValue = (double)accompanyDuration;

  NSDate *timeData = [NSDate dateWithTimeIntervalSince1970:(accompanyDuration / 1000.0)];
  NSDateFormatter *dateFormatter =[[NSDateFormatter alloc] init];
  [dateFormatter setDateFormat:@"mm:ss.SSS"];
  [_accompanyDurationLabel setStringValue:[dateFormatter stringFromDate:timeData]];
  [self createTimer];
}

- (void)refreshProgress {
  if (accompanyStarted) {
    xcast_variant_t *vp = xcast_get_property("device.accompany.position");
    xcast_data_t prop(vp);
    uint32_t position = prop.uint32_val() % accompanyDuration;
    xcast_variant_unref(vp);
    _progressSlider.doubleValue = (double)position;
  }
}

@end
