#include <stdio.h>
#include <arpa/inet.h>
#include <cstdlib>
#include "tx_dr_base.h"
#include "tx_dr_def.h"
#include "tx_dr_util.h"

#include "txg_time_util.h"
#include "txg_log.h"

CTXDataReportBase* CTXDataReportBase::s_instance = new CTXDataReportBase();


static void checkBaseEvent() {
	CTXDataReportBase::GetInstance().CheckEvent();
}


CTXDataReportBase::CTXDataReportBase()
		: mStrBizID("5000")
#ifdef _WIN32
		, mPlatform(PLATFORM_WIN_PC)
#elif __APPLE__

#include "TargetConditionals.h"

#if TARGET_IPHONE_SIMULATOR
		, mPlatform(PLATFORM_IOS)
#elif TARGET_OS_IPHONE
		, mPlatform(PLATFORM_IOS)
#elif TARGET_OS_MAC
		, mPlatform(PLATFORM_MAC)
#else
		, mPlatform(PLATFORM_UNKNOWN)
#endif
#elif __ANDROID__
, mPlatform(PLATFORM_ANDROID)
#else
, mPlatform(PLATFORM_UNKNOWN)
#endif
		, mAppID(0), mReportInterval(5000) {
//    CTXDataReportNetThread::GetInstance().AddTimer(checkBaseEvent);
}


void CTXDataReportBase::SetCommonValue(const char *pEvtKey, const char *pEvtValue) {
	if (!pEvtKey || !pEvtValue) {
		return;
	}
	if (!strncmp(pEvtKey, DR_KEY_PLATFORM, strlen(DR_KEY_PLATFORM))) {
		mPlatform = atoi(pEvtValue);
	} else if (!strncmp(pEvtKey, DR_KEY_SDK_VER, strlen(DR_KEY_SDK_VER))) {
		mStrVersion = pEvtValue;
	} else {
		mMapCommonKeyWithData[pEvtKey] = pEvtValue;
	}
}

void
CTXDataReportBase::SetEventValue(const char *pEvtToken, int nEvtCommandId, const char *pEvtKey, const char *pEvtValue) {
	if (!pEvtKey || !pEvtValue || !pEvtToken) {
		return;
	}

	{
		TXCScopedLock lock(mMutexTokenWithData);
		if (!strncmp(pEvtKey, DR_KEY_MODULE_ID, strlen(DR_KEY_MODULE_ID))) {
			mMapTokenWithData[pEvtToken][nEvtCommandId].mModuleId = atoi(pEvtValue);
		} else if (!strncmp(pEvtKey, DR_KEY_STREAMURL, strlen(DR_KEY_STREAMURL))) {
			mMapTokenWithData[pEvtToken][nEvtCommandId].mStrStreamUrl = pEvtValue;
        } else if (!strncmp(pEvtKey, DR_KEY_REPORTCOMMON, strlen(DR_KEY_REPORTCOMMON))){
            mMapTokenWithData[pEvtToken][nEvtCommandId].mReportCommonHead = (strncmp(pEvtValue, "no", 2) != 0);
        } else if (!strncmp(pEvtKey, DR_KEY_REPORTSTATUS, strlen(DR_KEY_REPORTSTATUS))){
            mMapTokenWithData[pEvtToken][nEvtCommandId].mReportTypeStatus = (strncmp(pEvtValue, "no", 2) != 0);
        }
        else {
			mMapTokenWithData[pEvtToken][nEvtCommandId].mMapKeyWithData[pEvtKey] = pEvtValue;
		}
	}
}


std::string& ReplaceAllDistinct(std::string& str,const std::string& old_value,const std::string& new_value)
{
    for(std::string::size_type pos(0); pos!=std::string::npos; pos+=new_value.length())
    {
        if((pos=str.find(old_value,pos))!=std::string::npos)
        {
            str.replace(pos,old_value.length(),new_value);
        }
        else
        {
            break;
        }
    }
    return str;
}

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));
        
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

std::string GetStreamIDFromQuery(std::string &query, const std::string keyname)
{
    query = ReplaceAllDistinct(query, "%26", "&");
    query = ReplaceAllDistinct(query, "%3D", "=");
    
    std::vector<std::string> querys;
    SplitString(query, querys, "&");
    std::vector<std::string>::iterator it = querys.begin();
    for (; it != querys.end(); it++) {
        std::vector<std::string> key_value;
        SplitString(*it, key_value, "=");
        if (key_value.size() != 2) {
            continue;
        }else{
            if (key_value[0] == keyname) {
                return key_value[1];
            }
        }
    }
    return "";
}


std::string CTXDataReportBase::GetStreamIDFromUrl(const std::string &url) {
	//@"rtmp://2157.livepush.myqcloud.com/live/2157_6cfbca22ecad11e5b91fa4dcbef5e35a?bizid=2157"
    std::string query;
	int idx_end = url.find('?');
	int idx_begin;
	if (idx_end == -1) {
		idx_end = url.length();
		idx_begin = url.rfind('/');
	} else {
		idx_begin = url.rfind('/', idx_end);
        query = url.substr(idx_end+1,url.length()-idx_end);
	}

	if (idx_begin == -1) {
        //LOGE("【DR】invalid url");
		return "";
	}

	std::string res = url.substr(idx_begin + 1, idx_end - idx_begin - 1);
    if (res.length() == 0 && query.length() != 0) {
        ///<定制化开发 stream_id 可能是空串。bilibili 的推流地址：rtmp://txy.live-send.acg.tv/live-txy/?streamname%3DXXXXXXX&key%3DXXXXXXXXXXX
        res = GetStreamIDFromQuery(query, "streamname");
    }
	return res;
}

bool CTXDataReportBase::SendEvtGeneral(stEvtItem *obj) {
	// form send buffer.
	tx_pb_buffer_t bufHead;
	bufHead.buf = (unsigned char *) malloc(10 * 1024);
	bufHead.buf_cap = 10 * 1024;
	bufHead.offset = 0;
	encode_head(&bufHead,
				SERVICE_LIVE_VIDEO,
				mPlatform,
				(char *) mStrVersion.c_str(),
				0L,
				(char *) mStrBizID.c_str(),
				(char *) (GetStreamIDFromUrl(obj->mStrStreamUrl).c_str()),
				obj->mModuleId,
                obj->mReportTypeStatus?DATATYPE_STATUS:DATATYPE_EVENT,
				obj->mEvtID,
				txf_getutctick() / 1000,
				(char *) (obj->mStrToken.c_str()));

	tx_pb_buffer_t bufBody;
	bufBody.buf = (unsigned char *) malloc(10 * 1024);
	bufBody.buf_cap = 10 * 1024;
	bufBody.offset = 0;

//    char c_log_p[256] = {0};
//    sprintf(c_log_p, "DR】evtid:%d token:%s platform:%u sdkVer:%s   -----> ", obj->mEvtID, obj->mStrToken.c_str(), mPlatform, mStrVersion.c_str());
//    std::string log_p = c_log_p;

    if (obj->mReportCommonHead){
        for (CMapKeyWithDataItr itr = mMapCommonKeyWithData.begin(); itr != mMapCommonKeyWithData.end(); ++itr) {
            encode_item(&bufBody, 1, (char *) itr->first.c_str(), (char *) itr->second.c_str());
        }
    }else {
            encode_item(&bufBody, 1, DR_KEY_STREAMURL, (char*) obj->mStrStreamUrl.c_str());
    }
//        log_p.append(itr->first);
//        log_p.append(" : ");
//        log_p.append(itr->second);
//        log_p.append(" ; ");

	for (CMapKeyWithDataItr itr = obj->mMapKeyWithData.begin(); itr != obj->mMapKeyWithData.end(); ++itr) {
		encode_item(&bufBody, 1, (char *) itr->first.c_str(), (char *) itr->second.c_str());

//        log_p.append(itr->first);
//        log_p.append(" : ");
//        log_p.append(itr->second);
//        log_p.append(" ; ");
	}

//    LOGE(log_p.c_str());

	bool ret = SendPacket(bufHead, bufBody);

	free(bufBody.buf);
	bufBody.buf = NULL;
	free(bufHead.buf);
	bufHead.buf = NULL;

	return ret;
}

void CTXDataReportBase::ReportEvtGenaral(const char *pEvtToken, int nEvtCommandId) {
	if (!pEvtToken || !nEvtCommandId) return;

	stEvtItem evtItm;
	{
		TXCScopedLock lock(mMutexTokenWithData);
		evtItm = mMapTokenWithData[pEvtToken][nEvtCommandId];
		mMapTokenWithData[pEvtToken].erase(nEvtCommandId);
	}

	if (evtItm.mMapKeyWithData.empty()) {
        LOGE("【DR】 no data to report!");
		return;
	}

	if (evtItm.mModuleId < 0) {
        LOGE("【DR】 invalid mModuleId, mModuleId may not be set!");
		return;
	}

	evtItm.mEvtID = nEvtCommandId;
	evtItm.mStrToken = pEvtToken;

	SendEvtGeneral(&evtItm);

	RecvResponse();
}


bool CTXDataReportBase::SendPacket(const tx_pb_buffer_t &bufHead, const tx_pb_buffer_t &bufBody) {
	// 组包
	int totallen = 10 + bufHead.offset + bufBody.offset;

	char *sendbuf = (char *) malloc(totallen);
	if (sendbuf == NULL) {
        LOGE("【DR】malloc %d Bytes failed!", totallen);
		return false;
	}

	sendbuf[0] = STX_C;
	unsigned int headlen = htonl(bufHead.offset);
	memcpy(sendbuf + 1, &headlen, sizeof(unsigned int));
	unsigned int bodylen = htonl(bufBody.offset);
	memcpy(sendbuf + 5, &bodylen, sizeof(unsigned int));
	memcpy(sendbuf + 9, bufHead.buf, bufHead.offset);
	memcpy(sendbuf + 9 + bufHead.offset, bufBody.buf, bufBody.offset);
	sendbuf[9 + bufBody.offset + bufHead.offset] = ETX_C;

	CTXDataReportNetThread::GetInstance().SendPacket(sendbuf, totallen);

	return true;
}

int CTXDataReportBase::RecvResponse() {
	// STX_C + replen + reppb + ETX_C
	char recvbuf[1024 * 10] = {0};
	char STX = 0;
	int r = 0;
	r = CTXDataReportNetThread::GetInstance().RecvResponse(&STX, 1);

	if (r == 0) { // Proxy Drop and close TCP.
        LOGE("【DR】TCP timeout (4s): Proxy Drop Report");
		return -1; // 发送失败。
	}

	while (r == 1 && STX != STX_C) {
		r = CTXDataReportNetThread::GetInstance().RecvResponse(&STX, 1);
	}

	// 设置了读超时4s,这里的读有可能没有完成就返回了。rsplen 不可信。因此需要设置一个安全区间。
	int rsplen = 0;
	CTXDataReportNetThread::GetInstance().RecvResponse((char *) &rsplen, 4);
	rsplen = ntohl(rsplen);
	if (rsplen > 1024 || rsplen <= 0) {
        LOGE("【DR】body len %d (max 1kB) 网络收包异常", rsplen);
		return 0;
	}

	r = CTXDataReportNetThread::GetInstance().RecvResponse(recvbuf, rsplen);
	if (r != rsplen) {// 前面读到的body长度，可能是错误的。导致此处读到的数据和期望读取的数据不一致。
        LOGE("【DR】body len %d ,but read %d", rsplen, r);
		return 0;
	}

	char ETX = 0;
	r = CTXDataReportNetThread::GetInstance().RecvResponse(&ETX, 1);
	if (r != 1 && ETX != ETX_C) {
        LOGE("【DR】not ETX_C");
		return 0;
	}

	tx_pb_buffer_t *decbuf = (tx_pb_buffer_t *) malloc(sizeof(tx_pb_buffer_t));
	decbuf->buf = (unsigned char *) recvbuf;
	decbuf->buf_cap = rsplen;
	decbuf->offset = 0;

	int islongconn = 0;
	unsigned int interval = 0;
	int result = 0;
	unsigned int log_upload_flag = 0;
	unsigned int send_interval = 0;
	unsigned int resend_times = 30;

	int moudleID = 0;
	unsigned char streamID[512] = {0};
	unsigned int streamIDLen = 0;
	unsigned char errInfo[512] = {0};
	unsigned int errInfoLen = 0;

	DECODE_PB_MSG_BEGIN(decbuf)      // begin decode example_t

			DECODE_PB_MSG_FIELD_BEGIN(decbuf, 1)
				if (!tx_pb_decode_uint32(decbuf, &moudleID)) {
        			LOGE("【DR】failed decode moudleID");
					return 0;
				}
			DECODE_PB_MSG_FIELD_END

			DECODE_PB_MSG_FIELD_BEGIN(decbuf, 2)
				if (!tx_pb_decode_string(decbuf, streamID, sizeof(streamID) - 1, &streamIDLen)) {
        			LOGE("【DR】failed decode streamID");
					return 0;
				}
			DECODE_PB_MSG_FIELD_END

			DECODE_PB_MSG_FIELD_BEGIN(decbuf, 3)
				if (!tx_pb_decode_uint32(decbuf, &result)) {
        			LOGE("【DR】failed decode result");
					return 0;
				}
			DECODE_PB_MSG_FIELD_END

			DECODE_PB_MSG_FIELD_BEGIN(decbuf, 4)
				if (!tx_pb_decode_string(decbuf, errInfo, sizeof(errInfo) - 1, &errInfoLen)) {
        			LOGE("【DR】failed decode errInfo");
					return 0;
				}
			DECODE_PB_MSG_FIELD_END

			DECODE_PB_MSG_FIELD_BEGIN(decbuf, 5)
				if (!tx_pb_decode_uint32(decbuf, &islongconn)) {
        			LOGE("【DR】failed decode islongconn");
					return 0;
				}
			DECODE_PB_MSG_FIELD_END


			DECODE_PB_MSG_FIELD_BEGIN(decbuf, 6)
				if (!tx_pb_decode_uint32(decbuf, &interval)) {
        			LOGE("【DR】failed decode report interval");
					return 0;
				}
			DECODE_PB_MSG_FIELD_END

			DECODE_PB_MSG_FIELD_BEGIN(decbuf, 7)
				if (!tx_pb_decode_uint32(decbuf, &log_upload_flag)) {
        			LOGE("【DR】failed decode log_flag");
					return 0;
				}
			DECODE_PB_MSG_FIELD_END

			DECODE_PB_MSG_FIELD_BEGIN(decbuf, 8)
				if (!tx_pb_decode_uint32(decbuf, &send_interval)) {
					LOGE("【DR】failed decode send interval");
					return 0;
				}
			DECODE_PB_MSG_FIELD_END

			DECODE_PB_MSG_FIELD_BEGIN(decbuf, 9)
				if (!tx_pb_decode_uint32(decbuf, &resend_times)) {
					LOGE("【DR】failed decode resend times");
					return 0;
				}
			DECODE_PB_MSG_FIELD_END

	DECODE_PB_MSG_END(decbuf)  // end decode example_t

	free(decbuf);

	LOGD("moudleID:%d, streamID:%s, result:%d, errInfo:%s", moudleID, streamID, result, errInfo);

	mReportInterval = interval * 1000;
	if (mReportInterval < 1000) {
		mReportInterval = 5000;
	}

	if (islongconn != 1) {
		// server 要求断开TCP连接。
		CTXDataReportNetThread::GetInstance().ColseServer();
	}

    // 设置发送线程的发送间隔
    CTXDataReportNetThread::GetInstance().SetSendInterval(send_interval);

    // 设置发送线程的发送次数
    CTXDataReportNetThread::GetInstance().SetMaxResendTimes(resend_times);


	return result;
}

void CTXDataReportBase::CheckEvent() {
	//do timer!
}
