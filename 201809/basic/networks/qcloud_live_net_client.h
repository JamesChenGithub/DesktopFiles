//
//  qcloud_live_net_client.h
//  saturn
//
//  Created by shockcao on 17/5/3.
//
//

#ifndef qcloud_live_net_client_h
#define qcloud_live_net_client_h

#include <stdint.h>
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace qcloud {
    
class QcloudLiveSyncNetClient {
    
public:
    virtual ~QcloudLiveSyncNetClient() {}
    
    virtual int Connect(const char* ip, uint16_t port, int64_t timeout) = 0;    //timeout ms

    virtual int ComplexConnect(const std::vector<std::string>& ipport_list, int64_t timeout) = 0;   //timeout ms

    virtual int Write(char* buffer, int length, int64_t timeout) = 0;   //timeout ms
    
    virtual int Recv(char* buffer, int read_len, int64_t timeout) = 0;  // timeout ms
    
    virtual int64_t EstimateBandWidth() = 0;    //0 for tcp | bandwidth for quic bytes/s
    
    virtual uint64_t BytesInFlight() = 0;       //nwrite for tcp | unacked bytes for quic_connection, not for client/stream
    
    virtual std::string GetConnectionStats() = 0; //
    
    virtual void CloseConn() = 0;
    
    virtual void GetStatisticInfo(std::map<std::string, std::string> & mapInfo) = 0;
    
    virtual void GetErrorNo(uint64_t& connection_id, uint32_t& stream_id, int& close_reason) = 0;
};
    
class AsyncNetClientCallBack {

public:
    virtual ~AsyncNetClientCallBack() {}
    
    virtual void OnConnectEnd(int error_code, uint64_t connection_id = 0, uint32_t stream_id = 0) = 0;
    
    virtual void OnWriteReady(char** buf, int& buf_len) = 0;
    
    virtual void OnDataAvailable(char* buf, int buf_len) = 0;
    
    virtual void OnWriteComplete(int buf_len) = 0;
    
    virtual void OnStatisticInfo(std::map<std::string, std::string> & mapInfo) = 0;
    
    virtual bool OnClosed(int close_reason, int source, uint64_t connection_id = 0, uint32_t stream_id = 0) = 0;
};

class QcloudLiveAsyncNetClient {

public:
    virtual ~QcloudLiveAsyncNetClient() {}
    
    virtual void Connect(const char* ip, uint16_t port) = 0;
    
    virtual void ComplexConnect(const std::vector<std::string>& ipport_list) = 0;
    
    virtual void TriggerWrite() = 0;
    
    virtual void CloseConn() = 0;
};
    
}



#endif /* qcloud_live_net_client_h */
