#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include "txg_connect_util.h"
#include "tx_dr_thread.h"
#include "tx_dr_util.h"
#include "txg_log.h"

#define CONN_STATE_CONNECTED 1
#define CONN_STATE_UNCONNECT 2

#define CONN_RECV_CACHE_SIZE (4*1024)

/*
 * arvinw @2017-12-25.
 * 2.0.3 版本 数据上报逻辑引入了一个循环发送脏数据的bug。占用大量服务器的带宽资源。后续版本修复了，但是2.0.3版本留存量大，且量不断增加。
 * 规避措施：最新的版本sdk4.0，上报到新的域名以及VIP和新的端口。待sdk4.0 版本覆盖达到指定覆盖率。封掉旧的20164上报端口。
 * 1. 修改上报域名
 * 2. 修改上报接口
 * 3. 修改上报预留VIP地址
 * 4. 修复内存释放函数不配对的问题
 */
#define SERVER_DNS "http://mlvbdc.live.qcloud.com/"
#define SERVER_TEST_IP "183.60.50.29"

#define SERVER_TEST_PORT 20166

#define MAX_SEND_CACHE 200

#define VIP_COUNT  3

static std::string g_report_server_ipv4s[VIP_COUNT] = {
		"183.36.108.244",
		"58.250.136.37",
		"121.51.141.89"
};

CTXDataReportNetThread::CTXDataReportNetThread()
		: mSocket(-1), mSocketStatus(CONN_STATE_UNCONNECT), mnIdleTime(0), mbDNSDone(false),
		  mRecvCache(CONN_RECV_CACHE_SIZE), mbCloseNet(false), mSocketCreateFailedFlag(false),mSendInterval(0), mMaxResendTimes(30), mResendTimes(0), mbHardcodeIPInited(
                false) {

}

static TXCMutex* s_oMutex = new TXCMutex();

CTXDataReportNetThread &CTXDataReportNetThread::GetInstance() {
    TXCScopedLock lock(*s_oMutex);
	static CTXDataReportNetThread* instance = NULL;
    if (instance == NULL)
    {
        instance = new CTXDataReportNetThread();
    }
	return *instance;
}

CTXDataReportNetThread::~CTXDataReportNetThread() {
	if (mSocketStatus == CONN_STATE_CONNECTED) {
		close(mSocket);
		mSocket = -1;
		mSocketStatus = CONN_STATE_UNCONNECT;
	}
}

void CTXDataReportNetThread::DoDNS() {
	if (mbDNSDone == false) {
		struct sockaddr_storage ips[10] = {0};
		socklen_t iplen[10] = {0};
		int ips_count = 10;
		// 这个函数有网络交互，在网络不好的情况下，可能会卡很久。
		txf_nslookup(SERVER_DNS, ips, iplen, &ips_count);

		for (int i = 0; i < ips_count; i++) {
			char host[NI_MAXHOST];
			getnameinfo((struct sockaddr *) &ips[i], iplen[i], host, sizeof(host), NULL, 0, NI_NUMERICHOST);
			mServerIps.insert(mServerIps.begin(), host);
		}
		mbDNSDone = true;
	}
}

void CTXDataReportNetThread::SetMaxResendTimes(int times)
{
    mMaxResendTimes = times;
}

void CTXDataReportNetThread::SetSendInterval(int interval)
{
    mSendInterval = interval;
}

bool CTXDataReportNetThread::ConnectServerInternal(std::string ip) {
	struct sockaddr *addr;
	int addr_len;
	//    {
	struct sockaddr_in addr4;
	bzero(&addr4, sizeof(addr4));
	addr4.sin_family = AF_INET;
	addr4.sin_port = htons(SERVER_TEST_PORT);

	struct sockaddr_in6 addr6;
	bzero(&addr6, sizeof(addr6));
	addr6.sin6_family = AF_INET6;
	addr6.sin6_port = htons(SERVER_TEST_PORT);

	if (inet_pton(AF_INET, ip.c_str(), &addr4.sin_addr) == 1) {
		addr = (struct sockaddr *) &addr4;
		addr_len = sizeof(addr4);
	} else if (inet_pton(AF_INET6, ip.c_str(), &addr6.sin6_addr) == 1) {
		addr = (struct sockaddr *) &addr6;
		addr_len = sizeof(addr6);
	} else {
		return false;
	}

	mSocket = socket(addr->sa_family, SOCK_STREAM, 0);
	if (mSocket < 0) {
        if (mSocketCreateFailedFlag == false) {
            LOGE("【DR】create socket failed!");
            mSocketCreateFailedFlag = true;
        }
		return false;
	}

    mSocketCreateFailedFlag = false;
#ifdef __APPLE__
	{
		int set = 1;
		setsockopt(mSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *) &set, sizeof(set));
	}
#endif

	// 设置TCP 超时时间 5s, 一般5s内都能成功。
	struct timeval timeout = {4, 0};

	//设置接收超时
	setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(struct timeval));

	if (connect(mSocket, addr, addr_len) < 0) {
		close(mSocket);
		return false;
	}

	return true;
}

bool CTXDataReportNetThread::ConnectServer() {
    /*
     * arvinwu @ 2018-01-02
     * hardcode 上报vip列表初始化，放在数据上报发送线程，防止getaddrinfo的dns操作卡顿，影响其他正常线程。
     */
    if (mbHardcodeIPInited == false)
    {
        mbHardcodeIPInited = true;

        for (int i = 0; i < VIP_COUNT; i++) {
#ifdef __APPLE__
            struct addrinfo hints, *res, *res0;
            int error;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = PF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_flags = AI_DEFAULT;
            error = getaddrinfo(g_report_server_ipv4s[i].c_str(), "http", &hints, &res0);
            if (error) {
                continue;
                /*NOTREACHED*/
            }
            for (res = res0; res; res = res->ai_next) {
                char host[NI_MAXHOST];
                getnameinfo((struct sockaddr *) res->ai_addr, res->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
                mServerIps.push_back(host);
            }
            freeaddrinfo(res0);
#else
            mServerIps.push_back(g_report_server_ipv4s[i]);
#endif
        }
    }

	if (mSocket > 0) {
		mSocketStatus = CONN_STATE_CONNECTED;
		//LOGW("connection is running!");
		return false;
	}

	DoDNS();

	std::vector<std::string>::iterator it = mServerIps.begin();
	for (; it != mServerIps.end(); it++) {
		if (ConnectServerInternal(*it)) {
			mSocketStatus = CONN_STATE_CONNECTED;
			//LOGD("connect server success!");
			return true;;
		}
	}

	mbDNSDone = false; //有可能由ipv4切换到ipv6，需要重新dns -- fx
	mSocketStatus = CONN_STATE_UNCONNECT;
	mSocket = -1;
	return false;
}

bool CTXDataReportNetThread::SendPacket(const char *pSendData, const int nSendDataLen) {
	if (!pSendData || !nSendDataLen) return false;

	{
		TXCScopedLock _l(mSendLock);
		mSendCache.push_back(CItem((char *) pSendData, nSendDataLen));
        DropSendCache();
	}

	run();

	return true;
}

void CTXDataReportNetThread::DropSendCache() {
    if (mSendCache.size()>MAX_SEND_CACHE) {
        while (mSendCache.size()>(MAX_SEND_CACHE*3/4)) {
            CItem item = mSendCache.front();
            mSendCache.pop_front();
            if (item.mData) {
                free(item.mData);
                item.mData = NULL;
            	LOGE("【DR】CTXDataReportNetThread，DropSendCache!");
            }
        }
    }
}

int CTXDataReportNetThread::RecvResponse(char *pRecvData, int nRecvMaxLen) {
	int len = 0;
	{
		TXCScopedLock _l(mRecvLock);
		len = mRecvCache.query((char *) pRecvData, nRecvMaxLen);
	}

	return len;
}

int CTXDataReportNetThread::ColseServer() {
	{
		TXCScopedLock _l(mCloseLock);
		mbCloseNet = true;
	}

	return true;
}

void CTXDataReportNetThread::AddTimer(Timer timer) {
	TXCScopedLock _l(mEvtTimersLock);
	if (timer) mEvtTimers.push_back(timer);
}

void CTXDataReportNetThread::DeleteTimer(Timer timer) {
	TXCScopedLock _l(mEvtTimersLock);
	if (timer) {
		for (std::list<Timer>::iterator itr = mEvtTimers.begin(); itr != mEvtTimers.end(); ++itr) {
			if (timer == *itr) {
				mEvtTimers.erase(itr);
				break;
			}
		}
	}
}

bool CTXDataReportNetThread::threadLoop() {
	bool closeNet = false;
	{
		TXCScopedLock _l(mCloseLock);
		closeNet = mbCloseNet;
	}

	if (closeNet && CONN_STATE_CONNECTED == mSocketStatus) {
		close(mSocket);
		mSocket = -1;
		mSocketStatus = CONN_STATE_UNCONNECT;
		return true;
	}

	{
		TXCScopedLock _l(mEvtTimersLock);
		for (std::list<Timer>::iterator itr = mEvtTimers.begin(); itr != mEvtTimers.end(); ++itr) {
			(*itr)();
		}
	}


	CItem item;
	{
		TXCScopedLock _l(mSendLock);
        if (!mSendCache.empty()) {
            item = mSendCache.front();
            mSendCache.pop_front();
        }

	}

	if (!item.dataLen) {
		tx_rtmp_msleep(500);
		mnIdleTime += 1;

		if (mnIdleTime > 120 && mSocketStatus == CONN_STATE_CONNECTED) {// 持续空闲超过60s。
            LOGE("【DR】Idle time > 60s Close TCP");
			close(mSocket);
			mSocket = -1;
			mSocketStatus = CONN_STATE_UNCONNECT;
		}

		return true;
	}

	mnIdleTime = 0;

	if (mSocketStatus == CONN_STATE_UNCONNECT) {
		ConnectServer();
	}

	if (mSocketStatus != CONN_STATE_CONNECTED) {
        if (item.dataLen) {
            TXCScopedLock _l(mSendLock);
            mSendCache.push_front(item);
        }
        
		tx_rtmp_msleep(3000); // arvinwu 网络物理断开后，隔一段时间重试，不可太频繁 3秒比较合适。

        /*
        * 限制连接频率
        */
        if (mSendInterval > 0)
        {
            tx_rtmp_msleep(mSendInterval);
        }
		return true;
	}

	// 发送
	size_t r = 0;
	if ((r = send(mSocket, item.mData, item.dataLen, 0)) != item.dataLen) {
		// 对端reset了。 关闭重连
		close(mSocket);
		mSocket = -1;
		mSocketStatus = CONN_STATE_UNCONNECT;
        // 连续发送失败超过 mMaxResendTimes 次，放弃该上报数据。
        mResendTimes++;
        if (mMaxResendTimes == 0 || mResendTimes <= mMaxResendTimes)
        {
            TXCScopedLock _l(mSendLock);
            if (item.dataLen) {
                mSendCache.push_front(item);
            }
        }
        else
        {
            if (item.mData) {
                free(item.mData);
                item.mData = NULL;
            }
        }
	} else {
        mResendTimes = 0; // 发送成功复位

        if (item.mData) {
            free(item.mData);
            item.mData = NULL;
        }

		// arvinwu add
		// 有server回包，说明数据上报服务器收到。可以认为发送成功。
		// 回报数据 检查失败，也做成功返回。log信息关键，保持打开。
		// 上报路径 SDK --> Proxy --> Worker.
		// proxy 超载后会直接丢包，这样会引起 SDK 处理回包线程阻塞。因此需要设置超时时间。

		int result = 0;
		char rcvBuf[2048] = {0};
		result = recv(mSocket, rcvBuf, sizeof(rcvBuf), 0);

		if (result <= 0) {
			close(mSocket);
			mSocket = -1;
			mSocketStatus = CONN_STATE_UNCONNECT;
		} else {
			TXCScopedLock _l(mRecvLock);
			mRecvCache.append(rcvBuf, result);
		}
	}

    /*
     * 限制发送频率
     */
    if (mSendInterval > 0)
    {
        tx_rtmp_msleep(mSendInterval);
    }

	tx_rtmp_msleep(50);
	return true;
}
