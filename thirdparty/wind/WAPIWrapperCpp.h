#ifndef _WAPIWRAPPERCPP_H
#define _WAPIWRAPPERCPP_H

#include <windows.h>
#include <comutil.h>

#ifdef WAPIWRAPPERCPP_EXPORTS
#define WAPIWRAPPERCPP_EXP __declspec(dllexport)
#else
#define WAPIWRAPPERCPP_EXP __declspec(dllimport)
#endif

//�ؼ�δע�����
const LONG noRegErr = -1;

enum ErrMsgLang //������Ϣ����
{
	eENG = 0,	///< Ӣ��
	eCHN,		///< ����
};

class WAPIWRAPPERCPP_EXP WindData
{
public:
	VARIANT	data;	///< ����
	VARIANT codes;	///< Code�б�
	VARIANT fields;	///< ָ���б�
	VARIANT times;	///< ʱ���б�
	LONG errorCode;	///< ������
public:

	WindData();
	~WindData();

	VOID InitWindData();
	VOID ClearWindData();

	//��ȡ������Ϣ
	LPCWSTR GetErrorMsg() const;

	//����ת������(��Ҫdelete)
	static WCHAR* DateToString(DATE date, LPCWSTR strFormat = L"%Y-%m-%d");
	static VOID FreeString(WCHAR*& pStr);

	//////////////////////////////////////////////////////////////////////////
	//����ʹ��

	INT GetCodesLength() const;
	INT GetFieldsLength() const;
	INT GetTimesLength() const;

	LPCWSTR GetCodeByIndex(int index) const;
	LPCWSTR GetFieldsByIndex(int index) const;
	BOOL GetTimeByIndex(int index, WCHAR timeBuffer[], int& length) const;
	DATE GetTimeByIndex(int index) const;

	//��ȡ���ݲ�ѯ�ӿ�OneData
	BOOL GetDataItem(int timeIndex, int codesIndex, int fieldsIndex, VARIANT& outItem) const;

	//��ȡ�����б�
	const DATE* GetTDaysInfo(LONG& lDateCount) const;

	//////////////////////////////////////////////////////////////////////////
	//����ʹ��

	INT GetRecordCount() const;

	//��ȡ���׽ӿ�OneData
	BOOL GetTradeItem(int recordIndex, int fieldsIndex, VARIANT& outItem) const;

	//��ȡ��¼ID
	LONG GetLogonID() const;

	//��ȡOrder����Id
	LONG GetOrderRequestID() const;

	//��ȡOrderNumber
	LPCWSTR GetOrderNumber(int recordIndex = 0) const;
};

// �ص���������
typedef LONG (WINAPI *WsqCallBack)(ULONGLONG reqId, const WindData &windData);

typedef LONG (WINAPI *TradeCallBack)(LONG LogonID, const WindData &windData);

// �����Ǵ� WAPIWrapperCpp.dll ������
class WAPIWRAPPERCPP_EXP CWAPIWrapperCpp {
public:
	
	//��������
	//////////////////////////////////////////////////////////////////////////
	//Wind�ӿ���������
	static LONG start(LPCWSTR options = NULL, LPCWSTR options2 = NULL, LONG timeout = 5000);

	//Wind�ӿ���ֹ����
	static LONG stop();

	//�ж�����״̬
	static LONG isconnected();

	//�� requestId Ϊ0 ȡ���������󣻷���ֻȡ������ID�ŵ�����
	static VOID cancelRequest(ULONGLONG requestId);

	//ȡ����������
	static VOID cancelAllRequest();

	//��ȡ��������Ӧ�Ĵ�����Ϣ
	static BOOL getErrorMsg(LONG errCode, ErrMsgLang lang, WCHAR msg[], int& /*inout*/msgLength);

	//////////////////////////////////////////////////////////////////////////

	//���ݺ���
	//////////////////////////////////////////////////////////////////////////
	//��ֵ����wsd�������������
	static LONG wsd(WindData& outWindData, LPCWSTR windCodes, LPCWSTR fields, LPCWSTR startTime = NULL, LPCWSTR endTime = NULL, LPCWSTR options = NULL);

	//��ֵ����wss�������ʷ����
	static LONG wss(WindData& outWindData, LPCWSTR windCodes, LPCWSTR fields, LPCWSTR options = NULL);

	//��ֵ����wsi����÷�������
	static LONG wsi(WindData& outWindData, LPCWSTR windCodes, LPCWSTR fields, LPCWSTR startTime, LPCWSTR endTime, LPCWSTR options = NULL);

	//��ֵ����wst�������������
	static LONG wst(WindData& outWindData, LPCWSTR windCodes, LPCWSTR fields, LPCWSTR startTime, LPCWSTR endTime, LPCWSTR options = NULL);

	//��ֵ����wsq�����ʵʱ����
	//�Ƕ���ģʽ��ȡһ���Կ�������
	static LONG wsq(WindData& outWindData, LPCWSTR windCodes, LPCWSTR fields, LPCWSTR options = NULL);

	//����ģʽ������ʵʱ���ݣ�����ͨ���ص���������
	static LONG wsq(ULONGLONG &reqId, LPCWSTR windCodes, LPCWSTR fields, WsqCallBack callBack, LPCWSTR options = NULL, BOOL updateAllFields = FALSE);

	//��ֵ����tdq�����ʵʱ����
	//����ģʽtd������ʵʱ���ݣ�����ͨ���ص���������
	static LONG tdq(ULONGLONG &reqId, LPCWSTR windCodes, LPCWSTR fields, WsqCallBack callBack, LPCWSTR options = NULL, BOOL updateAllFields = FALSE);

	//��ֵ����bbq�����ʵʱ����
	//�Ƕ���ģʽ��ȡһ���Կ�������
	static LONG bbq(WindData& outWindData, LPCWSTR windCodes, LPCWSTR fields, LPCWSTR options = NULL);

	//����ģʽ������ʵʱ���ݣ�����ͨ���ص���������
	static LONG bbq(ULONGLONG &reqId, LPCWSTR windCodes, LPCWSTR fields, WsqCallBack callBack, LPCWSTR options = NULL);

	//��ֵ����wset�����ָ�����ݼ�
	static LONG wset(WindData& outWindData, LPCWSTR reportName, LPCWSTR options = NULL);

	//��ֵ����edb����þ�������
	static LONG edb(WindData& outWindData, LPCWSTR windCodes, LPCWSTR startTime = NULL, LPCWSTR endTime = NULL, LPCWSTR options = NULL);

	//��ϱ�����
	static LONG wpf(WindData& outWindData, LPCWSTR portfolioName, LPCWSTR viewName, LPCWSTR options = NULL);

	//����ϴ�����
	static LONG wupf(WindData& outWindData, LPCWSTR portfolioName, LPCWSTR tradeDate, LPCWSTR windCodes, LPCWSTR quantity, LPCWSTR costPrice, LPCWSTR options = NULL);

	//֤��ɸѡ����
	static LONG weqs(WindData& outWindData, LPCWSTR planName, LPCWSTR options = NULL);

	//�����ա������ա������յ��������к���
	static LONG tdays(WindData& outWindData, LPCWSTR startTime, LPCWSTR endTime, LPCWSTR options = NULL);

	//�����ա������ա������յ�����ƫ�Ƽ���
	static LONG tdaysoffset(DATE& outDate, LPCWSTR startTime, LONG offset, LPCWSTR options = NULL);

	//�����ա������ա������յ�������������
	static LONG tdayscount(LONG& outCount, LPCWSTR startTime, LPCWSTR endTime, LPCWSTR options = NULL);
	//////////////////////////////////////////////////////////////////////////

	//���׺���
	//////////////////////////////////////////////////////////////////////////
	//�����˺ŵ�½
	static LONG tlogon(WindData& outWindData, LPCWSTR brokerID, LPCWSTR departmentID, LPCWSTR accountID, LPCWSTR password, LPCWSTR accountType, LPCWSTR options = NULL,TradeCallBack callBack=0);

	//�����˺ŵǳ�, 0:�ǳ�ȫ���ѵ�¼���˺�
	static LONG tlogout(INT longId=0);

	//�µ�
	static LONG torder(WindData& outWindData, LPCWSTR windCodes, LPCWSTR tradeSide, LPCWSTR orderPrice, LPCWSTR orderVolume, LPCWSTR options = NULL);
	static LONG torder(LONG& reqId, LPCWSTR windCodes, LPCWSTR tradeSide, DOUBLE orderPrice, LONG orderVolume, LPCWSTR options = NULL, WCHAR** pErrosMsg = NULL);

	 //����ȡ��
	//����
	static LONG tcovered(WindData& outWindData, LPCWSTR windCodes, LPCWSTR tradeSide, LPCWSTR orderVolume, LPCWSTR options = NULL);
	static LONG tcovered(LONG& reqId, LPCWSTR windCodes, LPCWSTR tradeSide, LONG orderVolume, LPCWSTR options = NULL, WCHAR** pErrosMsg = NULL);
	

	//���������ѯ
	static LONG tquery(WindData& outWindData, LPCWSTR qryCode, LPCWSTR options = NULL);

	//����
	static LONG tcancel(LPCWSTR orderNumber, LPCWSTR options = NULL);
	//////////////////////////////////////////////////////////////////////////

	//�ز⺯��
	//////////////////////////////////////////////////////////////////////////
	//�ز⿪ʼ
	static LONG bktstart(WindData& outWindData, LPCWSTR strategyName, LPCWSTR startDate, LPCWSTR endDate, LPCWSTR options = NULL);

	//�ز��ѯ
	static LONG bktquery(WindData& outWindData, LPCWSTR qrycode, LPCWSTR qrytime, LPCWSTR options = NULL);

	//�ز��µ�
	static LONG bktorder(WindData& outWindData, LPCWSTR tradeTime, LPCWSTR securityCode, LPCWSTR tradeSide, LPCWSTR tradeVol, LPCWSTR options = NULL);

	//�ز����
	static LONG bktend(WindData& outWindData, LPCWSTR options);

	//�鿴�ز�״̬
	static LONG bktstatus(WindData& outWindData, LPCWSTR options);

	//�ز��Ҫ
	static LONG bktsummary(WindData& outWindData, LPCWSTR bktID, LPCWSTR view, LPCWSTR options);

	//�ز�ɾ��
	static LONG bktdelete(WindData& outWindData, LPCWSTR bktID, LPCWSTR options);

	//���ز����б�
	static LONG bktstrategy(WindData& outWindData, LPCWSTR options);

	//////////////////////////////////////////////////////////////////////////
	//appӦ�����
	/**
	* @brief   ����������App����Ȩ����֤
	* @param   WindData & outWindData: ��������
	* @param   LPCTSTR appKey:	������������app�Ӵ�����վ��ȡΨһ��֤��Id
	* @param   LPCTSTR appSecret:����������app����Կ
	* @param   LPCTSTR options:��������
	* @return  LONG:	�ɹ�\������
	*/
	static LONG wappAuth(WindData& outWindData, LPCTSTR appKey, LPCTSTR appSecret, LPCTSTR options);

	/**
	* @brief   ����������App�ʹ��½���ͨѶ����
	* @param   WindData & outWindData:��������
	* @param   LPCTSTR type_id: communication type,include:message_put; message_get; app_version; app_fps; app_data_get; app_data_put
	* @param   LPCTSTR message:	communication content
	* @param   LPCTSTR options: ��������
	* @return  LONG:	�ɹ�\������
	*/
	static LONG wappMessage(WindData& outWindData, LPCTSTR type_id, LPCTSTR message, LPCTSTR options);

	//////////////////////////////////////////////////////////////////////////
};

#endif
