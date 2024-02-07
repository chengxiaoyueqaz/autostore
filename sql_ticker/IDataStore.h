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
//在不同的系统上，定义宏
#include "otlv4.h"
#endif

using namespace std;
//继承自IService
class SERVICE_API IDatastore
{
public:
	IDatastore(void);
	virtual ~IDatastore(void);


	virtual bool login(const string& user_name, const string& password, const ServerInfo& bs);

	virtual bool logout();

	virtual bool request(int request_id, void* param);

	virtual  bool isLogined() { return m_bIsLogined; };
	//1.存储行情（高频）
	virtual bool StoreMarketData(MarketData* pMarketData);
	//2.存储当日资金流向（调用）
	virtual bool StoreFundsFlowMarketData(FundsFlowMarketData* pFundsFlowMarketData);
	//3,存储资金信息（调用，中频）
	virtual bool StoreTradingAccount(TradingAccount* pTradingAccount);
	//存储投资者持仓（调用，中频）
	virtual bool StorePosition(Position* pPosition);
	//存储可转债信息（低频，一天一次）
	virtual bool StoreConversionBondInfo(ConversionBondInfo* pConversionBondInfo);
	//存储可转债赎回信息（低频，一天一次）
	virtual bool StorePrematurityRepoOrder(PrematurityRepoOrder* pPrematurityRepoOrder);
	//存储历史交易信息（回调，低频）
	virtual bool StoreHistoryTrade(HistoryTrade* pHistoryTrade);
	//存储交易信息
	virtual bool StoreReturnTrade(ReturnOrder *pRturnOrder);
	//存储登录过的用户信息
	virtual bool StoreUserInfo(UserInfo* pUserInfo);
	//存储交易所信息
	virtual bool StoreExhchange(Exchange* pExchange);
	//存储报单信息
	virtual bool StoreInsertOrder(InputOrder* inputorder);
	//存储撤单信息
	virtual bool StoreInsertOrderAction(InputOrderAction* inputorderaction);




	//取数据
	virtual bool LoadMarketData(vector<MarketData>& out_data, int start_time, int end_time);
	//2.存储当日资金流向（调用）
	virtual bool LoadFundsFlowMarketData(vector<FundsFlowMarketData>& out_data);
	//3,存储资金信息（调用，中频）
	virtual bool LoadTradingAccount(vector<TradingAccount>& out_data, int date);
	//存储投资者持仓（调用，中频）
	virtual bool LoadPosition(vector<Position>& out_data);
	//存储可转债信息（低频，一天一次）
	virtual bool LoadConversionBondInfo(vector<ConversionBondInfo>& out_data, int date);
	//存储可转债赎回信息（低频，一天一次）
	virtual bool LoadPrematurityRepoOrder(vector<PrematurityRepoOrder>& out_data, int date);
	//存储历史交易信息（回调，低频）
	virtual bool LoadHistoryTrade(vector<HistoryTrade>& out_data, int date);
	//取出交易信息
	virtual bool LoadReturnTrade(vector<ReturnOrder> &out_data,int date);
	//取出报单信息
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