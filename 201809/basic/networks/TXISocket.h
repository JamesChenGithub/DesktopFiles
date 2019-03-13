//
// Created by alderzhang on 2017/6/29.
//

#ifndef LITEAV_TXISOCKET_H
#define LITEAV_TXISOCKET_H

#include <string>
#include <netinet/tcp.h>

#define SE_BROKEN	-1		//发生不可挽回的错误
#define SE_INTR		-2		//被中断事件打断
#define SE_TIMEOUT	-3		//操作超时
#define SE_REFUSE	-4		//远端主动断开连接

#if defined(__APPLE__) && !defined(tcp_info)
#define tcp_info tcp_connection_info
#define TCP_INFO TCP_CONNECTION_INFO
#endif

class TXISocketCallback{
public:
	virtual void onConnect(bool success) = 0;

	virtual void onSendable() = 0;

	virtual void onRecv(void *buffer, int length) = 0;
};

class TXISocket {
public:
	/**
	 * 构造函数
	 * @param callback 异步回调对象，传入nullptr则使用同步模式
	 */
	TXISocket(TXISocketCallback *callback = nullptr)
			: _callback(callback){

	}
    
    virtual ~TXISocket(){};

public:
	/**
	 * 检查是否已连接
	 * @return 是否已连接
	 */
	virtual bool isConnected() = 0;

	/**
	 * 检查网络是否处于可发送状态
	 * @return 是否可发送（同步模式下总是返回true）
	 */
//	virtual bool isSendable() = 0;

	/**
	 * 连接到主机
	 * @param host		主机名称或地址
	 * @param port 		主机端口
	 * @param timeout 	连接超时时间，单位毫秒，-1表示永不超时
	 * @return 是否连接成功（异步模式下总是返回true）
	 */
	virtual bool connect(char * host, short port, long timeout = 60000) = 0;

	/**
	 * 关闭连接
	 * @return 是否成功（异步模式下总是返回true）
	 */
	virtual bool close() = 0;

	/**
	 * 发送数据到远端（异步模式下放入发送缓冲区等待发送）
	 * @param buffer	要发送的数据缓冲区
	 * @param size		要发送的数据大小
	 * @return 发送字节数（异步模式下为存放到发送缓冲区的字节数）
	 * 		SE_BROKEN		出现无法恢复的错误
	 * 		SE_INTR			被中断打断
	 * 		SE_TIMEOUT		发送超时
	 * 		SE_REFUSE		远端主动断开连接
	 */
	virtual int send(const void *buffer, unsigned int size) = 0;

	/**
	 * 从远端接收数据（异步模式下无效）
	 * @param buffer	存放接收数据的缓冲区
	 * @param size		缓冲区大小
	 * @return 接收字节数
	 * 		SE_BROKEN		出现无法恢复的错误
	 * 		SE_INTR			被中断打断
	 * 		SE_TIMEOUT		接收超时
	 * 		SE_REFUSE		远端主动断开连接
	 */
	virtual int recv(void *buffer, unsigned int size) = 0;

	/**
	 * 设置发送超时时间（异步模式下无效）
	 * @param timeout	发送超时时间，单位毫秒，0表示取消超时设置，-1表示恢复上次设置
	 * @return 设置是否成功
	 */
	virtual bool setSendTimeout(long timeout) = 0;

	/**
	 * 设置接收超时时间（异步模式下无效）
	 * @param timeout	接收超时时间，单位毫秒，0表示取消超时设置，-1表示恢复上次设置
	 * @return 设置是否成功
	 */
	virtual bool setRecvTimeout(long timeout) = 0;
    
    /**
     * 获取错误码及错误描述信息
     * @param error 出参，错误码
     * @param description 出参，错误描述信息
     */
    virtual void getLastError(int* error, char** description) = 0;

	/**
	 * 获取nwrite
	 * @param nwriteLen
	 * @return
	 */
	virtual int getWriteBufLength(int * nwriteLen) = 0;

    /**
     * 获取TCP_INFO
     */
    virtual int getTcpInfo(tcp_info *info) = 0;

private:
	TXISocketCallback *_callback;

protected:
	void onConnect(bool success){
		if(_callback){
			_callback->onConnect(success);
		}
	}

	void onSendable(){
		if(_callback){
			_callback->onSendable();
		}
	}

	void onRecv(void *buffer, int length){
		if(_callback){
			_callback->onRecv(buffer, length);
		}
	}
};


#endif //LITEAV_TXISOCKET_H
