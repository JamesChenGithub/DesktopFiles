//
//  ConnectUtil.h
//  TXRTMPSDK
//
//  Created by arvinwu on 16/3/28.
//
//

#ifndef ConnectUtil_h
#define ConnectUtil_h
#include <sys/socket.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
    /*
    * url       :   【入参】rtmpurl
    * ips       :   【出参】地址解析得到的ip列表
    * ips       :   【出参】地址解析得到的ip长度列表     
    * ipcount   :   【入参出参】地址解析得到的ip个数
    * return    : error code.
    */
    int txf_nslookup(char* url, struct sockaddr_storage *ips, socklen_t *iplen, int* ipcount);



    /*
     * ip   : 【入参】待测试的服务器ip
     * port : 【入参】待测试的服务器端口
     * band : 【入参】发送带宽      单位Kbps
     * times: 【入参】测试次数
     * rate : 【出参】实际速率      单位Kbps
     * drop : 【出参】丢包率       百分比乘以 1000
     * rtt  : 【出参】往返时延      单位ms
     * return: error code.
     */
    int txf_test_band_width(int ip, short port, int band, int* rate, int* drop, int* rtt);

#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* ConnectUtil_h */
