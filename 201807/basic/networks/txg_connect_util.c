//
//  ConnectUtil.c
//  TXRTMPSDK
//
//  Created by arvinwu on 16/3/28.
//
//
#include "txg_connect_util.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#ifndef NO_ASSERT
#include "txg_log.h"
#endif

#define TEST_PACKET_SIZE 1000

int txf_nslookup(char* url, struct sockaddr_storage *ips, socklen_t *iplen, int* ipcount)
{
    if(ipcount == NULL) return -1;
    /* 从url 中解析hostname */
    char* p = NULL;
    p = strstr(url, "://");
    if (!p)
    {
        return -1;
    }

    p+=3;

    if (*p == 0)
    {
        return -2;
    }

    char * end = p + strlen(p);
    char * col = strchr(p, ':');
    char * ques = strchr(p, '?');
    char * slash = strchr(p, '/');

    char hostname[256] = {0};
    {
        int hostlen = 0;
        if (slash)
        {
            hostlen = slash - p;
        }
        else
        {
            hostlen = end - p;
        }

        if (col && col-p < hostlen)
        {
            hostlen = col - p;
        }

        if (hostlen < 256)
        {
            memcpy(hostname, p, hostlen);
            //LOGD("parse hostname:%s \n\n", hostname);
        }
    }

    struct addrinfo hints, *res, *res0;
    memset(&hints, 0, sizeof(hints));
    
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    int error;
    error = getaddrinfo(hostname, "http", &hints, &res0);
    if (error) {
        return -3;
        /*NOTREACHED*/
    }
    int count = 0;
    for (res = res0; res && count < *ipcount; res = res->ai_next, count++) {
        switch (res->ai_family) {
            case AF_INET:
            case AF_INET6:
                memcpy(&ips[count], res->ai_addr, res->ai_addrlen);
                iplen[count] = res->ai_addrlen;
                break;
            default:
                break;
        }
    }
    *ipcount = count;
    freeaddrinfo(res0);
    return 0;
    
    /* 查询域名 */
//    struct hostent* hptr = NULL;
//    hptr = gethostbyname(hostname);
//    if (hptr == NULL)
//    {
//        return -3;
//    }
//
//
//    struct in_addr ** pptr = NULL;
//    char ip[32] = {0};
//    struct in_addr addr;
//
//    LOGD("official hostname:%s \n", hptr->h_name);
//
//    switch(hptr->h_addrtype)
//    {
//        case AF_INET:
//        case AF_INET6:
//            pptr = (struct in_addr**)hptr->h_addr_list;
//            if (pptr == NULL)
//            {
//                return -4;
//            }
//
//            int i = 0;
//            for(; pptr[i] != NULL && i < *ipcount; i++)
//            {
//                memcpy(&ips[i], pptr[i], hptr->h_length);
//                memcpy(&addr, pptr[i], hptr->h_length);
//                inet_ntop(AF_INET, &addr, ip, sizeof(ip));
//                LOGD("address:%s\n", ip);
//            }
//            *ipcount = i;
//            break;
//        default:
//            break;
//    }
//    return 0;
}


int txf_test_band_width(int ip, short port, int band, int* rate, int* drop, int* rtt)
{
    /* 参数 检查 */
    if (band < 0 || band > 100000) {// 10Mbps
        return -1;
    }
    
    /* 创建 socket */
    int sockfd;
    sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1) {
#ifndef NO_ASSERT
        LOGE("socket create failed!\n");
#endif
        return -2;
    }
    

    /* 设置 socket 属性*/
    int ret = 0;
    int flags = fcntl(sockfd, F_GETFL, 0);
    ret = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    if (ret == -1) {
        close(sockfd);
#ifndef NO_ASSERT
        LOGE("set O_NONBLOCK failed!\n");
#endif
        return -3;
    }
    
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);
    memset((char*)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_addr.s_addr = ip;
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;

    struct in_addr in;
    memcpy(&in, &ip, 4);
#ifndef NO_ASSERT
    LOGI("ip:%0x port:%d\n", ip, port);
#endif
    
    /* 绑定到本地端口 9000 */
    struct sockaddr_in local_addr;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(9000);
    local_addr.sin_family = AF_INET;
    if (bind(sockfd, &local_addr, sizeof(local_addr)) < 0)
    {
#ifndef NO_ASSERT
        LOGE("bind failed!\n");
#endif
        close(sockfd);
        return -4;
    }

    /* 发送数据 */
    char sendbuf[TEST_PACKET_SIZE];                 // 发送大小 1K
    int  sendlen = TEST_PACKET_SIZE;
    char recvbuf[TEST_PACKET_SIZE];                 // 接收大小 1K
    int  recvlen = TEST_PACKET_SIZE;
    int sendcount = band/8;             // 发送次数
    
    int sendinterval = 1000/sendcount;  // 发送间隔 单位ms
#ifndef NO_ASSERT
    LOGI("unit: %d bytes, count: %d, interval: %d ms\n", TEST_PACKET_SIZE, sendcount, sendinterval);
#endif
    int packet_idx = 0;
    
    fd_set  fds;
    struct timeval timeout = {0};
    struct timeval timeTS = {0};
    struct timeval timeNow = {0};

    /* 测试开始的时间*/
    struct timeval timeB ={0};
    gettimeofday(&timeB, NULL);    
    struct timeval timeE ={0};

    int recvcount = 0;
    int rttsum = 0;
    int doneflag = 0;
    int timec = 0;

    do {
        
        /* packet seq*/
        packet_idx ++;

        /* 总共发sendcount KBytes 测试数据*/
        if (packet_idx <= sendcount)
        {               
            /* 发送 测试 包   packet = dwPacketLen + dwPacketSeq + u64Ts + ...*/
            sendbuf[0] = 0x4;  /* 服务器要求开头字节和结尾字节不能取值为 0x2 或 0x3 */
            sendbuf[TEST_PACKET_SIZE-1] = 0x4;

            int *p =(int*)sendbuf; 
            *p = htonl(TEST_PACKET_SIZE);   /* 服务器要求的，packet length*/
            p++;

            *p = htonl(packet_idx);   /* packet seqence */
            p++;

            gettimeofday(&timeTS, NULL);   /* packet timestamp */
            *p = htonl(timeTS.tv_sec);
            p++;
            *p = htonl(timeTS.tv_usec);
            
            int r = sendto(sockfd, sendbuf, sendlen, 0, &server_addr, server_addr_len);
            if (r == -1) {
                close(sockfd);
#ifndef NO_ASSERT
                LOGE("send data failed! errno:%d\n", errno);
#endif
                return -5;
            }
        }

        
        /* 看看有没有可读的 */
        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);
        timeout.tv_sec = 0;
        timeout.tv_usec = sendinterval*1000;


        if (select(sockfd + 1, &fds, NULL, NULL, &timeout) <= 0) {
            usleep(1000);
            // printf("select timeout\n");
            continue;
        }
        
        if (FD_ISSET(sockfd, &fds)) {
            memset(recvbuf, 0, recvlen);
            int r = recvfrom(sockfd, recvbuf, recvlen, 0, &server_addr, &server_addr_len);
            /* 解析收到的 echo 包*/
            if (r == -1)
            {
#ifndef NO_ASSERT
                LOGE(" recv data failed! errno: %d\n", errno );
#endif
                close(sockfd);
                return -6;
            }

            gettimeofday(&timeNow, NULL);
            int* p = (int*)recvbuf;
            p++;
            int seq = ntohl(*p);
            p++;
            int sec = ntohl(*p);
            p++;
            int usec = ntohl(*p);

            int rtt =  (timeNow.tv_sec - sec)*1000000LL + timeNow.tv_usec - usec;

            rttsum += rtt;
            recvcount ++;

            // printf("recvfrom ret:%d, seq %d, rtt %u \n", r, seq, rtt );
            if (seq == sendcount)
            {
#ifndef NO_ASSERT
                LOGW("recv done!\n");
#endif
                doneflag = 1;
                break;
            }
        }

        gettimeofday(&timeE, NULL);
        timec = (timeE.tv_sec - timeB.tv_sec)*1000000LL + timeE.tv_usec - timeB.tv_usec;
#ifndef NO_ASSERT
        LOGI("timec %u\n", timec);
#endif
    } while( (packet_idx < sendcount) || (timec < 1200000));

    if (recvcount == 0)
    {
        close(sockfd);
        return -7; /* 服务器没有任何回包，可以认为server可能没有部署测速 */
    }
    
    if (doneflag == 0)
    {
        close(sockfd);
        return -8; /* 没有收到最后一个分组，表明数据没有收完， 可以减少测量数据再试一次 */
    }

    /* 测试结束时间 */
    gettimeofday(&timeE, NULL); 
    int timeuse = (timeE.tv_sec - timeB.tv_sec)*1000000LL + timeE.tv_usec - timeB.tv_usec;
#ifndef NO_ASSERT
    LOGI("\nsend %d kb, recv %d kb, avgrtt %d us, use time %u us\n", sendcount*8, recvcount*8, rttsum/recvcount, timeuse);
#endif
    
    /* 统计并释放资源 */
    * rate = recvcount*8*1000000LL/timeuse;         /* 用实际接收的数据来计算带宽*/
    * drop = (sendcount-recvcount)*1000/sendcount;
    * rtt = rttsum/recvcount;
    close(sockfd);
    return 0;
}


