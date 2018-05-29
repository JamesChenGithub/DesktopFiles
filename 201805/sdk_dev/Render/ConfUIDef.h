//
//  ConfUIDef.h
//  QQ4Mac
//
//  Created by wolf on 15/9/26.
//  Copyright © 2015年 tencent. All rights reserved.
//

#ifndef ConfUIDef_h
#define ConfUIDef_h

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(a) if (a) [a release]; a = nil;
#endif

//--------------------------------------------------------------------------------
// UI Event ID Begin
enum {
    ConfUI_Evt_MainWin          = 1,
    ConfUI_Evt_Toolbar          = 100,
    ConfUI_Evt_UserList         = 200,
    ConfUI_Evt_VideoCtrl        = 300,
    ConfUI_Evt_MainView         = 400,
    
    ConfUI_Evt_QQEvt            = 500,  //这部分事件本不应该出现在这，后期重构逻辑所在层次
};

// MainWin UI Event
enum {
    ConfUI_Evt_MainWin_DefaultBtn = ConfUI_Evt_MainWin,  ///< 默认按钮(测试用)
    ConfUI_Evt_MainWin_SystemClose,                      ///< 系统关闭按钮
    ConfUI_Evt_MainWin_FullScreenMainWndChange,          ///< 全屏时获得/失去MainWnd属性
    ConfUI_Evt_MainWin_VideoAIOInfoChange,               ///< 视频AIO状态信息改变
};


// Toolbar UI Event
enum {
    ConfUI_Evt_Toolbar_ReturnAIO = ConfUI_Evt_Toolbar,  ///< 返回AIO按钮
    ConfUI_Evt_Toolbar_Mic,         ///< 麦克风按钮
    ConfUI_Evt_Toolbar_Camare,      ///< 摄像头按钮
    ConfUI_Evt_Toolbar_ScreenShare, ///< 屏幕分享按钮
    ConfUI_Evt_Toolbar_ScreenShareMode,   ///< 高清/流畅
    ConfUI_Evt_Toolbar_AddMember,   ///< 加人按钮
    ConfUI_Evt_Toolbar_Close,       ///< 挂断按钮
    ConfUI_Evt_Toolbar_Accept,      ///< 接受按钮
    ConfUI_Evt_Toolbar_HiddenSmall, ///< 隐藏小视频窗口按钮
    ConfUI_Evt_Toolbar_CloseScreenshare, ///< 屏幕分享结束按钮
};


// UI Type
enum {
    ConfUI_Type_None = 0,   ///< 隐藏窗口
    ConfUI_Type_Audio_Inviting,    ///< 呼叫中(音频)
    ConfUI_Type_Video_Inviting,     ///< 呼叫中(视频)
    ConfUI_Type_BeInviting,   ///< 被邀请中(被动方)
    ConfUI_Type_Audio_Connecting, ///< 连接服务器中(音频)
    ConfUI_Type_Video_Connecting, ///< 连接服务器中(视频)
    ConfUI_Type_Audio,      ///< 音频
    ConfUI_Type_Video,      ///< 视频
    ConfUI_Type_Max,        ///<
};

// toolbar Ctrl Type
enum {
    ConfUI_Toolbar_Ctrl_Mic = 0,    ///< 麦克风按钮
    ConfUI_Toolbar_Ctrl_Camera,     ///< 视频按钮
    ConfUI_Toolbar_Ctrl_ScreenShare,///< 屏幕分享按钮
    ConfUI_Toolbar_Ctrl_ScreenShareMode,  ///< 屏幕分享模式按钮
};

//--------------------------------------------------------------------------------

// UI Evt Info key
#define kConfUI_Evt_InfoKey_Common_OpenOrClose @"kConfUI_Evt_InfoKey_Common_OpenOrClose"     ///< 通用开/关表示 (比如麦克风 摄像头)
#define kConfUI_Evt_InfoKey_VideoInfoArray @"kConfUI_Evt_InfoKey_VideoInfoArray"             ///< 视频AIO信息数组



//--------------------------------------------------------------------------------
// 视频数据信息
typedef struct
{
    const char * ullUin;
    bool bMainData;
    unsigned char *rgb24buf;
    int nWidth;
    int nHeight;
    int nRotate;
}tagMAVVideoData;
//视频主辅路
typedef enum enMEDateType
{
    MEDateType_Main  = 1,
    MEDateType_Sub   = 2,
} EnMEDateType;

#endif  /* ConfUIDef_h */