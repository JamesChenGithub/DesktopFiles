//
//  LoginViewController.m
//  QAVSDKDemo
//
//  Created by dackli on 16/4/13.
//  Copyright © 2016年 qqconnect. All rights reserved.
//

#define STREAM_NAME "stream-default"

#include "xcast.h"
#include "./include/xcast_data.h"
#include "./include/xcast_variant.h"
#include "libyuv.h"
#include "xcast.hh"

#import "LoginViewController.h"
#import "app_delegate.h"
#import "AccompanyViewController.h"

using namespace xCast;

static void    *g_user_data;
static int32_t on_xcast_event(void *user_data, xcast_variant_t *data);
static int32_t on_channel_event(void *user_data, xcast_variant_t *data);
static int32_t on_stream_event(void *user_data, xcast_variant_t *data);
static int32_t on_camera_event(void *user_data, xcast_variant_t *data);
static int32_t on_mic_event(void *user_data, xcast_variant_t *data);
static int32_t on_speaker_event(void *user_data, xcast_variant_t *data);
static int32_t on_screen_event(void *user_data, xcast_variant_t *data);
static int32_t on_device_event(void *user_data, xcast_variant_t *data);
static int32_t on_tips_event(void *user_data, xcast_variant_t *data);
static int32_t on_accompany_event(void *user_data, xcast_variant_t *e);

static id<MAVRender> renderDelegate;
static id<UIDelegate> UIDelegate;

xcast_data_t
xcast_get_start_param(void *user_data)
{
  xcast_data_t settings;
  settings["app_id"] = [UIDelegate appid];
  settings["identifier"] = [UIDelegate identifier];
  return settings;
}

void
ui_xcast_err(int32_t err, const char *err_msg, void* user_data)
{
  
}

xcast_data_t
xcast_get_channel_param(void *user_data)
{
  xcast_data_t       params;

  params["relation_id"] = [UIDelegate roomid];
  
  //only for 1400046799
  if ([UIDelegate appid] == 1400046799) {
      params["videomaxbps"] = 3000;
  }
  
  params["role"] = [UIDelegate role];
  params["auto_recv"] = true;
  
  return params;
}

int32_t
ui_init_xcast(bool start, void* user_data)
{
  int32_t     rt = XCAST_OK;
  
  if (start) {
    /* 准备启动参数 */
    xcast_data_t settings = xcast_get_start_param(user_data);
    
    /* 启动XCAST */
    rt = xcast_startup(settings);
    if (rt == XCAST_OK) {
      g_user_data = user_data;
    }
    
    /* 注册事件通知回调 */
    xcast_handle_event(XC_EVENT_SYSTEM, (xcast_func_pt)on_xcast_event, user_data);
    xcast_handle_event(XC_EVENT_CHANNEL, (xcast_func_pt)on_channel_event, user_data);
    xcast_handle_event(XC_EVENT_STREAM, (xcast_func_pt)on_stream_event, user_data);
    xcast_handle_event(XC_EVENT_DEVICE, (xcast_func_pt)on_device_event, user_data);
    xcast_handle_event(XC_EVENT_DEVICE_PREPROCESS, (xcast_func_pt)on_device_event, user_data);
    xcast_handle_event(XC_EVENT_STATISTIC_TIPS, (xcast_func_pt)on_tips_event, user_data);
  } else {
    /* 停止XCAST */
    user_data = NULL;
    xcast_shutdown();
  }
  
  return rt;
}


void
ui_start_stream(bool start, void* user_data)
{
  if (start) {
    xcast_data_t params = xcast_get_channel_param(user_data);
    int32_t rt = xcast_start_channel(STREAM_NAME, params);
    if (XCAST_OK != rt) {
      ui_xcast_err(rt, xcast_err_msg(), user_data);
    }
  } else {
    xcast_close_channel(STREAM_NAME);
  }
}

/* xcast系统事件通知 */
int32_t
on_xcast_event(void *user_data, xcast_variant_t *evt)
{
  const char             *err_msg;
  int32_t                 err;
  
  err = xcast_vdict_get_int32(evt, "err", 0);
  err_msg = xcast_vdict_get_str(evt, "err-msg", NULL);
  ui_xcast_err(err, err_msg, user_data);
  
  return XCAST_OK;
}

/* 流状态通知： 更新UI中媒体流状态 */
static int32_t
on_channel_event(void *user_data, xcast_variant_t *e)
{
  xcast_data_t            evt(e);
  
  switch ((int32_t)evt["type"]) {
    case xc_channel_added:
      break;
    case xc_channel_updated:
      if (evt["state"] == xc_channel_connected) {
        break;
      }
      break;
    case xc_channel_removed:
          
      [UIDelegate onDisconnect];
      break;
    default:
      break;
  }
  
  return XCAST_OK;
}

/* 流轨道状态通知： 更新UI中媒体流轨道状态 */
static int32_t
on_stream_event(void *user_data, xcast_variant_t *v)
{
  const char             *src,*state,*stream;
  int32_t                type,clazz,direction,format,size;
  uint64_t               uin;
  int32_t                err;
  const char             *err_msg;
  
  xcast_data_t evt(v);
  src = evt["src"];
  type = evt["type"];
  clazz = evt["class"];
  direction = evt["direction"];
  state = evt["state"];
  uin = evt["uin"];
  err = evt["err"];
  err_msg = evt["err-msg"];
  stream = evt["channel"];
  format = evt["format"];
  size = evt["size"];
  NSString *srcTye = [NSString stringWithUTF8String:src];
  if (type == xc_stream_added) {
    /* 新增轨道 */
    if(![srcTye containsString:@"video-out"]){
      xcast_data_t path,param;
      path.format(XC_STREAM_ENABLED, stream, src);
      param["enabled"] = true;
      xcast_set_property(path.str_val(), param);
    }
    [UIDelegate outTrackAdd:srcTye];
  } else if (type == xc_stream_removed) {
    if ([[NSString stringWithUTF8String:src] hasPrefix:@"video-in"]) {
      NSString* renderKey = [NSString stringWithFormat:@"%lld%d",uin,0];
      [renderDelegate onUserVideoStateChange:renderKey :NO :NO :NO];
    }
    if ([[NSString stringWithUTF8String:src] hasPrefix:@"sub-video-in"]) {
      NSString* renderKey = [NSString stringWithFormat:@"%lld%d",uin,1];
      [renderDelegate onUserVideoStateChange:renderKey :NO :NO :NO];
    }
    /* 移除轨道 */
  } else if (type == xc_stream_updated) {
    /* 更新轨道 */
  } else if (type == xc_stream_media) {
    /* 轨道媒体数据通知 */
    NSString* renderKey = @"";
    if ([[NSString stringWithUTF8String:src] hasPrefix:@"video-in"]) {
      renderKey = [NSString stringWithFormat:@"%lld%d",uin,0];
    }else if ([[NSString stringWithUTF8String:src] hasPrefix:@"sub-video-in"]){
      renderKey = [NSString stringWithFormat:@"%lld%d",uin,1];
    }
    xc_media_format format = (xc_media_format)xcast_vdict_get_uint32(evt,
                                                                     "format",
                                                                     xc_media_i420);
    if ((format == xc_media_i420 || format == xc_media_argb32) && renderKey.length) {
      [renderDelegate onUserVideoStateChange:renderKey :YES :NO :NO];
      
      uint32_t width = xcast_vdict_get_uint32(evt, "width", 0);
      uint32_t height = xcast_vdict_get_uint32(evt, "height", 0);
      uint32_t rotate = xcast_vdict_get_uint32(evt, "rotate", 0);
      //I420->RGB
      unsigned char* rgbBuf = (unsigned char *)malloc(width * height * 3);
      
      unsigned char* I420buf = (unsigned char*)xcast_vdict_get_buf(evt, "data", NULL);
      
//      NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//      NSString *documentsDirectory = [paths objectAtIndex:0];
//      static int count = 0;
//      count++;
//      if (count < 100) {
//        [[NSFileManager defaultManager] createFileAtPath:[NSString stringWithFormat:@"%@/%d.yuv",documentsDirectory,count] contents:[NSData dataWithBytes:I420buf length:size] attributes:nil];
//      }
      
      libyuv::I420ToRAW(I420buf, width, I420buf + width * height, width >> 1, I420buf + width * height * 5 / 4, width >> 1, rgbBuf, width * 3, width, height);
      
      bool isMainView = NO;
      //dispatch data
      tagMAVVideoData videoData;
      videoData.rgb24buf = rgbBuf;
      videoData.ullUin = [renderKey cStringUsingEncoding:NSUTF8StringEncoding];
      videoData.bMainData = isMainView;
      videoData.nWidth = width;
      videoData.nHeight = height;
      videoData.nRotate = rotate;
      [renderDelegate dispatchVideoData:&videoData];
    }
  }
  return XCAST_OK;
}

static int32_t
on_device_event(void *user_data, xcast_variant_t *e)
{
  xcast_data_t        evt(e);
  int32_t    clazz = (int32_t)evt["class"];
  switch (clazz) {
    case xc_device_camera:
      on_camera_event(user_data, e);
      break;
    case xc_device_screen_capture:
      on_screen_event(user_data, e);
      break;
    case xc_device_player:
      break;
    case xc_device_mic:
      on_mic_event(user_data, e);
      break;
    case xc_device_speaker:
      on_speaker_event(user_data, e);
      break;
    case xc_device_accompany:
      on_accompany_event(user_data, e);
      break;
    default:
      break;
  }
  return XCAST_OK;
}

static int32_t
on_tips_event(void *user_data, xcast_variant_t *e)
{
    xcast_data_t        evt(e);
    const char* tips = evt["tips"];
    [UIDelegate setTips:[NSString stringWithUTF8String:tips]];
    return XCAST_OK;
}

/* 摄像头事件通知： 更新UI中摄像头状态  */
static int32_t
on_camera_event(void *user_data, xcast_variant_t *v)
{
  const char             *src,*state,*stream;
  int32_t                type,clazz,direction,format;
  uint64_t               uin;
  int32_t                err;
  const char             *err_msg;
  
  xcast_data_t evt(v);
  src = evt["src"];
  type = evt["type"];
  clazz = evt["class"];
  direction = evt["direction"];
  state = evt["state"];
  uin = evt["uin"];
  err = evt["err"];
  err_msg = evt["err-msg"];
  stream = evt["stream"];
  format = evt["format"];
  
  if (type == xc_device_added) {
    [UIDelegate addCamera:[NSString stringWithCString:src encoding:NSUTF8StringEncoding]];
  } else if (type == xc_device_removed) {
    [UIDelegate removeCamera:[NSString stringWithUTF8String:src]];
  } else if (type == xc_device_updated) {

  } else if (type == xc_device_preprocess) {
    uint8_t    *data;
    uint32_t    size, width, height, rotate;
    
    data = (uint8_t *)evt["data"].ptr_val();
    size = evt["size"];
    width = evt["width"];
    height = evt["height"];
    rotate = evt["rotate"];
    
    if (format == xc_media_i420) {
      data += width * height;
      for (int32_t i = 0; i < height / 2; i++) {
        if (i % 2) memset(data + i * width / 2, 0xFF, width / 2);
      }
    }
  } else if (type == xc_device_preview) {
    
    
    NSString* renderKey = [NSString stringWithFormat:@"%d%d",0,1];
    [renderDelegate onUserVideoStateChange:renderKey :YES :YES :NO];
    
    uint32_t width = xcast_vdict_get_uint32(evt, "width", 0);
    uint32_t height = xcast_vdict_get_uint32(evt, "height", 0);
    uint32_t rotate = xcast_vdict_get_uint32(evt, "rotate", 0);
    //I420->RGB
    unsigned char* rgbBuf = (unsigned char *)malloc(width * height * 3);
    
    unsigned char* I420buf = (unsigned char*)xcast_vdict_get_buf(evt, "data", NULL);;
    
    libyuv::I420ToRAW(I420buf, width, I420buf + width * height, width >> 1, I420buf + width * height * 5 / 4, width >> 1, rgbBuf, width * 3, width, height);
    
    bool isMainView = YES;
    //dispatch data
    tagMAVVideoData videoData;
    videoData.rgb24buf = rgbBuf;
    videoData.ullUin = [renderKey cStringUsingEncoding:NSUTF8StringEncoding];
    videoData.bMainData = isMainView;
    videoData.nWidth = width;
    videoData.nHeight = height;
    videoData.nRotate = rotate;
    [renderDelegate dispatchVideoData:&videoData];
  }
  
  return XCAST_OK;
}

/* 麦克风事件通知 */
static int32_t
on_mic_event(void *user_data, xcast_variant_t *evt)
{
  const char             *src, *format;
  int32_t                type, state;
  const uint8_t          *data;
  uint32_t               size, width;
  int32_t                err;
  const char             *err_msg;
  
  src = xcast_vdict_get_str(evt, "src", NULL);              /* device id */
  type = xcast_vdict_get_int32(evt, "type", 0);            /* event type string */
  state = xcast_vdict_get_int32(evt, "state", NULL);          /* state message */
  err = xcast_vdict_get_int32(evt, "err", 0);            /* error code */
  err_msg = xcast_vdict_get_str(evt, "err-msg", NULL);      /* error message */
  format = xcast_vdict_get_str(evt, "format", NULL);
  data = xcast_vdict_get_buf(evt, "data", NULL);
  size = xcast_vdict_get_uint32(evt, "size", 0);
  width = xcast_vdict_get_uint32(evt, "width", 0);
  
  if (type == xc_device_added) {
    [UIDelegate addMic:[NSString stringWithUTF8String:src]];
  } else if (type == xc_device_removed) {
    [UIDelegate removeMic:[NSString stringWithUTF8String:src]];
  } else if (type == xc_device_updated) {
    [UIDelegate updateMic:[NSString stringWithUTF8String:src] state:state];
  } else if (type == xc_device_preprocess) {

  }
  
  return XCAST_OK;
}

/* 扬声器事件通知 */
static int32_t
on_speaker_event(void *user_data, xcast_variant_t *evt)
{
  const char             *src;
  int32_t                type, state;
  int32_t                 err;
  const char             *err_msg;
  
  src = xcast_vdict_get_str(evt, "src", NULL);              /* device id */
  type = xcast_vdict_get_int32(evt, "type", 0);            /* event type string */
  state = xcast_vdict_get_int32(evt, "state", NULL);          /* state message */
  err = xcast_vdict_get_int32(evt, "err", 0);            /* error code */
  err_msg = xcast_vdict_get_str(evt, "err-msg", NULL);      /* error message */
  
  if (type == xc_device_added) {
    [UIDelegate addSpeaker:[NSString stringWithUTF8String:src]];
  } else if (type == xc_device_removed) {
    [UIDelegate removeSpeaker:[NSString stringWithUTF8String:src]];
  } else if (type == xc_device_updated) {
    [UIDelegate updateSpesker:[NSString stringWithUTF8String:src] state:state];
  }
  
  return XCAST_OK;
}

static int32_t
on_accompany_event(void *user_data, xcast_variant_t *e)
{
  xcast_data_t evt(e);
  int32_t type = evt["type"];
  uint32_t duration = evt["duration"];

  if (type == xc_device_updated) {
    NSDictionary *dict = [NSDictionary dictionaryWithObject:[NSNumber numberWithUnsignedInt:duration]
                                                     forKey:@"duration"];
    [[NSNotificationCenter defaultCenter] postNotificationName:AccompanyFileDuration
                                                        object:nil
                                                      userInfo:dict];
  }

  return XCAST_OK;
}

/* 屏幕捕获事件通知 */
static int32_t
on_screen_event(void *user_data, xcast_variant_t *data)
{
  
  return XCAST_OK;
}


static int32_t
stop_stream()
{
  ui_start_stream(false, NULL);
  return XCAST_OK;
}


@interface LoginViewController ()
{
  BOOL     isStreamRunning;
  BOOL     isPreviewing;
  BOOL     isXcastStarted;
  BOOL     isMicPreView;
  BOOL     isCamPreprocess;
  BOOL     isSpeakerUpdate;
}

@property (weak) IBOutlet NSTextField *appidTextField;
@property (weak) IBOutlet NSTextField *roomidTextField;
@property (weak) IBOutlet NSTextField *identifierTextField;
@property (weak) IBOutlet NSTextField *roleTextField;
@property (weak) IBOutlet NSButton *loginButton;
@property (weak) IBOutlet NSComboBox *cameraBox;
@property (weak) IBOutlet NSComboBox *speakerBox;
@property (weak) IBOutlet NSComboBox *micBox;
@property (weak) IBOutlet NSButton *subVideoBtn;
@property (weak) IBOutlet NSButton *preMicBtn;
@property (strong) IBOutlet NSTextView *infoTextField;
@property (weak) IBOutlet NSButton *roomButton;
@property (weak) IBOutlet NSButton *accompanyButton;
@end

@implementation LoginViewController

//
//@protocol NSComboBoxDelegate <NSTextFieldDelegate>
//@optional

/* Notifications */
- (void)comboBoxWillPopUp:(NSNotification *)notification
{
  return;
}
- (void)comboBoxWillDismiss:(NSNotification *)notification
{
  return;
}
- (void)comboBoxSelectionDidChange:(NSNotification *)notification
{
  NSComboBox* box = notification.object;
  NSString* selectedItem = [box objectValueOfSelectedItem];
  if(!selectedItem){
    return;
  }
  if (box == _cameraBox) {
    if ([selectedItem isEqualToString:@"Disable Camera"]) {
      [self handleCameraPreview:NO];
      if (isStreamRunning) {
        [self handleTrack:@"video-out" bOpen:NO];
      }
    }else if(selectedItem && selectedItem.length > 0){
      [self handleCameraPreview:NO];
    
      xcast_data_t param([selectedItem cStringUsingEncoding:NSUTF8StringEncoding]);
      xcast_set_property(XC_CAMERA_DEFAULT, param);

      [self handleCameraPreview:YES];
      [self resetVideoTrackCapture];
      [self handleTrack:@"video-out" bOpen:YES];
    }
  }
  if (box == _speakerBox) {
    if ([selectedItem isEqualToString:@"Disable Speaker"]) {
      [self handleSpeakerEnable:NO];
    }else{
        if(isSpeakerUpdate){
            isSpeakerUpdate = NO;
            return;
        }
        xcast_data_t speaker_name(xcast_get_property(XC_SPEAKER_DEFAULT));
        xcast_data_t param([selectedItem cStringUsingEncoding:NSUTF8StringEncoding]);
        xcast_set_property(XC_SPEAKER_DEFAULT, param);
        [self handleSpeakerEnable:YES];
    }
  }
  if (box == _micBox) {
    if ([selectedItem isEqualToString:@"Disable Mic"]) {
        [self resetAudioTrackCapture];
        xcast_set_property(XC_MIC_ENABLED, xcast_data_t(false));
        if(isMicPreView){
            xcast_data_t mic_name(xcast_get_property(XC_MIC_DEFAULT));
            if (mic_name.str_val()) {
                xcast_data_t path;
                path.format(XC_MIC_PREVIEW, mic_name.str_val());
                xcast_data_t param(false);
                xcast_set_property(path.str_val(), param);
            }
        }
    }else{
      xcast_data_t param([selectedItem cStringUsingEncoding:NSUTF8StringEncoding]);
      xcast_set_property(XC_MIC_DEFAULT, param);
      [self resetAudioTrackCapture];
      xcast_set_property(XC_MIC_ENABLED, xcast_data_t(true));
      if(isMicPreView){
        xcast_data_t mic_name(xcast_get_property(XC_MIC_DEFAULT));
        if (mic_name.str_val()) {
            xcast_data_t path;
            path.format(XC_MIC_PREVIEW, mic_name.str_val());
            xcast_data_t param(true);
            xcast_set_property(path.str_val(), param);
        }
      }
    }
  }
  
  return;
}
- (void)comboBoxSelectionIsChanging:(NSNotification *)notification
{
  return;
}

//@end

- (void)handleCameraPreview:(BOOL)bOpen
{
  xcast_data_t camera_name(xcast_get_property(XC_CAMERA_DEFAULT));
  xcast_data_t path;
  path.format(XC_CAMERA_PREVIEW,camera_name.str_val());
  bool open = bOpen;
  xcast_data_t param(open);
  xcast_set_property(path, param);
  isPreviewing = bOpen;
}

- (void)handleTrack:(NSString*)track bOpen:(BOOL)bOpen
{
  xcast_data_t path,param;
  path.format(XC_STREAM_ENABLED, STREAM_NAME, [track UTF8String]);
  bool open = bOpen;
  param["enabled"] = open;
  xcast_set_property(path.str_val(), param);
}

- (void)outTrackAdd:(NSString *)name{
    bool isCameraOpen = ![[_cameraBox objectValueOfSelectedItem] isEqualToString:@"Disable Camera"];
    if([name isEqualToString:@"video-out"] && isCameraOpen){
        [self handleTrack:name bOpen:YES];
    }
    
}

- (void)resetVideoTrackCapture
{
  xcast_data_t path;
  path.format(XC_STREAM_CAPTURE, STREAM_NAME,"video-out");
  xcast_data_t name(xcast_get_property(XC_CAMERA_DEFAULT));
  xcast_set_property(path.str_val(), name);
}

- (void)resetAudioTrackCapture
{
  xcast_data_t path;
  path.format(XC_STREAM_CAPTURE, STREAM_NAME,"audio-out");
  xcast_data_t name(xcast_get_property(XC_MIC_DEFAULT));
  xcast_set_property(path.str_val(), name);
}


- (void)handleMicPreViewEnable:(BOOL)bEnable
{
  xcast_data_t mic_name(xcast_get_property(XC_MIC_DEFAULT));
  if (mic_name.str_val()) {
    xcast_data_t path;
    path.format(XC_MIC_PREVIEW, mic_name.str_val());
    bool enable = bEnable;
    xcast_data_t param(enable);
    xcast_set_property(path.str_val(), param);
  }
}

- (void)handleSpeakerEnable:(BOOL)bEnable
{
  if(isStreamRunning){
    bool enable = bEnable;
    xcast_data_t param(enable);
    xcast_set_property(XC_SPEAKER_ENABLED, param);
  }
  else{
      xcast_data_t speaker_name(xcast_get_property(XC_SPEAKER_DEFAULT));
      if (speaker_name.str_val()) {
        xcast_data_t path;
        path.format(XC_SPEAKER_PREVIEW, speaker_name.str_val());
        bool enable = bEnable;
        xcast_data_t param(enable);
        xcast_set_property(path.str_val(), param);
      }
  }
}

- (void)viewDidLoad {
  [super viewDidLoad];

  [_loginButton setKeyEquivalent:@"\r"];
    
    // 设置背景色
  [self.view setWantsLayer:YES];
  self.view.layer.backgroundColor = [[NSColor colorWithRed:241/256.0 green:241/256.0 blue:241/256.0 alpha:1] CGColor];
  
  [self initVideoUI];
  
  [self initTextFields];
  
  [self initComboBox];
    
  UIDelegate = self;
}

- (int32_t)appid
{
  int32_t appid = 0;
  if (_appidTextField.stringValue && _appidTextField.stringValue.length) {
    appid = [_appidTextField.stringValue intValue];
  }
  return appid;
}

- (int32_t)roomid
{
  int32_t roomid = 0;
  if (_roomidTextField.stringValue && _roomidTextField.stringValue.length) {
    roomid = [_roomidTextField.stringValue intValue];
  }
  return roomid;
}

- (int32_t)identifier
{
  int32_t identifier = 0;
  if (_identifierTextField.stringValue && _identifierTextField.stringValue.length) {
    identifier = [_identifierTextField.stringValue intValue];
  }
  return identifier;
}

- (const char*)role
{
  const char* role = NULL;
  if (_roleTextField.stringValue && _roleTextField.stringValue.length) {
    role =  [_roleTextField.stringValue UTF8String];
  }
  return role;
}

#pragma mark - 初始化 -
-(void)initTextFields
{
  _appidTextField.placeholderString = @"AppID";
  _appidTextField.stringValue = @"1400036169";
  
  _roomidTextField.placeholderString = @"RoomID";
  _roomidTextField.stringValue = @"200099";
  
  _identifierTextField.placeholderString = @"Identifier";
  _identifierTextField.stringValue = @"2019";
  
  _roleTextField.placeholderString = @"Role";
  _roleTextField.stringValue =@"user";
    
  isXcastStarted = NO;
  isMicPreView = NO;
  isSpeakerUpdate = NO;
}

-(void)initVideoUI{
    if(!_renderImpl){
      _renderImpl     = [[MAVRenderImpl alloc] init];
      renderDelegate = _renderImpl;
      
      [self.view addSubview:_renderImpl.view positioned:NSWindowBelow relativeTo:nil];
    }
}

-(void)initComboBox{
  [_cameraBox setDelegate:self];
  [_cameraBox removeAllItems];
  [_cameraBox setEditable:NO];
  [_cameraBox addItemWithObjectValue:@"Disable Camera"];
  
  [_micBox setDelegate:self];
  [_micBox removeAllItems];
  [_micBox setEditable:NO];
  [_micBox addItemWithObjectValue:@"Disable Mic"];
  
  [_speakerBox setDelegate:self];
  [_speakerBox removeAllItems];
  [_speakerBox setEditable:NO];
  [_speakerBox addItemWithObjectValue:@"Disable Speaker"];
  
    _infoTextField.superview.superview.hidden = YES;
    _infoTextField.backgroundColor = NSColor.clearColor;
}

- (void)addCamera:(NSString*)name
{
  [_cameraBox addItemWithObjectValue:name];
  xcast_data_t camera_name(xcast_get_property(XC_CAMERA_DEFAULT));
  if ([name isEqualToString:[NSString stringWithUTF8String:camera_name.str_val()]]) {
    [_cameraBox selectItemWithObjectValue:[NSString stringWithUTF8String:camera_name.str_val()]];
  }
}
- (void)removeCamera:(NSString*)name
{
  xcast_data_t camera_name(xcast_get_property(XC_CAMERA_DEFAULT));
  if (camera_name.str_val()) {
    [_cameraBox selectItemWithObjectValue:[NSString stringWithUTF8String:camera_name.str_val()]];
  }
  [_cameraBox removeItemWithObjectValue:name];
}
- (void)addSpeaker:(NSString*)name
{
  [_speakerBox addItemWithObjectValue:name];
  xcast_data_t speaker_name(xcast_get_property(XC_SPEAKER_DEFAULT));
  if ([name isEqualToString:[NSString stringWithUTF8String:speaker_name.str_val()]]) {
    [_speakerBox selectItemWithObjectValue:[NSString stringWithUTF8String:speaker_name.str_val()]];
  }
}
- (void)removeSpeaker:(NSString*)name
{
  [_speakerBox removeItemWithObjectValue:name];
  xcast_data_t speaker_name(xcast_get_property(XC_SPEAKER_DEFAULT));
  if (speaker_name.str_val()) {
    [_speakerBox selectItemWithObjectValue:[NSString stringWithUTF8String:speaker_name.str_val()]];
  }
}
- (void)addMic:(NSString*)name
{
  [_micBox addItemWithObjectValue:name];
  xcast_data_t mic_name(xcast_get_property(XC_MIC_DEFAULT));
  if ([name isEqualToString:[NSString stringWithUTF8String:mic_name.str_val()]]) {
    [_micBox selectItemWithObjectValue:[NSString stringWithUTF8String:mic_name.str_val()]];
  }
}
- (void)removeMic:(NSString*)name
{
  [_micBox removeItemWithObjectValue:name];
  xcast_data_t mic_name(xcast_get_property(XC_MIC_DEFAULT));
  if (mic_name.str_val()) {
    [_micBox selectItemWithObjectValue:[NSString stringWithUTF8String:mic_name.str_val()]];
  }
}

- (void)updateSpesker:(NSString*)name state:(int)state{
    if(state == xc_device_running){
        if(![[_speakerBox objectValueOfSelectedItem] isEqualToString:name]){
            isSpeakerUpdate = YES;
        }
        [_speakerBox selectItemAtIndex:[_speakerBox indexOfItemWithObjectValue:name]];
    }
}

- (void)updateMic:(NSString*)name state:(int)state{
    if(state == xc_device_running){
        [_micBox selectItemAtIndex:[_micBox indexOfItemWithObjectValue:name]];
    }
}


- (void)onDisconnect
{
    [_roomButton setTitle:@"进房"];
    isStreamRunning = NO;
}

- (void)setTips:(NSString*)tips
{
    [_infoTextField  setString:tips];
}

-(void)unInitVideoUI{
  [_renderImpl.view removeFromSuperview];
  _renderImpl = nil;
  renderDelegate = nil;
  [_cameraBox removeAllItems];
  [_micBox removeAllItems];
  [_speakerBox removeAllItems];
}

- (void)dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)initUI
{
    if (_isViewLoad) {
        return;
    }
  _isViewLoad = YES;
  [_loginButton setKeyEquivalent:@"\r"];
}

- (IBAction)loginAction:(id)sender {
  NSButton *btn = sender;
  if(!isXcastStarted){
      
      ui_init_xcast(true, NULL);
      
      [self initVideoUI];
      [self initComboBox];
      isXcastStarted = NO;
      isMicPreView = NO;
      isSpeakerUpdate = NO;
      isXcastStarted = !isXcastStarted;
      
      btn.title = @"停止";
  }else{
      [self handleCameraPreview:NO];
      ui_init_xcast(false, NULL);
      [self unInitVideoUI];
      isXcastStarted = !isXcastStarted;
      btn.title = @"启动";
      isStreamRunning = NO;
      isMicPreView = NO;
      _preMicBtn.title = @"允许回放";
      _roomButton.title = @"进房";
  }
}

- (IBAction)logoutAction:(id)sender {
    NSButton *btn = sender;
    if(isStreamRunning){
        int32_t rt;
        rt = xcast_close_channel(STREAM_NAME);
        if(rt == 0){
            isStreamRunning = !isStreamRunning;
            btn.title = @"进房";
        }
    }else{
        xcast_data_t params = xcast_get_channel_param(NULL);
        int32_t rt;
        rt = xcast_start_channel(STREAM_NAME,params);
        if (XCAST_OK == rt || XCAST_ERR_PENDING == rt) {
            isStreamRunning = !isStreamRunning;
            btn.title = @"退房";
        } else {
        }
    }
}
- (IBAction)subVideoAction:(id)sender {
  
  xcast_data_t path,ret;
  path.format(XC_STREAM_STATE,STREAM_NAME,"sub-video-out");
  ret = xcast_get_property(path);
  
  xc_stream_state state = (xc_stream_state)ret.int32_val();
  
  BOOL bOpen;
  if (state == xc_stream_running) {
    bOpen = NO;
  }else{
    bOpen = YES;
  }
  [self handleTrack:@"sub-video-out" bOpen:bOpen];
}

- (IBAction)tipsAction:(id)sender {
    _infoTextField.superview.superview.hidden = !_infoTextField.superview.superview.hidden;
}

- (IBAction)micPreViewAction:(id)sender {
    NSButton *btn = sender;
    isMicPreView = !isMicPreView;
    [self handleMicPreViewEnable:isMicPreView];
    if(!isMicPreView){
        [btn setTitle:@"允许回放"];
    }
    else{
        [btn setTitle:@"禁止回放"];
    }
}

- (IBAction)preprocessAction:(id)sender {
  isCamPreprocess = !isCamPreprocess;
  xcast::set_property(XC_CAMERA_PREPROCESS, isCamPreprocess ? true : false);
}


- (IBAction)accompanyAction:(id)sender {
  if (isStreamRunning) {
    AccompanyViewController *accompanyViewController = [[AccompanyViewController alloc] init];
    [self presentViewControllerAsSheet:accompanyViewController];
  } else {
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"确定"];
    alert.messageText = @"操作错误";
    alert.informativeText = @"进房后才能操作伴奏";
    [alert setAlertStyle:NSAlertStyleWarning];
    [alert beginSheetModalForWindow:[self.view window] completionHandler:nil];
  }
}

@end
