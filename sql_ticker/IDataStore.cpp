#pragma once
#include "pch.h"
#include "ToraService.h"
#include "IDataStore.h"
#include"Datatypes.h"
#include"ServerInfo.h"




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
#include <atlcomcli.h>


void insert(otl_connect& db);
void select_data(otl_connect& db);
void trim(string& s);

IDatastore::IDatastore(void)
{
	otl_connect::otl_initialize();
	m_bIsLogined = false;
}

IDatastore::~IDatastore(void)
{

}

bool IDatastore::login(const string& user_name, const string& password, const ServerInfo& bs)
{
	try
	{
		char szConnStr[128] = { 0 };
		sprintf(szConnStr, "driver={%s};server=LAPTOP-KJMF4JRP\\SQLEXPRESS;uid=%s;pwd=%s;DSN=%s", bs.protocol.c_str(), user_name.c_str(), password.c_str(), bs.name.c_str());
		//const string connect_str = "drvier={Sql Server};server=LAPTOP-KJMF4JRP\\SQLEXPRESS;uid=czymm;pwd=160829;DSN=Multi";
		sql_server.rlogon(szConnStr);
		////测试
		//otl_cursor::direct_exec(sql_server, "create table tb88(InstrumentID varchar(32),InstrumentName varchar(64),PriceTick float)", otl_exception::enabled); // droptable
		//insert(sql_server);
		//select_data(sql_server);
		//sql_server.logoff();
		otl_cursor::direct_exec
		(
			sql_server,
			(string("use ") + bs.id).c_str(),
			otl_exception::enabled // disable OTL exceptions
		);
		m_ServerInfo = bs;
		user = bs.name;
	}
	catch (otl_exception & p)
	{
		//抛出异常表示失败	
		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);
	}

	m_bIsLogined = true;
	return m_bIsLogined;
}

bool IDatastore::logout()
{
	if (isLogined())
	{
		sql_server.logoff();
		return true;
	}
	return false;
}

bool IDatastore::request(int request_id, void* param)
{
	return true;
}


bool IDatastore::StoreMarketData(MarketData* pMarketData)
{
	if (!m_bIsLogined) {
		return false;
	}
	if (NULL == pMarketData) {
		return false;
	}
	try
	{
		otl_cursor::direct_exec
		(
			sql_server,
			("use " + m_ServerInfo.id).c_str(),
			otl_exception::enabled // disable OTL exceptions
		); 
	//建表;
		string table_name;
        table_name=pMarketData->SecurityName;
		trim(table_name);
		string create_table_sql =
			"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[" + table_name + "]') AND type in (N'U')) "
			"create table " + table_name + "(TradingDay varchar(9),"
			"SecurityName varchar(41),Securityid varchar(31),Exchangeid varchar(6),MDSecurityStat int,"
			"LastPrice float,PreClosePrice float,OpenPrice float,HighestPrice float,LowestPrice float,"
			"Volume bigint,Turnover float,OpenInterest float,ClosePrice float,UpperLimitPrice float,LowerLimitPrice float,"
			"UpdateTime varchar(9),UpdateMillisec int,"
			"BidPrice1 float,BidVolume1 bigint,AskPrice1 float,AskVolume1 bigint,"//到时候 long long int 的类型需要改变
			"BidPrice2 float,BidVolume2 bigint,AskPrice2 float,AskVolume2 bigint,"
			"BidPrice3 float,BidVolume3 bigint,AskPrice3 float,AskVolume3 bigint,"
			"BidPrice4 float,BidVolume4 bigint,AskPrice4 float,AskVolume4 bigint,"
			"BidPrice5 float,BidVolume5 bigint,AskPrice5 float,AskVolume5 bigint)";
		otl_cursor::direct_exec
		(
			sql_server,
			create_table_sql.c_str()
			, otl_exception::enabled); // create table
		//存储数据;
		string insert_data_sql = "insert into " + table_name +
			" values(:f1<char[9]>,:f2<char[41]>,:f3<char[31]>,:f4<char[6]>,:f5<int>,:f6<double>,:f7<double>,"
			//f8=ClosePrice
			":f8<double>,:f9<double>,:f10<double>,:f11<bigint>,:f12<double>,:f13<double>,:f14<double>,:f15<double>,:f16<double>,"
			//f17=BidVolume
			":f17<char[9]>,:f18<int>,:f19<double>,:f20<bigint>,:f21<double>,:f22<bigint>,:f23<double>,:f24<bigint>,:f25<double>,:f26<bigint>,:f27<double>,:f28<bigint>,:f29<double>,:f30<bigint>,:f31<double>,:f32<bigint>,:f33<double>,:f34<bigint>,:f35<double>,:f36<bigint>,:f37<double>,:f38<bigint>)";
		otl_stream o(1, //buffer size should be == 1 always on INSERT.
			insert_data_sql.c_str(),
			sql_server 
		);
		o << pMarketData->TradingDay
			<< pMarketData->SecurityName
			<< pMarketData->Securityid
			<< pMarketData->ExchangeID
			//不做收盘的行情，所以强转int
			<< (int)pMarketData->MDSecurityStat
			<< pMarketData->LastPrice
			<< pMarketData->PreClosePrice
			<< pMarketData->OpenPrice
			<< pMarketData->HighestPrice
			<< pMarketData->LowestPrice
			<< pMarketData->Volume
			<< pMarketData->Turnover
			<< pMarketData->OpenInterest
			<< pMarketData->ClosePrice
			<< pMarketData->UpperLimitPrice
			<< pMarketData->LowerLimitPrice
			<< pMarketData->UpdateTime
			<< pMarketData->UpdateMillisec
			<< pMarketData->BidPrice1
			<< pMarketData->BidVolume1
			<< pMarketData->AskPrice1
			<< pMarketData->AskVolume1
			<< pMarketData->BidPrice2
			<< pMarketData->BidVolume2
			<< pMarketData->AskPrice2
			<< pMarketData->AskVolume2
			<< pMarketData->BidPrice3
			<< pMarketData->BidVolume3
			<< pMarketData->AskPrice3
			<< pMarketData->AskVolume3
			<< pMarketData->BidPrice4
			<< pMarketData->BidVolume4
			<< pMarketData->AskPrice4
			<< pMarketData->AskVolume4
			<< pMarketData->BidPrice5
			<< pMarketData->BidVolume5
			<< pMarketData->AskPrice5
			<< pMarketData->AskVolume5;
	}
	catch (otl_exception & p)
	{


		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);




	}


	return true;
}





bool IDatastore::StoreFundsFlowMarketData(FundsFlowMarketData* pFundsFlowMarketData)
{
	if (!m_bIsLogined) {
		return false;
	}
	if (NULL == pFundsFlowMarketData) {
		return false;
	}
	try
	{
		otl_cursor::direct_exec
		(
			sql_server,
			("use " + m_ServerInfo.id).c_str(),
			otl_exception::enabled // disable OTL exceptions
		); // droptable
	//建表;
		string table_name(pFundsFlowMarketData->SecurityID);
		string create_table_sql =
			"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[" + table_name + "]') AND type in (N'U')) "
			"create table " + table_name + "(ExchangeID varchar(6),"
			"Securityid varchar(31),UpdateTime varchar(9),UpdateMillisec int,"
			"RetailBuyTurnover float,RetailBuyVolume bigint,RetailBuyAmount bigint,RetailSellTurnover float,RetailSellVolume bigint,RetailSellAmount bigint,"
			"MiddleBuyTurnover float,MiddleBuyVolume bigint,MiddleBuyAmount bigint,MiddleSellTurnover float,MiddleSellVolume bigint,MiddleSellAmount bigint,"
			"LargeBuyTurnover float,LargeBuyVolume bigint,LargeBuyAmount bigint,LargeSellTurnover float,LargeSellVolume bigint,LargeSellAmount bigint,"
			"InstitutionBuyTurnover float,InstitutionBuyVolume bigint,InstitutionBuyAmount bigint,InstitutionSellTurnover float,InstitutionSellVolume bigint,InstitutionSellAmount bigint";




		otl_cursor::direct_exec
		(
			sql_server,
			create_table_sql.c_str()
			, otl_exception::enabled
		); // create table
		//存储数据;
		string insert_data_sql = "insert into " + table_name +
			" values(:f1<char[6]>,:f2<char[31]>,:f3<char[9]>,:f4<int>,"
			//f5 散户*6
			":f5<double>,:f6<bigint>,:f7<bigint>,:f8<double>,:f9<bigint>,:f10<bigint>,"
			//f11 = 中户*6
			":f11<double>,:f12<bigint>,:f13<bigint>,:f14<double>,:f15<bigint>,:f16<bigint>,"
			":f17<double>,:f18<bigint>,:f19<bigint>,:f20<double>,:f21<bigint>,:f22<bigint>,"
			":f23<double>,:f24<bigint>,:f25<bigint>,:f26<double>,:f27<bigint>,:f28<bigint>";
		otl_stream o
		(1, //buffer size should be == 1 always on INSERT.
			insert_data_sql.c_str(),
			sql_server // connectobject
		);
		o << pFundsFlowMarketData->ExchangeID
			<< pFundsFlowMarketData->SecurityID
			<< pFundsFlowMarketData->UpdateTime
			<< pFundsFlowMarketData->UpdateMillisec
			<<pFundsFlowMarketData->RetailBuyAmount
			<< pFundsFlowMarketData->RetailBuyTurnover
			<< pFundsFlowMarketData->RetailBuyVolume
			<< pFundsFlowMarketData->RetailSellAmount
			<< pFundsFlowMarketData->RetailSellTurnover
			<< pFundsFlowMarketData->RetailSellVolume
			<< pFundsFlowMarketData->MiddleBuyAmount
			<< pFundsFlowMarketData->MiddleBuyTurnover
			<< pFundsFlowMarketData->MiddleBuyVolume
			<< pFundsFlowMarketData->MiddleSellAmount
			<< pFundsFlowMarketData->MiddleSellTurnover
			<< pFundsFlowMarketData->MiddleSellVolume
			<< pFundsFlowMarketData->LargeBuyAmount
			<< pFundsFlowMarketData->LargeBuyTurnover
			<< pFundsFlowMarketData->LargeBuyVolume
			<< pFundsFlowMarketData->LargeSellAmount
			<< pFundsFlowMarketData->LargeSellTurnover
			<< pFundsFlowMarketData->LargeSellVolume
			<< pFundsFlowMarketData->InstitutionBuyAmount
			<< pFundsFlowMarketData->InstitutionBuyTurnover
			<< pFundsFlowMarketData->InstitutionBuyVolume
			<< pFundsFlowMarketData->InstitutionSellAmount
			<< pFundsFlowMarketData->InstitutionSellTurnover
			<< pFundsFlowMarketData->InstitutionSellVolume;

	}
	catch (otl_exception & p)
	{
		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);
	}
	return true;
}





bool IDatastore::StoreTradingAccount(TradingAccount* pTradingAccount)
{
	if (!m_bIsLogined) 
	{
		return false;
	}
	if (NULL == pTradingAccount) 
	{
		return false;
	}
	try
	{
		
		otl_cursor::direct_exec
		(
			sql_server,
			("use " + m_ServerInfo.id).c_str(),
			otl_exception::enabled // disable OTL exceptions
		);
	//建表;
		char tab[9] = "00030398";
		string table_name(tab);
		string create_table_sql =
			"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[A华鑫证券账户]') AND type in (N'U')) "
			"create table A华鑫证券账户 (AccountID varchar(21),Available float,WithdrawQuota float,CurrencyID varchar(4),"
			"Deposit float,Withdraw float,FrozenCash float,FrozenCommission float ,"
			"AccountOwner varchar(13),BankID varchar(18),BankAccountID varchar(31))";




		otl_cursor::direct_exec
		(
			sql_server,
			create_table_sql.c_str()
			, otl_exception::enabled); // create table
		//存储数据;
		string insert_data_sql = "insert into A华鑫证券账户" 
			" values(:f1<char[21]>,:f2<double>,:f3<double>,:f4<char[4]>,"
			":f5<double>,:f6<double>,:f7<double>,:f8<double>,"
			":f9<char(13)>,:f10<char(18)>,:f11<char(31)>)";


		otl_stream o(1, 
			insert_data_sql.c_str(),
			sql_server 
		);
		o << pTradingAccount->AccountID
			<< pTradingAccount->Available
			<< pTradingAccount->WithdrawQuota
			<< pTradingAccount->CurrencyID
			<< pTradingAccount->Deposit
			<< pTradingAccount->Withdraw
			<< pTradingAccount->FrozenCash
			<< pTradingAccount->FrozenCommission
			<< pTradingAccount->AccountOwner
			<< pTradingAccount->BankID
			<< pTradingAccount->BankAccountID;
	}
	catch (otl_exception & p)
	{
		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);
	}


	return true;
}

bool IDatastore::StorePosition(Position* pPosition)
{
	if (!m_bIsLogined) {
		return false;
	}
	if (NULL == pPosition) {
		return false;
	}
	try
	{
		otl_cursor::direct_exec
		(
			sql_server,
			("use " + m_ServerInfo.id).c_str(),
			otl_exception::enabled // disable OTL exceptions
		); // droptable
	//建表;
		string table_name(pPosition->InvestorID);
		string create_table_sql =
			"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[A仓位]') AND type in (N'U')) "
			"create table A仓位(SecurityID varchar(31),InvestorID varchar(13),BusinessUnitID varchar(17),ExchangeID varchar(6),"
			"MarketID varchar(10),ShareholderID varchar(11),TradingDay varchar(9),HistoryPos int ,HistoryPosFrozen int,TodayBSPos int,TodayBSFrozen int,TodayPRPos int,"
			"TodayPRFrozen int ,TotalPosCost float,PrePosition int ,AvailablePosition int ,CurrentPosition int ,LastPrice float,OpenPosCost float)";


		otl_cursor::direct_exec
		(
			sql_server,
			create_table_sql.c_str()
			, otl_exception::enabled); // create table
		//存储数据;
		string insert_data_sql = "insert into A仓位" 
			" values(:f1<char[31]>,:f2<char(13)>,:f3<char(17)>,:f4<char[6]>,"
			":f5<char(10)>,:f6<char(11)>,:f7<char(9)>,:f8<int>,"
			":f9<int>,:f10<int>,:f11<int>,:f12<int>,"
			":f13<int>,:f14<double>,:f15<int>,:f16<int>,:f17<int>,:f18<double>,:f19<double>)";


		otl_stream o(1, //buffer size should be == 1 always on INSERT.
			insert_data_sql.c_str(),
			sql_server // connectobject
		);
		o << pPosition->SecurityID
			<< pPosition->InvestorID
			<< pPosition->BusinessUnitID
			<< pPosition->ExchangeID
			<< pPosition->MarketID
			<< pPosition->ShareholderID
			<< pPosition->TradingDay
			<< pPosition->HistoryPos
			<< pPosition->HistoryPosFrozen
			<< pPosition->TodayBSPos
			<< pPosition->TodayBSFrozen
			<< pPosition->TodayPRPos
			<< pPosition->TodayPRFrozen
			<< pPosition->TotalPosCost
			<< pPosition->PrePosition
			<< pPosition->AvailablePosition
			<< pPosition->CurrentPosition
			<< pPosition->LastPrice
			<< pPosition->OpenPosCost;


	}
	catch (otl_exception & p)
	{
		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);
	}


	return true;
}

bool IDatastore::StoreConversionBondInfo(ConversionBondInfo* pConversionBondInfo)
{
	if (!m_bIsLogined) {
		return false;
	}
	if (NULL == pConversionBondInfo) {
		return false;
	}
	try
	{
		otl_cursor::direct_exec
		(
			sql_server,
			("use " + m_ServerInfo.id).c_str(),
			otl_exception::enabled // disable OTL exceptions
		); // droptable
	//建表;
		string table_name(pConversionBondInfo->SecurityID);
		string create_table_sql =
			"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[" + table_name + "]') AND type in (N'U')) "
			"create table " + table_name + "(ExchangeID varchar(6),MarketID varchar(10),SecurityID varchar(31),ConvertOrderID varchar(31),"
			"ConvertPrice float,ConvertVolUnit int ,ConvertVolMax int ,ConvertVolMin int,BeginDate varchar(9),EndDate varchar(9),IsSupportCancel int";



		otl_cursor::direct_exec
		(
			sql_server,
			create_table_sql.c_str()
			, otl_exception::enabled); // create table
		//存储数据;
		string insert_data_sql = "insert into " + table_name +
			" values(:f1<char[6]>,:f2<char(10)>,:f3<char(31)>,:f4<char[31]>,"
			":f5<double>,:f6<int>,:f7<int>,:f8<int>,"
			":f9<char(9)>,:f10<char(9)>,:f11<int>";




		otl_stream o(1, //buffer size should be == 1 always on INSERT.
			insert_data_sql.c_str(),
			sql_server // connectobject
		);
		o << pConversionBondInfo->ExchangeID
			<< pConversionBondInfo->MarketID
			<< pConversionBondInfo->SecurityID
			<< pConversionBondInfo->ConvertOrderID
			<< pConversionBondInfo->ConvertPrice
			<< pConversionBondInfo->ConvertVolUnit
			<< pConversionBondInfo->ConvertVolMax
			<< pConversionBondInfo->ConvertVolMin
			<< pConversionBondInfo->BeginDate
			<< pConversionBondInfo->EndDate
			<< pConversionBondInfo->IsSupportCancel;

	}
	catch (otl_exception & p)
	{
		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);
	}


	return true;
}

bool IDatastore::StorePrematurityRepoOrder(PrematurityRepoOrder* pPrematurityRepoOrder)
{
	if (!m_bIsLogined) {
		return false;
	}
	if (NULL == pPrematurityRepoOrder) {
		return false;
	}
	try
	{
		otl_cursor::direct_exec
		(
			sql_server,
			("use " + m_ServerInfo.id).c_str(),
			otl_exception::enabled // disable OTL exceptions
		); // droptable
	//建表;
		string table_name(pPrematurityRepoOrder->InvestorID);
		string create_table_sql =
			"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[" + table_name + "]') AND type in (N'U')) "
			"create table " + table_name + "(ExchangeID varchar(6),MarketID varchar(10),InvestorID varchar(13),ShareholderID varchar(11),"
			"TradeDay varcahr(9),ExpireDay varcahr(9),OrderLocalID varchar(13) ,SecurityID varcahr(13),SecurityName varchar(41),ProductID varchar(19),SecurityType varchar(65),"
			"Direction varchar(26),VolumeTraded int,Price float,Turnover float,TradeID varchar(21),RepoTotalMoney float,InterestAmount float";



		otl_cursor::direct_exec
		(
			sql_server,
			create_table_sql.c_str()
			, otl_exception::enabled); // create table
		//存储数据;
		string insert_data_sql = "insert into " + table_name +
			" values(:f1<char[6]>,:f2<char(10)>,:f3<char(13)>,:f4<char[11]>,"
			":f5<char(9)>,:f6<char(9)>,:f7<char(13)>,:f8<char(13)>,"
			":f9<char(41)>,:f10<char(19)>,:f11<char(65)>,:f12<char(26)>,:f13<int>,:f14<double>,:f15<double>,:f16<char(21)>,:f17<double>,:f18<double>";




		otl_stream o(1, //buffer size should be == 1 always on INSERT.
			insert_data_sql.c_str(),
			sql_server // connectobject
		);
		o << pPrematurityRepoOrder->ExchangeID
			<< pPrematurityRepoOrder->MarketID
			<< pPrematurityRepoOrder->InvestorID
			<< pPrematurityRepoOrder->ShareholderID
			<< pPrematurityRepoOrder->TradeDay
			<< pPrematurityRepoOrder->ExpireDay
			<< pPrematurityRepoOrder->OrderLocalID
			<< pPrematurityRepoOrder->SecurityID
			<< pPrematurityRepoOrder->SecurityName
			<< pPrematurityRepoOrder->ProductID
			<< pPrematurityRepoOrder->SecurityType
			<< pPrematurityRepoOrder->Direction
			<< pPrematurityRepoOrder->VolumeTraded
			<< pPrematurityRepoOrder->Price
			<< pPrematurityRepoOrder->Turnover
			<< pPrematurityRepoOrder->TradeID;

	}
	catch (otl_exception & p)
	{
		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);
	}


	return true;
}

bool IDatastore::StoreHistoryTrade(HistoryTrade* pHistoryTrade)
{
	if (!m_bIsLogined) {
		return false;
	}
	if (NULL == pHistoryTrade) {
		return false;
	}
	try
	{
		otl_cursor::direct_exec
		(
			sql_server,
			("use " + user).c_str(),
			otl_exception::enabled // disable OTL exceptions
		); // droptable
	//建表;
		string table_name(pHistoryTrade->SecurityID);
		string create_table_sql =
			"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[" + table_name + "]') AND type in (N'U')) "
			"create table " + table_name + "(InvestorID varchar(13),SecurityID varchar(31),UserID varchar(16),ExchangeID varchar(6),"
			"TradeID varcahr(21),Direction varcahr(26),OffsetFlag varchar(7) ,Price double,Volume int,TradeDate varchar(9),TradeTime varchar(9),ActualBrokerage	double";


		otl_cursor::direct_exec
		(
			sql_server,
			create_table_sql.c_str()
			, otl_exception::enabled); // create table
		//存储数据;
		string insert_data_sql = "insert into " + table_name +
			" values(:f1<char[6]>,:f2<char(10)>,:f3<char(13)>,:f4<char[11]>,"
			":f5<char(9)>,:f6<char(9)>,:f7<char(13)>,:f8<char(13)>,"
			":f9<char(41)>,:f10<char(19)>,:f11<char(65)>,:f12<char(26)>,:f13<int>,:f14<double>,:f15<double>,:f16<char(21)>,:f17<double>,:f18<double>";




		otl_stream o(1, //buffer size should be == 1 always on INSERT.
			insert_data_sql.c_str(),
			sql_server // connectobject
		);
		o << pHistoryTrade->InvestorID
			<< pHistoryTrade->SecurityID
			<< pHistoryTrade->UserID
			<< pHistoryTrade->ExchangeID
			<< pHistoryTrade->TradeID
			<< pHistoryTrade->Direction
			<< pHistoryTrade->OffsetFlag
			<< pHistoryTrade->Price
			<< pHistoryTrade->Volume
			<< pHistoryTrade->TradeDate
			<< pHistoryTrade->TradeTime
			<< pHistoryTrade->ActualBrokerage;

	}
	catch (otl_exception & p)
	{
		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);
	}


	return true;
}

bool IDatastore::StoreReturnTrade(ReturnOrder* pRturnOrder)
{
	if (!m_bIsLogined) {
		return false;
	}
	if (NULL == pRturnOrder) {
		return false;
	}
	try
	{
		otl_cursor::direct_exec
		(
			sql_server,
			("use " + m_ServerInfo.id).c_str(),
			otl_exception::enabled // disable OTL exceptions
		); // droptable
	//建表;
		string table_name(pRturnOrder->InsertDate);
		string create_table_sql =
			"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[" + table_name + "]') AND type in (N'U')) "
			"create table " + table_name + "(InvestorID varchar(13),"
			"SecurityID varchar(31),BusinessUnitID varchar(17),ForceCloseReason varchar(7),ExchangeID varchar(6),"
			"OrderSubmitStatus varchar(8),OrderStatus varchar(6),VolumeTraded int,VolumeTotal int,InsertDate varchar(9),"
			"InsertTime varchar(9),CancelTime varchar(9),FrontID int,SessionID int,StatusMsg varchar(81),UserForceClose int,Turnover float)";

		otl_cursor::direct_exec
		(
			sql_server,
			create_table_sql.c_str()
			, otl_exception::enabled); // create table
		//存储数据;
		string insert_data_sql = "insert into " + table_name +
			" values(:f1<char[13]>,:f2<char[31]>,:f3<char[17]>,:f4<char[7]>,:f5<char[6]>,:f6<char(8)>,:f7<char(6)>,"
			":f8<int>,:f9<int>,:f10<char(9)>,:f11<char(9)>,:f12<char(9)>,:f13<int>,:f14<int>,:f15<char(81)>,:f16<int>,"
			":f17<double>)";
		otl_stream o(1, //buffer size should be == 1 always on INSERT.
			insert_data_sql.c_str(),
			sql_server // connectobject
		);
		o << pRturnOrder->InvestorID
			<< pRturnOrder->SecurityID
			<< pRturnOrder->BusinessUnitID
			<< pRturnOrder->ForceCloseReason
			<< pRturnOrder->ExchangeID
			<< pRturnOrder->OrderSubmitStatus
			<< pRturnOrder->OrderStatus
			<< pRturnOrder->VolumeTraded
			<< pRturnOrder->VolumeTotal
			<< pRturnOrder->InsertDate
			<< pRturnOrder->InsertTime
			<< pRturnOrder->CancelTime
			<< pRturnOrder->FrontID
			<< pRturnOrder->SessionID
			<< pRturnOrder->StatusMsg
			<< pRturnOrder->UserForceClose
			<< pRturnOrder->Turnover;



	}
	catch (otl_exception & p)
	{
		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);
	}


	return true;

}

bool IDatastore::StoreUserInfo(UserInfo* pUserInfo)
{
	if (!m_bIsLogined)
	{
		return false;
	}
	if (NULL == pUserInfo)
	{
		return false;
	}
	try
	{
		otl_cursor::direct_exec
		(
			sql_server,
			("use " + m_ServerInfo.id).c_str(),
			otl_exception::enabled // disable OTL exceptions
		);
	//建表
		string table_name(pUserInfo->SystemName);
		string create_table_sql = "IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].["+table_name +"]') AND type in (N'U')) "
			"create table "+table_name+"("
			"UserID varchar(16),BrokerID varchar(11),TradingDay varchar(9),LoginTime varchar(9),"
			"SystemName char(41),ExchangeTime varchar(9))";
		otl_cursor::direct_exec
		(
			sql_server,
			create_table_sql.c_str()
			, otl_exception::enabled); // create table
		//存储数据
		string insert_data_sql = "insert into " + table_name + " values(:f1<char[16]>,:f2<char[11]>,:f3<char[9]>,:f4<char[9],:f5<char[9]>,:f6<char[41]>,:f7<char[9]>)";
		otl_stream o(1, //buffer size should be == 1 always on INSERT.
			insert_data_sql.c_str(),
			sql_server 
		);
		o << pUserInfo->UserID
			<< pUserInfo->BrokerID
			<< pUserInfo->TradingDay
			<< pUserInfo->LoginTime
			<< pUserInfo->SystemName
			<< pUserInfo->ExhangeTime;
	}
	catch (otl_exception& p)
	{
		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);
	}
}

bool IDatastore::StoreExhchange(Exchange* pExchange)
{

	if (!m_bIsLogined)
	{
		return false;
	}
	if (NULL == pExchange)
	{
		return false;
	}
	try
	{
		otl_cursor::direct_exec
		(
			sql_server,
			("use " + m_ServerInfo.id).c_str(),
			otl_exception::enabled // disable OTL exceptions
		); // droptable
	//建表
		string create_table_sql = "IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[tb_exchanges]') AND type in (N'U')) "
			"create table tb_exchanges(ExchangeID int,ExchangeName varchar(61),TradingDay varchar(9))";
		otl_cursor::direct_exec
		(
			sql_server,
			create_table_sql.c_str()
			, otl_exception::enabled); // create table
		//存储数据
		string insert_data_sql = "insert into tb_exchanges values(:f1<int>,:f2<char[61]>,:f3<char[9]>)";
		otl_stream o(1, //buffer size should be == 1 always on INSERT.
			insert_data_sql.c_str(),
			sql_server // connectobject
		);
		o << (int)pExchange->ExchangeID
			<< pExchange->ExchangeName.c_str()
			<< pExchange->TradingDay.c_str();
		return true;
	}
	catch (otl_exception& p)
	{
		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);
	}
	return false;
}

bool IDatastore::StoreInsertOrder(InputOrder* inputorder)
{
	if (!m_bIsLogined) {
		return false;
	}
	if (NULL == inputorder) {
		return false;
	}
	try
	{
		otl_cursor::direct_exec
		(
			sql_server,
			("use " + m_ServerInfo.id).c_str(),
			otl_exception::enabled // disable OTL exceptions
		); // droptable
	//建表;
		string create_table_sql =
			"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[A报单信息]') AND type in (N'U')) "
			"create table  A报单信息(InvestorID varchar(13),"
			"SecurityID varchar(31),BusinessUnitID varchar(17),ForceCloseReason varchar(7),ExchangeID varchar(6),LimitPrice float,VolumeTotalOriginal float)";	


		otl_cursor::direct_exec
		(
			sql_server,
			create_table_sql.c_str()
			, otl_exception::enabled); // create table
		//存储数据;
		string insert_data_sql = "insert into A报单信息"
			" values(:f1<char[13]>,:f2<char[31]>,:f3<char[17]>,:f4<char[7]>,:f5<char[6]>,:f6<double>,:f7<double>)";
		otl_stream o(1, //buffer size should be == 1 always on INSERT.
			insert_data_sql.c_str(),
			sql_server // connectobject
		);
		o << inputorder->InvestorID.c_str()
			<< inputorder->SecurityID
			<< inputorder->BusinessUnitID.c_str()
			<< inputorder->ForceCloseReason.c_str()
			<< inputorder->ExchangeID.c_str()
			<< inputorder->LimitPrice
			<< inputorder->VolumeTotalOriginal;



	}
	catch (otl_exception& p)
	{
		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);
	}


	return true;
}

bool IDatastore::StoreInsertOrderAction(InputOrderAction* inputorderaction)
{
	if (!m_bIsLogined) {
		return false;
	}
	if (NULL == inputorderaction)
	{
		return false;
	}
	try
	{
		otl_cursor::direct_exec
		(
			sql_server,
			("use " + m_ServerInfo.id).c_str(),
			otl_exception::enabled // disable OTL exceptions
		); // droptable
	//建表;
		string create_table_sql =
			"IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[A撤单信息]') AND type in (N'U')) "
			"create table A报单信息(InvestorID varchar(13),"
			"SecurityID varchar(31),LimitPrice float,VolumeChange int)";

		otl_cursor::direct_exec
		(
			sql_server,
			create_table_sql.c_str()
			, otl_exception::enabled); // create table
		//存储数据;
		string insert_data_sql = "insert into A报单信息"
			" values(:f1<char[13]>,:f2<char[31]>,:f3<double>,:f4<int>)";
		otl_stream o(1, //buffer size should be == 1 always on INSERT.
			insert_data_sql.c_str(),
			sql_server // connectobject
		);
		o << inputorderaction->InvestorID.c_str()
			<< inputorderaction->SecurityID.c_str()
			<< inputorderaction->LimitPrice
			<< inputorderaction->VolumeChange;



	}
	catch (otl_exception& p)
	{
		MessageBoxA(NULL, (char*)p.msg, __FUNCTION__, MB_OK);
	}


	return true;
}


bool IDatastore::LoadMarketData(vector<MarketData>& out_data, int start_time, int end_time)
{
	return false;
}

bool IDatastore::LoadFundsFlowMarketData(vector<FundsFlowMarketData>& out_data)
{
	return false;
}

bool IDatastore::LoadTradingAccount(vector<TradingAccount>& out_data, int date)
{
	return false;
}

bool IDatastore::LoadPosition(vector<Position>& out_data)
{
	return false;
}

bool IDatastore::LoadConversionBondInfo(vector<ConversionBondInfo>& out_data, int date)
{
	return false;
}

bool IDatastore::LoadPrematurityRepoOrder(vector<PrematurityRepoOrder>& out_data, int date)
{
	return false;
}

bool IDatastore::LoadHistoryTrade(vector<HistoryTrade>& out_data, int date)
{
	return false;
}

bool IDatastore::LoadReturnTrade(vector<ReturnOrder>& out_data, int date)
{
	return false;
}

bool IDatastore::LoadInsertOrder(InputOrder* inputorder)
{
	return false;
}












//插入数据:test
void insert(otl_connect& db)
{
	//buffer size should be == 1 always on INSERT.
	otl_stream o(1, "insert into tb88 values(:f1<char[32]>,:f2<char[64]>,:f3<float>)", db);
	o << "IF1401" << "股指1401" << 1234.3f;
	o << "IF1402" << "股指1402" << 1234.0f;
}

void select_data(otl_connect& db)
{
	otl_stream o(1024, //buffer size should be == 1 always on INSERT.
		"select * from tb88",
		db // connectobject
	);
	char InstrumentID[32], InstrumentName[64];
	double LastPrice;
	while (!o.eof())
	{
		o >> InstrumentID >> InstrumentName >> LastPrice;
		char buf[256] = { 0 };
		sprintf(buf, "合约ID:%s,合约名:%s,价格:%.02f", InstrumentID, InstrumentName, LastPrice);
		MessageBoxA(NULL, buf, "查询", MB_OK);
	}
}

void trim(string& s)
{
	int index = 0;
	if (!s.empty())
	{
		while ((index = s.find(' ', index)) != string::npos)
		{
			s.erase(index, 1);
		}
	}
}
