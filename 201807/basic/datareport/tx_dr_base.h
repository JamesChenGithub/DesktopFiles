#ifndef tx_dr_base_h
#define tx_dr_base_h

#include <string>
#include <map>
#include "tx_dr_thread.h"
#include "tx_dr_codec.h"
#include "TXCLock.h"

typedef std::map<std::string, std::string> CMapKeyWithData;

typedef CMapKeyWithData::iterator CMapKeyWithDataItr;


struct stEvtItem {
	int mEvtID;
	std::string mStrToken;
	CMapKeyWithData mMapKeyWithData;
	int mModuleId;
	std::string mStrStreamUrl;
    bool  mReportCommonHead;
    bool  mReportTypeStatus;

	stEvtItem() {
        mEvtID = -1;
		mModuleId = -1;
        mReportCommonHead = true;
        mReportTypeStatus = false;
	}

	stEvtItem(const stEvtItem &itm) {
		mEvtID = itm.mEvtID;
		mStrToken = itm.mStrToken;
		mMapKeyWithData = itm.mMapKeyWithData;
		mModuleId = itm.mModuleId;
        mReportCommonHead = itm.mReportCommonHead;
        mReportTypeStatus = itm.mReportTypeStatus;
        mStrStreamUrl = itm.mStrStreamUrl;
	}

	virtual ~stEvtItem() {

	}

};

class CTXDataReportBase {
public:
	static CTXDataReportBase &GetInstance() {
		return *s_instance;
	}

private:
	CTXDataReportBase();

	CTXDataReportBase(const CTXDataReportBase &);

	CTXDataReportBase &operator=(const CTXDataReportBase &);

	virtual ~CTXDataReportBase() {}

public:
	void SetCommonValue(const char *pEvtKey, const char *pEvtValue);

	void SetEventValue(const char *pEvtToken, int nEvtCommandId, const char *pEvtKey, const char *pEvtValue);

	void ReportEvtGenaral(const char *pEvtToken, int nEvtCommandId);

	void CheckEvent();
    
    int GetStautsReportInterval(){return mReportInterval;}

private:
	std::string GetStreamIDFromUrl(const std::string &url);

	bool SendEvtGeneral(stEvtItem *obj);

	bool SendPacket(const tx_pb_buffer_t &head, const tx_pb_buffer_t &boddy);

	int RecvResponse();

private:
	// head
	unsigned int mPlatform;         // 平台类型，由App接口层提供

	std::string mStrVersion;       // SDK版本，由App接口层提供

	std::string mStrBizID;         // 唯一标识一个业务 （龙珠，斗鱼）

	unsigned long long mAppID;            // 唯一标识一个APP

	//
	typedef std::map<int, stEvtItem> CMapCommandIDWithData;
	typedef CMapCommandIDWithData::iterator CMapCommandIDWithDataItr;
	typedef std::map<std::string, CMapCommandIDWithData> CMapTokenWithData;
	typedef CMapTokenWithData::iterator CMapTokenWithDataItr;

	CMapTokenWithData mMapTokenWithData;

	CMapKeyWithData mMapCommonKeyWithData;

	TXCMutex mMutexTokenWithData;

	int mReportInterval;   // unit ms
    static CTXDataReportBase* s_instance;
};

#endif /* tx_dr_base_h */
