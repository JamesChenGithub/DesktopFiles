#include "tx_dr_codec.h"

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
				 char *token
) {


	if (!tx_pb_encode_varint(encbuf, 1, server_id)) {
		return false;
	}

	if (!tx_pb_encode_varint(encbuf, 2, platform_id)) {
		return false;
	}

	if (!tx_pb_encode_string(encbuf, 3, (unsigned char *) version, strlen(version))) {
		return false;
	}

	if (!tx_pb_encode_varint(encbuf, 4, app_id)) {
		return false;
	}

	if (!tx_pb_encode_string(encbuf, 5, biz_id, strlen(biz_id))) {
		return false;
	}

	if (!tx_pb_encode_string(encbuf, 6, stream_id, strlen(stream_id))) {
		return false;
	}

	if (!tx_pb_encode_varint(encbuf, 7, module_id)) {
		return false;
	}

	if (!tx_pb_encode_varint(encbuf, 8, date_type)) {
		return false;
	}

	if (!tx_pb_encode_varint(encbuf, 9, command_id)) {
		return false;
	}

	if (!tx_pb_encode_varint(encbuf, 10, timestamp)) {
		return false;
	}

	if (!tx_pb_encode_string(encbuf, 11, token, strlen(token))) {
		return false;
	}

	return true;
}

bool encode_item(tx_pb_buffer_t *encbuf,
				 int tag,
				 char *key,
				 char *value) {
	ENCODE_SUB_PB_MSG_BEGIN(encbuf, tag)
		if (!tx_pb_encode_string(encbuf, 1, key, strlen(key))) {
			return false;
		}
		if (!tx_pb_encode_string(encbuf, 2, value, strlen(value))) {
			return false;
		}
	ENCODE_SUB_PB_MSG_END(encbuf)
	return true;
}
