#ifndef tx_dr_thread_h
#define tx_dr_thread_h

#include <vector>
#include <list>
#include <string>
#include "TXCLock.h"
#include "TXCAbstractThread.h"

typedef void(*Timer)();

class CTXDataReportNetThread : TXCAbstractThread {
public:
	static CTXDataReportNetThread &GetInstance();

private:
	CTXDataReportNetThread();

	CTXDataReportNetThread(CTXDataReportNetThread &);

	CTXDataReportNetThread &operator=(const CTXDataReportNetThread &);

	~CTXDataReportNetThread();

public:
	bool SendPacket(const char *pSendData, const int nSendDataLen);

	int RecvResponse(char *pRecvData, int nRecvMaxLen);

	int ColseServer();

	/**
	 * arvinwu @2017-12-26 add
	 * @param interval 单位ms，发送完一个上报后sleep时间。默认为0ms
	 */
	void SetSendInterval(int interval);

	/**
	 *
	 * @param times 发送失败最大重试次数,0表示无限重试，默认为0。
	 */
	void SetMaxResendTimes(int times);

	void AddTimer(Timer timer);

	void DeleteTimer(Timer timer);

private:
	bool threadLoop();

	// 网络相关
	void DoDNS();

	bool ConnectServer();

	bool ConnectServerInternal(std::string ip);
    
    void DropSendCache();

private:
	std::vector<std::string> mServerIps;

	// 上报通道相关 采用TCP连接
	int mSocket;
	int mSocketStatus;
	int mnIdleTime;
	bool mbDNSDone;

	struct CItem {
		char *mData;
		int dataLen;

		CItem()
				: mData(NULL), dataLen(0) {

		}

		CItem(char *pData, const int nDataLen)
				: dataLen(nDataLen), mData(pData) {

		}

		CItem(const CItem &itm) {
			mData = itm.mData;
			dataLen = itm.dataLen;
		}

		virtual ~CItem() {

		}
	};

	struct CCycleQueue {
		char *mData;

		int mSize;
		int mMaxSize;
		int mCapacity;

		int mHead;
		int mEnd;

		CCycleQueue(int size)
				: mCapacity(size), mSize(0), mMaxSize(0), mHead(0), mEnd(0) {
			if (mCapacity <= 1) mCapacity = 2;

			mMaxSize = mSize = mCapacity - 1;

			mData = new char[mCapacity];
		}

		virtual ~ CCycleQueue() {
			if (mData) {
				delete[] mData;
			}
		}

		void append(const char *data, size_t len) {
			size_t left = len;
			while (left) {
				if (!mSize) {
					mEnd = (mEnd + mMaxSize / 2) % mCapacity;
					mSize = mMaxSize / 2;
				}

				if (left > mSize) {
					if ((mHead + mSize + 1) <= mEnd || 0 == mEnd || 1 == mEnd) {
						left -= appendInterval(data + (len - left), mSize);
					} else {
						left -= appendInterval(data + (len - left), mCapacity - mHead);

						left -= appendInterval(data + (len - left), mSize);
					}
				} else {
					if ((mHead + left + 1) <= mEnd || (mHead >= mEnd && (mEnd == 0 || left <= (mSize - mEnd + 1)))) {
						left -= appendInterval(data + (len - left), left);
					} else {
						left -= appendInterval(data + (len - left), mCapacity - mHead);

						left -= appendInterval(data + (len - left), left);
					}
				}
			}
		}

		size_t appendInterval(const char *data, size_t len) {
			memcpy(mData + mHead, data, len);
			mHead = (mHead + len) % mCapacity;
			mSize -= len;

			return len;
		}

		int query(char *data, int len) {
            int ret = len;
			if (len + mSize > mMaxSize) return 0;

			if (mHead > mEnd || (mHead < mEnd && len <= (mMaxSize - mSize - mHead))) {
				memcpy(data, mData + mEnd, len);
				mEnd += len;
				mSize += len;
			} else {
				int cpyLen = mCapacity - mEnd;
				memcpy(data, mData + mEnd, cpyLen);
				mEnd = 0;
				mSize += cpyLen;

				data += cpyLen;
				len -= cpyLen;

				memcpy(data, mData + mEnd, len);
				mEnd += len;
				mSize += len;
			}
			return ret;
		}
	};

	TXCMutex mSendLock;
	std::list<CItem> mSendCache;

	TXCMutex mRecvLock;
	CCycleQueue mRecvCache;

	TXCMutex mEvtTimersLock;
	std::list<Timer> mEvtTimers;

	TXCMutex mCloseLock;
	bool mbCloseNet;
    
    bool mSocketCreateFailedFlag;

	int mSendInterval;    //

	int mMaxResendTimes;  //

    int mResendTimes;     // 当前的重试次数

	bool mbHardcodeIPInited;  // 域名解析工作放到，数据上报发送线程

};

#endif /* tx_dr_mgr_h */
