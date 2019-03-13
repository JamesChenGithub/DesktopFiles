#ifndef TXSNALPacket_hpp
#define TXSNALPacket_hpp
#if     defined(ANDROID)            // Android
#include <TXENALType.h>
#elif   defined(__MACH__)           // iOS
#include "TXENALType.h"
#endif
#include <string>


typedef struct tagTXSNALPacket
{
    TXENALType              nalType;
    std::string             nalData;
    uint64_t                gopIndex;
    uint64_t                gopFrameIndex;
    uint64_t                frameIndex;
    uint64_t                refFrameIndex;
    uint64_t                pts;
    uint64_t                dts;
}TXSNALPacket_HPP;

#endif // TXSNALPacket_hpp
