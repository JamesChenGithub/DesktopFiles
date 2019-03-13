//
//  dr_general_def.h
//  TXRTMPSDK
//
//  Created by realingzhou on 2017/4/25.
//
//

#ifndef dr_general_def_h
#define dr_general_def_h

#ifdef __cplusplus
extern "C" {
#endif
    
/******************************** 协议字段定义 ***************************/


/*********** 事件标准字段 ***********/
/*
 * 以下为所有事件上报都会带的字段
 */
    
//protol head
#define DR_KEY_MODULE_ID    "u32_module_id"
#define DR_KEY_STREAMURL    "str_stream_url"
#define DR_KEY_PLATFORM     "platform"              // 平台类型，由App接口层提供
#define DR_KEY_SDK_VER      "sdk_version"           // SDK版本，由App接口层提供
#define DR_KEY_BIZ_ID       "biz_id"                // 唯一标识一个业务 （龙珠，斗鱼）
#define DR_KEY_APP_ID       "app_id"                // 唯一标识一个APP
    
//protol body head
#define DR_KEY_DEVTYPE     "dev_type"              // 设备型号
#define DR_KEY_NET_TYPE     "net_type"              // 网络类型
#define DR_KEY_DEV_ID       "dev_id"                // 手机唯一标示
#define DR_KEY_DEV_UUID     "dev_uuid"              // 新增手机唯一标示,(dev_id不可靠, 但是暂时不能删除dev_id)
#define DR_KEY_APP_NAME     "app_name"              // app 包名
#define DR_KEY_APP_VER      "app_version"            // 应用版本名
#define DR_KEY_SYS_VER      "sys_version"            // 系统版本号
#define DR_KEY_MAC_ADDR     "mac_addr"              // mac地址，仅Android有
#define DR_KEY_EVT_COMM     "event_comm"              // 事件说明，比如 40001，可以填"start_push"
#define DR_KEY_SDK_ID       "sdk_id"                // sdk说明，详见 DR_SDK_ID
   
//
#define DR_KEY_REPORTCOMMON "report_common"         // 用于控制是否上报body head。
#define DR_KEY_REPORTSTATUS "report_status"         // 用于控制是否是区间状态上报。
    
/*********** 事件特有字段 ***********/
/*
 * 以下为各事件上报特有字段
 */
   
/**  DAU统计上报 **/
#define DR_KEY_DAU_EVENT_ID         "event_id"           //操作发生的模块
#define DR_KEY_DAU_ERR_CODE         "err_code"           //操作结果
#define DR_KEY_DAU_ERR_INFO         "err_info"           //操作错误信息
#define DR_KEY_DAU_EXT              "ext"                //拓展字段
    
/**  ugc uplaod 统计上报 **/
#define DR_KEY_UPLAOD_EVENT_ID         "event_id"           //操作发生的模块
#define DR_KEY_UPLAOD_ERR_CODE         "err_code"           //操作结果
#define DR_KEY_UPLAOD_ERR_INFO         "err_info"           //操作错误信息
#define DR_KEY_UPLOAD_FILE_SIZE        "file_size"          //上传文件大小
#define DR_KEY_UPLOAD_UP_USE_TIME      "up_usetime"         //上传耗时
#define DR_KEY_UPLOAD_UP_ip            "up_ip"              //上传时解析出的第一跳IP
    
/************ 40003 异常上报相关字段 ***************/
#define  DR_KEY_ERROR_CODE            "u32_error_code"
#define  DR_KEY_ERROR_MSG             "str_error_msg"
#define  DR_KEY_MSG_MORE              "str_msg_more"
    
    /**
     *   推流异常码
     */
    enum PUSH_ERR_EVT
    {
        PEE_SERVER_DROP             = 1000,     // 服务器主动断开
        PEE_CONN_SERVER_FAILED      = 1001,     // SDK 连接upload服务器失败
        PEE_RECONN_BEGIN            = 1002,     // 发起自动重连
        
        PEE_DROP_FRAME              = 2001,     // 网络拥塞，主动丢帧
        PEE_TURN_HWENCODE           = 2002,     // 用户切换软硬编 & 硬编切换失败切软编
        PEE_ENCODE_INIT             = 2003,     // 编码线程初始化 & 硬编切换失败切软编
        PEE_ENCODE_FAIL             = 2004,     // 编码失败切软编。
        PEE_ENCODE_INIT_ANDROID     = 2005,     // 编码线程初始化 & 硬编切换失败切软编 for android
        PEE_ENCODE_FAIL_ANDROID     = 2006,     // Android硬编编码失败
        
        PEE_QUIC_TO_TCP             = 2007,     // Quic连接失败切TCP
        PEE_QOS_RESULT              = 2008,     // QOS 流控码率和分辨率
        PEE_VIDEO_ENCODE_SW_TO_HW   = 2009,     // 视频软编性能不足，切硬编。
    };

/******************************** 协议相关枚举值 ***************************/
    
// 命令 ID：
enum COMMAND_EVTID
{
    //- 推流相关
    COMMAND_ID_40001        = 40001, //开始推流
    COMMAND_ID_40000        = 40000, //推流中 每隔5s 上报一次。
    COMMAND_ID_40002        = 40002, //结束推流
    COMMAND_ID_40003        = 40003, //推流异常上报
    COMMAND_ID_40005        = 40005, //推流链路质量上报
    
    // - 直播播放相关
    COMMAND_ID_40101        = 40101, //开始直播播放
    COMMAND_ID_40100        = 40100, //播放中 每隔5s 上报一次
    COMMAND_ID_40102        = 40102, //结束直播播放
    
    // - 实时播放相关
    COMMAND_ID_40501        = 40501, //开始实时播放
    COMMAND_ID_40500        = 40500, //播放中 每隔5s 上报一次
    COMMAND_ID_40502        = 40502, //结束实时播放
    
    // - 点播播放行为相关
    COMMAND_ID_40201        = 40201,
    COMMAND_ID_40200        = 40200,
    COMMAND_ID_40202        = 40202,
    
    // - 点播播放行为相关
    COMMAND_ID_VOD_ACTION_DR    = 40301, //播放结束后统计
    
    // - UGC相关
    COMMAND_ID_UGC_UPLOAD   = 40401,
    
    // - 连麦
    COMMAND_ID_LINKMIC      = 40402,
    
    // - QUIC
    COMMAND_ID_QUIC         = 40403,
    
    // - DAU统计
    COMMAND_ID_DAU          = 49999,
};
    
#define COMMAND_ID_COMMENT_UGC_UPLOAD_40401         "ugc_upload"
#define COMMAND_ID_COMMENT_DAU_49999                "DAU"
#define COMMAND_ID_COMMENT_LINKMIC_40402            "LINKMIC"
#define COMMAND_ID_COMMENT_QUIC_40403               "QUIC"

// 服务类型
enum SERVICE_TYPE
{
    SERVICE_LIVE_VIDEO        = 0x1, //直播
    SERVICE_ONDEMAND_VIDEO    = 0x2, //点播
    SERVICE_ONLINE_VIDEO      = 0x3, //在线视频
    SERVICE_MULTI_VIDEO       = 0x4, //多人视频
    SERVICE_P2P_VIDEO         = 0x5, //点对点视频
    SERVICE_UNKNOWN           = 0xFF,//未知类型
};
    
//模块类型
/*模块类型
 - SDK 只用到 带 “*” 的 MODULE_ID
 - MODULE_PUSH_SDK
 - MODULE_PLAYER_SDK
 - MODULE_VOD_PLAYER_SDK
 */
enum MODULE_ID
{
    MODULE_PPSERVER             = 1000, //拉推流模块
    MODULE_UPLOAD               = 1001, //upload
    MODULE_STREAM_PROCESS       = 1002, //流处理
    MODULE_CONTROLLER           = 1003, //controller
    MODULE_PUSH_SDK             = 1004, //推流SDK  *
    MODULE_PLAYER_SDK           = 1005, //播放SDK  *
    MODULE_TEG_UPLOAD           = 1006, //upload_teg
    MODULE_LIVE_CODEC           = 1007, //livecodec
    MODULE_DC                   = 1008, //dc
    MODULE_CDN                  = 1009, //cdn
    MODULE_VOD_PLAYER_SDK       = 1010, //点播SDK  *
    MODULE_VOD_ACTION_PLAYER_SDK = 1011, //点播行为
};


/* 网络类型 */
enum NETWORK_TYPE
{
    NETWORK_TYPE_UNKNOWN   = 0xFF,
    NETWORK_TYPE_WIFI      = 0x1,
    NETWORK_TYPE_4G        = 0x2,
    NETWORK_TYPE_3G        = 0x3,
    NETWORK_TYPE_2G        = 0x4,
};

/* 平台类型
 - SDK 只用到 带 “*” 的 PLATFORM_TYPE
 - PLATFORM_IOS
 - PLATFORM_ANDROID
 */
enum PLATFORM_TYPE
{
    PLATFORM_IOS              = 0x1, // iOS *
    PLATFORM_ANDROID          = 0x2, // Android *
    PLATFORM_WEB              = 0x3, // WEB客户端
    PLATFORM_SERVER           = 0x4, // 服务器上报的
    PLATFORM_THIRD_SERVER     = 0x5, // 第三方服务器上报的
    PLATFORM_WIN_PHONE        = 0x6, // windows phone
    PLATFORM_WIN_PC           = 0x7, // windows PC客户端
    PLATFORM_MAC              = 0x8, // MAC客户端
    PLATFORM_UNKNOWN          = 0xFF,// 未知类型
};
    
enum DR_DAU_EVENT_ID
{
    //1000~1099  for rtmpsdk/ liteavsdk
    DR_DAU_EVENT_ID_LINK_MIC          = 1001,//连麦
    DR_DAU_EVENT_ID_UGC_PUSH_RECORD   = 1002,//UGC录制
    DR_DAU_EVENT_ID_UGC_PLAY_RECORD   = 1003,//截流录制
    DR_DAU_EVENT_ID_UGC_CUT           = 1004,//短视频裁剪
    DR_DAU_EVENT_ID_UGC_JOIN          = 1005,//短视频合并
    DR_DAU_EVENT_ID_UGC_PUBLISH       = 1006,//短视频上传
    DR_DAU_EVENT_ID_REVERB            = 1007,//混响
    DR_DAU_EVENT_ID_BGM               = 1008,//背景音
    DR_DAU_EVENT_ID_EYE_SCALE         = 1009,//大眼，弃用，统一使用BeautyModule内定义的
    DR_DAU_EVENT_ID_FACE_SCALE        = 1010,//瘦脸，弃用，统一使用BeautyModule内定义的
    DR_DAU_EVENT_ID_FILTER            = 1011,//滤镜，弃用，统一使用BeautyModule内定义的
    DR_DAU_EVENT_ID_GREEN_SCREEN      = 1012,//绿幕，弃用，统一使用BeautyModule内定义的
    DR_DAU_EVENT_ID_MOTION_TMPL       = 1013,//动效，弃用，统一使用BeautyModule内定义的
    DR_DAU_EVENT_ID_SCREEN_CAPTURE    = 1014,//录屏
    DR_DAU_EVENT_ID_PUSH_RECORD       = 1015,//推流录制
    DR_DAU_EVENT_ID_UGC_LICENSE_SUC   = 1016,//短视频license校验成功量
    DR_DAU_EVENT_ID_UGC_LICENSE_FAILED = 1017,//短视频License校验失败量

    // 1100 ~ 1199 for avsdk
    
    // 1200 ~ 1299 for beautysdk
    
    
    DR_DAU_EVENT_ID_VOD_PLAY          = 1997,//点播播放
    DR_DAU_EVENT_ID_LIVE_PLAY         = 1998,//直播播放
    DR_DAU_EVENT_ID_LIVE_PUSH         = 1999,//直播推流
    
    
    //1300 ~ 1350 for avroom
    DR_DAU_EVENT_ID_AVROOM_ENTER      = 1301,//进房
};
    
enum DR_SDK_ID
{
    DR_SDK_ID_RTMPSDK            = 1, // rtmp sdk
	DR_SDK_ID_LITEAV			 = 2, // lite av sdk
    DR_SDK_ID_BEAUTY_SDK         = 3, // beauty sdk
    DR_SDK_ID_VOD_SDK			 = 4, // vod sdk
    DR_SDK_ID_AVSDK              = 10, // avsdk
};
    
enum DR_UGC_UPLOAD_EVENT_ID
{
    UPLOAD_EVENT_ID_REQUEST_UPLOAD = 1,
    UPLOAD_EVENT_ID_UPLOAD         = 2,
    UPLOAD_EVENT_ID_UPLOAD_RESULT  = 3,
};
    
// 请求格式:
// STX_C + head_len[uint32] + body_len[uint32] + QualityRptHead + QualityRptBody + ETX_C
// STX_C is 0x28, ETX_C is 0x29
#define STX_C  0x28
#define ETX_C  0x29
    
#define DATATYPE_EVENT      1
#define DATATYPE_STATUS     2
    
//#define STR(x) #x
//#define SDK_VERSION(x,y,z) STR(x) "." STR(y) "." STR(z)

#ifdef __cplusplus
}
#endif

#endif /* dr_general_def_h */
