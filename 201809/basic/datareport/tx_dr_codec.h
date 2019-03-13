#ifndef __TX_DR_CODER_H__
#define __TX_DR_CODER_H__

#include "tx_pb_codec.h"

#ifdef __cplusplus
extern "C" {
#endif

bool encode_head(tx_pb_buffer_t *encbuf,
				 unsigned int server_id,            // 服务类型，取值参见SERVICE_TYPE, 本上报一律填 SERVICE_LIVE_VIDEO
				 unsigned int platform_id,          // 平台类型，取值参见PLATFORM_TYPE, 本上报 PLATFORM_IOS / PLATFORM_ANDROID
				 char *version,                     // 上报方的版本，本上报取SDK版本
				 unsigned long long app_id,         // 唯一标识某个第三方应用的appid，由上层提供
				 char *biz_id,                      // 唯一标识某个用户的id，URL解析
				 char *stream_id,                   // 唯一标识某个流的id，URL解析
				 unsigned int module_id,            // 哪个模块上报的数据，参考 MODULE_ID，本上报一律填MODULE_PUSH_SDK / MODULE_PLAYER_SDK
				 unsigned int date_type,            // 数据的类型，事件数据为1，区间统计数据为2

				 unsigned int command_id,           //
				 unsigned long long timestamp,      // 数据的产生时间，unix秒数
				 char *token                       // 用于鉴权，暂时未用。
);


bool encode_item(tx_pb_buffer_t *encbuf,
				 int tag,
				 char *key,
				 char *value);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
