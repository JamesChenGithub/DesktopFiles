//
//  qcloud_live_net_client_context.h
//  saturn
//
//  Created by shockcao on 17/5/3.
//
//

#ifndef qcloud_live_net_client_context_h
#define qcloud_live_net_client_context_h

#include <vector>
#include <stdint.h>
#include <string>

#include "qcloud_live_net.h"

namespace qcloud {

class QcloudLiveSyncNetClient;
class QcloudLiveAsyncNetClient;
class AsyncNetClientCallBack;
    
class QcloudLiveNetClientContext {
    
public:
    struct Params{
        Params();
        Params(const Params& other);
        ~Params();
    
        bool require_confirm_for_connect;
        bool head_of_blocking;
        
        int cert_verify_flags;
        
        std::vector<uint32_t> quic_connection_options;
        
        bool migrate_sessions_on_network_change;
        bool quic_do_not_fragment;
        
        int idle_connection_timeout_seconds;
        int idle_connection_timeout_before_handshake_seconds;
        int handshake_timeout_seconds;
        
        int custom_quic_max_packet_size;
        int custom_quic_max_unsend_size;
        
        int once_write_max_len;
        int once_read_max_len;
        
        bool use_disk_cache;
        std::string app_file_path;
        
        bool keep_connection_alive;
        int ping_timeout;   //ms
    };
    
    explicit QcloudLiveNetClientContext(const Params& context_params);
    virtual ~QcloudLiveNetClientContext();
    
    static void SetLogMessageHandler(qcloud::LogMessageHandlerFunction handler);
    static void SetLogLevel(LogLevel log_level);
    
    bool Initialize();

    QcloudLiveSyncNetClient* CreateSyncNetClient(bool is_tcp = false, bool nodelay = false);
    void ReleaseSyncNetClient(QcloudLiveSyncNetClient* client);
    
    QcloudLiveAsyncNetClient* CreateAsyncNetClient(AsyncNetClientCallBack* callback, bool is_tcp = false, bool nodelay = false);
    void ReleaseAsyncNetClient(QcloudLiveAsyncNetClient* client);
    
    void CloseAlivingConnnection(const char* ip, uint16_t port);
    
private:
    bool __TearDown();
    
    class ContextImpl;
    ContextImpl* context_impl_;
    
    // DISALLOW_COPY_AND_ASSIGN
    QcloudLiveNetClientContext(const QcloudLiveNetClientContext&);
    void operator=(const QcloudLiveNetClientContext&);
};
    
}

#endif /* qcloud_live_net_client_context_h */
