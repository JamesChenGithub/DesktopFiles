#ifndef tx_dr_api_h
#define tx_dr_api_h

struct stExtInfo
{
    char command_id_comment[256];
    char url[2048];
    bool report_common;
    bool report_status;
};
    
/*********************************辅助函数*********************************/

/**
 *  创建唯一标识一次数据上报的token
 */
void txCreateEventToken(char* pToken, int nMaxLen);
    
/*
 *  设置公共数据的值（手机信息 app信息等）
 **/
void txSetCommonValue(const char* pKey, const char* pValue);


/*******************************一般事件上报接口*******************************/
/**
 * 数据上报通用接口，
 * 优势 ： 可以全局使用，包括上层和C++层，都可以混合使用；适用于上报点分散于SDK各处的情况；
 * 劣势 ： 需要自己保存token。
 */
    
/**
 *  初始化一次事件上报
 */
void txInitEvent(const char* pToken, int nCommandId /* Event_ID */, int nModuleId, const stExtInfo& extInfo);

/**
 *  设置事件相关数据的值
 */
void txSetEventValue(const char* pToken, int nCommandId, const char* pKey, const char* pValue);
    
/**
 *  上报一次事件
 */
void txReportEvent(const char* pToken, int nCommandId);

/**
 * 获取区间上报间隔
 */
int txGetStatusReportInterval();
    
    
/******************************* 通用事件上报接口 *******************************/
/**
 * 统计DAU;
 */
void txReportDAU(int eventId);

void txReportDAU(int eventId, int errCode, const char* errInfo);


/******************************* 通用事件上报接口 *******************************/
/**
 * 异常事件通用上报-主要用于uls排查问题;
 */
void txReportEvt40003(char* url, int code, char* desc, char* msg_more);

/*******************************数据上报辅助类*******************************/
/**
 * 数据上报辅助类，
 * 优势 ： 可以避免使用者对token的管理，方便使用；
 * 劣势 ： 无法全局使用
 */

class CTXDataReportHelper {

public:
    /**
     *  初始化一次事件上报, 自动生成唯一标示的事件的token;
     */
    CTXDataReportHelper(int nCommandId, int nModuleId, const stExtInfo& extInfo);
   
    /**
     *  初始化一次事件上报, 外部传入唯一标示的事件的token;
     */
    CTXDataReportHelper(const char* pToken, int nCommandId, int nModuleId, const stExtInfo& extInfo);
    
    /**
     *
     */
    ~CTXDataReportHelper(){};
    
public:
    /**
     *  创建唯一标识一次数据上报的token
     */
    static void CreateEventToken(char* pToken, int nMaxLen);
    
    /*
     *  设置公共数据的值（手机信息 app信息等）
     **/
    void SetCommonValue(const char* pKey, const char* pValue);
    
    /**
     *  设置事件相关数据的值
     */
    void SetEventValue(const char* pKey, const char* pValue);
    
    void SetEventIntValue(const char *pKey, long nValue);
    
    /**
     *  上报一次事件
     */
    void ReportEvent();
    
private:
    char mToken[1024];
    
    int  mCommandId;
};

#endif /* tx_dr_api_h */
