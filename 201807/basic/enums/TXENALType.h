//
//  TXSNALType.h
//  basic
//
//  Created by alderzhang on 2017/5/17.
//  Copyright © 2017年 Tencent. All rights reserved.
//

#ifndef TXENALType_h
#define TXENALType_h

typedef enum
{
    TXE_NAL_TYPE_NONE    = -1,
    TXE_NAL_TYPE_IFRAME  = 0,        //I帧数据
    TXE_NAL_TYPE_PFRAME  = 1,        //P帧数据
    TXE_NAL_TYPE_BFRAME  = 2,        //B帧数据
    TXE_NAL_TYPE_SPS     = 3,        //SPS数据
    TXE_NAL_TYPE_PPS     = 4,        //PPS数据
    TXE_NAL_TYPE_VPS     = 5,        //VPS数据
    TXE_NAL_TYPE_SEI     = 6,        //SEI数据
}TXENALType;

#endif /* TXENALType_h */
