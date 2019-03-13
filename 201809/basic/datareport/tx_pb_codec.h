#ifndef __PB_CODEC_H__
#define __PB_CODEC_H__

#include <string.h>
#include <stdint.h>

#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#else
#define _Bool bool
#endif

    
#ifdef __cplusplus
extern "C" {
#endif
    
typedef enum {
	PB_WT_VARINT = 0,
	PB_WT_64BIT  = 1,
	PB_WT_STRING = 2,
	PB_WT_32BIT  = 5,
} tx_pb_wire_type_t;

typedef struct {
	uint8_t  *buf;
	uint32_t buf_cap;
	uint32_t offset;
} tx_pb_buffer_t;

// interface function
bool tx_pb_encode_varint(tx_pb_buffer_t *pb_buf, uint32_t field_number, uint64_t value);
bool tx_pb_encode_string(tx_pb_buffer_t *pb_buf, uint32_t field_number, const uint8_t *src, uint32_t n);

bool tx_pb_decode_uint8(tx_pb_buffer_t *pb_buf, void *dst);
bool tx_pb_decode_uint32(tx_pb_buffer_t *pb_buf, void *dst);
bool tx_pb_decode_uint64(tx_pb_buffer_t *pb_buf, void *dst);
bool tx_pb_decode_string(tx_pb_buffer_t *pb_buf, uint8_t *dst, uint32_t dst_cap, uint32_t *n);
bool tx_pb_decode_submsg(tx_pb_buffer_t *pb_buf, tx_pb_buffer_t *sub_pb_buf);

// helper function
bool tx_pb_write(tx_pb_buffer_t *pb_buf, const uint8_t *src, uint32_t n);
bool tx_pb_read(tx_pb_buffer_t *pb_buf, uint8_t *dst, uint32_t n);

bool tx_pb_encode_tag(tx_pb_buffer_t *pb_buf, uint32_t field_number, tx_pb_wire_type_t wire_type);
bool tx_pb_encode_varint_with_no_field(tx_pb_buffer_t *pb_buf, uint64_t value);
uint32_t tx_pb_encode_varint_to_buf(uint8_t *dst, uint64_t value);

bool tx_pb_decode_varint(tx_pb_buffer_t *pb_buf, void *dst, uint32_t data_len);
bool tx_pb_decode_tag(tx_pb_buffer_t *pb_buf, uint32_t *field_number, tx_pb_wire_type_t *wire_type, bool *eof);
bool tx_pb_skip_varint(tx_pb_buffer_t *pb_buf);
bool tx_pb_skip_string(tx_pb_buffer_t *pb_buf);
bool tx_pb_skip_field(tx_pb_buffer_t *pb_buf, tx_pb_wire_type_t wire_type);


// encode
#define ENCODE_SUB_PB_MSG_BEGIN(enc_pb_buf, field_number)\
{\
	if (!tx_pb_encode_tag(enc_pb_buf, field_number, PB_WT_STRING)) return false;\
	if (!tx_pb_encode_varint_with_no_field(enc_pb_buf, 0)) return false;\
	uint32_t u32ValueBeginPos = enc_pb_buf->offset;

#define ENCODE_SUB_PB_MSG_END(enc_pb_buf)\
	uint32_t u32ValueEndPos = enc_pb_buf->offset;\
	uint32_t u32ValueLen = u32ValueEndPos - u32ValueBeginPos;\
	uint8_t bufValueLen[10];\
	uint32_t n = tx_pb_encode_varint_to_buf(bufValueLen, u32ValueLen);\
	if (enc_pb_buf->buf_cap < enc_pb_buf->offset + n - 1) return false;\
	if (n > 1) {\
		uint32_t i = 0;\
		for (i = u32ValueEndPos-1; i >= u32ValueBeginPos; --i) {\
			enc_pb_buf->buf[i+n-1] = enc_pb_buf->buf[i];\
		}\
	}\
	enc_pb_buf->offset += (n-1);\
	memcpy(enc_pb_buf->buf + u32ValueBeginPos - 1, bufValueLen, n);\
}

// decode
#define DECODE_PB_MSG_BEGIN(dec_pb_buf) {\
	while (dec_pb_buf->offset < dec_pb_buf->buf_cap) {\
		uint32_t field_number = 0; \
		tx_pb_wire_type_t wire_type = PB_WT_VARINT; \
		bool eof = false;\
		bool skip = true;\
		if (!tx_pb_decode_tag(dec_pb_buf, &field_number, &wire_type, &eof)) {\
			if (eof) break;\
			else { return false; } \
		}
	
#define DECODE_PB_MSG_END(dec_pb_buf)\
		if (skip) {\
			if (!tx_pb_skip_field(dec_pb_buf, wire_type)) return false;\
		}\
	}\
}

#define DECODE_PB_MSG_FIELD_BEGIN(dec_pb_buf, field)\
if (field_number == field) {\
	skip = false;

#define DECODE_PB_MSG_FIELD_END \
}

#ifdef __cplusplus
} /* extern "C" */
#endif
    
#endif /* __PB_CODEC_H__ */
