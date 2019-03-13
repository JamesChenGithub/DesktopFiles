#include <stdio.h>
#include "tx_dr_api.h"
#include "tx_dr_def.h"
#include "tx_dr_base.h"
#include "txg_verinfo.h"
#include "txg_log.h"

extern int CreateToken(char *pToken, int nTokenMaxLen);

extern void TXDRSetCommonInfo();

void txInitEvent(const char *pToken, int nCommandId /* Event_ID */, int nModuleId,
				 const stExtInfo &extInfo) {
	TXDRSetCommonInfo();

	char strModuleId[512] = {0};
	snprintf(strModuleId, sizeof(strModuleId) - 1, "%d", nModuleId);

	CTXDataReportBase::GetInstance().SetEventValue(pToken, nCommandId, DR_KEY_MODULE_ID,
												   strModuleId);

	CTXDataReportBase::GetInstance().SetEventValue(pToken, nCommandId, DR_KEY_STREAMURL,
												   extInfo.url);
    
    if (extInfo.report_common == false) {
        CTXDataReportBase::GetInstance().SetEventValue(pToken, nCommandId, DR_KEY_REPORTCOMMON, "no");
    }
    
    if (extInfo.report_status == true) {
        CTXDataReportBase::GetInstance().SetEventValue(pToken, nCommandId, DR_KEY_REPORTSTATUS, "yes");
    }
    
//    CTXDataReportBase::GetInstance().SetEventValue(pToken, nCommandId, DR_KEY_STREAMURL, "rtmp://2157.livepush.myqcloud.com/live/2157_6e9051a415dd11e6b91fa4dcbef5e35a?bizid=2157");

	CTXDataReportBase::GetInstance().SetCommonValue(DR_KEY_EVT_COMM,
													extInfo.command_id_comment);

	CTXDataReportBase::GetInstance().SetCommonValue(DR_KEY_SDK_VER, txf_get_sdk_version());

	char strSdkId[256] = {0};
	snprintf(strSdkId, sizeof(strSdkId) - 1, "%d", txf_get_sdk_id());
	CTXDataReportBase::GetInstance().SetCommonValue(DR_KEY_SDK_ID, strSdkId);
}

void txSetCommonValue(const char *pKey, const char *pValue) {
	CTXDataReportBase::GetInstance().SetCommonValue(pKey, pValue);
}

void
txSetEventValue(const char *pToken, int nCommandId, const char *pKey, const char *pValue) {
	CTXDataReportBase::GetInstance().SetEventValue(pToken, nCommandId, pKey, pValue);
}

void txSetEventIntValue(const char *pToken, int nCommandId, const char *pKey, long nValue) {
	char strValue[2048] = {0};
	snprintf(strValue, sizeof(strValue) - 1, "%ld", nValue);
	txSetEventValue(pToken, nCommandId, pKey, strValue);
}

void txReportEvent(const char *pToken, int nCommandId) {
	CTXDataReportBase::GetInstance().ReportEvtGenaral(pToken, nCommandId);
}

int txGetStatusReportInterval() {
    return CTXDataReportBase::GetInstance().GetStautsReportInterval();
}

void txCreateEventToken(char *pToken, int nMaxLen) {
	CreateToken(pToken, nMaxLen);
}

void txReportDAU(int eventId){
    txReportDAU(eventId, 0, NULL);
}

void txReportDAU(int eventId, int errCode, const char *errInfo) {
	int module_id = MODULE_PUSH_SDK;
	int command_id = COMMAND_ID_DAU;

	char token[512] = {0};
	txCreateEventToken(token, sizeof(token));

	stExtInfo extInfo = {0};
	strncpy(extInfo.command_id_comment, COMMAND_ID_COMMENT_DAU_49999, sizeof(COMMAND_ID_COMMENT_DAU_49999) - 1);
    extInfo.report_common = true;
    
	txInitEvent(token, command_id, module_id, extInfo);

	char pValue[128] = {0};
	snprintf(pValue, sizeof(pValue) - 1, "%d", eventId);
	txSetEventValue(token, command_id, DR_KEY_DAU_EVENT_ID, pValue);

	char pErrCode[128] = {0};
	snprintf(pErrCode, sizeof(pErrCode) - 1, "%d", errCode);
	txSetEventValue(token, command_id, DR_KEY_DAU_ERR_CODE, pErrCode);

	txSetEventValue(token, command_id, DR_KEY_DAU_ERR_INFO, errInfo);

	txSetEventValue(token, command_id, DR_KEY_DAU_EXT, "");

	txReportEvent(token, command_id);
    
    LOGD("【DR】DAU report, eventid=%d, errCode=%d, token=%s", eventId, errCode, token);
}

void txReportEvt40003(char* url, int code, char* desc, char* msg_more)
{
    int module_id = MODULE_PUSH_SDK;
    int command_id = COMMAND_ID_40003;
    
    char token[512] = {0};
    txCreateEventToken(token, sizeof(token));
    
    stExtInfo info = {0};
    snprintf(info.url,2048,"%s",url);
    info.report_common = false;
    
    txInitEvent(token, command_id, module_id, info);
    
    char ErrCode[128] = {0};
    snprintf(ErrCode, sizeof(ErrCode), "%d", code);
    txSetEventValue(token, command_id, DR_KEY_ERROR_CODE, ErrCode);
    
    txSetEventValue(token, command_id, DR_KEY_ERROR_MSG, desc);
    
    txSetEventValue(token, command_id, DR_KEY_MSG_MORE, msg_more);
    
    txReportEvent(token, command_id);
    
    LOGD(" 【DR】40003 report,url=%s, code=%d, msg=%s, msg_more=%s",url, code, desc, msg_more);
}

#pragma CTXDataReportHelper

CTXDataReportHelper::CTXDataReportHelper(int nCommandId, int nModuleId,
										 const stExtInfo &extInfo)
		: mCommandId(nCommandId) {
	txCreateEventToken(mToken, sizeof(mToken));

	txInitEvent(mToken, nCommandId, nModuleId, extInfo);
}

CTXDataReportHelper::CTXDataReportHelper(const char *pToken, int nCommandId, int nModuleId,
										 const stExtInfo &extInfo)
		: mCommandId(nCommandId) {
	if (!pToken) strncpy(mToken, pToken, strlen(pToken));

	txInitEvent(mToken, nCommandId, nModuleId, extInfo);
}

void CTXDataReportHelper::CreateEventToken(char *pToken, int nMaxLen) {
	txCreateEventToken(pToken, nMaxLen);
}

void CTXDataReportHelper::SetCommonValue(const char *pKey, const char *pValue) {
	txSetCommonValue(pKey, pValue);
}

void CTXDataReportHelper::SetEventValue(const char *pKey, const char *pValue) {
	txSetEventValue(mToken, mCommandId, pKey, pValue);
}

void CTXDataReportHelper::SetEventIntValue(const char *pKey, long nValue) {
	char strValue[2048] = {0};
	snprintf(strValue, sizeof(strValue) - 1, "%ld", nValue);
	SetEventValue(pKey, strValue);
}

void CTXDataReportHelper::ReportEvent() {
	txReportEvent(mToken, mCommandId);
}
