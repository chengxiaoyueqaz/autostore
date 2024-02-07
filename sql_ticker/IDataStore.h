#include "iservice.h"
#include "DataTypes.h"
#pragma once
#include <memory>

#ifdef _WIN64
#if defined(_UNICODE) || defined(UNICODE)
#undef _UNICODE
#undef UNICODE
#define OTL_ODBC_MSSQL_2008
#define OTL_EXCEPTION_IS_DERIVED_FROM_STD_EXCEPTION
#include "otlv4.h"
#endif
#define _UNICODE
#define UNICODE
#else
//�ڲ�ͬ��ϵͳ�ϣ������
#include "otlv4.h"
#endif

using namespace std;
//�̳���IService
class SERVICE_API IDatastore
{
public:
	IDatastore(void);
	virtual ~IDatastore(void);


	virtual bool login(const string& user_name, const string& password, const ServerInfo& bs);

	virtual bool logout();

	virtual bool request(int request_id, void* param);

	virtual  bool isLogined() { return m_bIsLogined; };
	//1.�洢���飨��Ƶ��
	virtual bool StoreMarketData(MarketData* pMarketData);
	//2.�洢�����ʽ����򣨵��ã�
	virtual bool StoreFundsFlowMarketData(FundsFlowMarketData* pFundsFlowMarketData);
	//3,�洢�ʽ���Ϣ�����ã���Ƶ��
	virtual bool StoreTradingAccount(TradingAccount* pTradingAccount);
	//�洢Ͷ���ֲ֣߳����ã���Ƶ��
	virtual bool StorePosition(Position* pPosition);
	//�洢��תծ��Ϣ����Ƶ��һ��һ�Σ�
	virtual bool StoreConversionBondInfo(ConversionBondInfo* pConversionBondInfo);
	//�洢��תծ�����Ϣ����Ƶ��һ��һ�Σ�
	virtual bool StorePrematurityRepoOrder(PrematurityRepoOrder* pPrematurityRepoOrder);
	//�洢��ʷ������Ϣ���ص�����Ƶ��
	virtual bool StoreHistoryTrade(HistoryTrade* pHistoryTrade);
	//�洢������Ϣ
	virtual bool StoreReturnTrade(ReturnOrder *pRturnOrder);
	//�洢��¼�����û���Ϣ
	virtual bool StoreUserInfo(UserInfo* pUserInfo);
	//�洢��������Ϣ
	virtual bool StoreExhchange(Exchange* pExchange);
	//�洢������Ϣ
	virtual bool StoreInsertOrder(InputOrder* inputorder);
	//�洢������Ϣ
	virtual bool StoreInsertOrderAction(InputOrderAction* inputorderaction);




	//ȡ����
	virtual bool LoadMarketData(vector<MarketData>& out_data, int start_time, int end_time);
	//2.�洢�����ʽ����򣨵��ã�
	virtual bool LoadFundsFlowMarketData(vector<FundsFlowMarketData>& out_data);
	//3,�洢�ʽ���Ϣ�����ã���Ƶ��
	virtual bool LoadTradingAccount(vector<TradingAccount>& out_data, int date);
	//�洢Ͷ���ֲ֣߳����ã���Ƶ��
	virtual bool LoadPosition(vector<Position>& out_data);
	//�洢��תծ��Ϣ����Ƶ��һ��һ�Σ�
	virtual bool LoadConversionBondInfo(vector<ConversionBondInfo>& out_data, int date);
	//�洢��תծ�����Ϣ����Ƶ��һ��һ�Σ�
	virtual bool LoadPrematurityRepoOrder(vector<PrematurityRepoOrder>& out_data, int date);
	//�洢��ʷ������Ϣ���ص�����Ƶ��
	virtual bool LoadHistoryTrade(vector<HistoryTrade>& out_data, int date);
	//ȡ��������Ϣ
	virtual bool LoadReturnTrade(vector<ReturnOrder> &out_data,int date);
	//ȡ��������Ϣ
	virtual bool LoadInsertOrder(InputOrder* inputorder);


	virtual string get_user() { return user; }


	virtual ServerInfo get_server() {return m_ServerInfo;}


private:
	otl_connect sql_server;
	bool m_bIsLogined;
	ServerInfo m_ServerInfo;
	string user;
};

class SERVICE_API CDataStore :public IDatastore
{
};