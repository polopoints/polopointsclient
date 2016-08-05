#include "StdAfx.h"
#include "polopointclient.h"
#include "SqliteDeal.h"
#include "CommonStruct.h"
#include "MyMutex.h"

CSqliteDeal::CSqliteDeal(void)
{
	m_pSqliteWrite = NULL;
	m_pCs = new CCriticalSection();

}

CSqliteDeal::~CSqliteDeal(void)
{
	if ( NULL != m_pSqliteWrite ) {
       sqlite3_close(m_pSqliteWrite);
	   m_pSqliteWrite = NULL ;
	}
	if( NULL != m_pCs ) {
		delete m_pCs;
		m_pCs = NULL;
	}

}

//初始化检测表是否存在，不存在则创建表
BOOL CSqliteDeal::InitializationDB(){
	sqlite3 **pDBConn = GetDBConnect(); //获取数据库连接
	string strTableName = "sqlite_master";
	string strCondition = "type='table' and name = 't_wallet_address'";

	if(!GetTableCountItem(strTableName, strCondition)) 
	{
		string createSQL="CREATE TABLE t_wallet_address(address TEXT PRIMARY KEY, reg_id TEXT, money DOUBLE, cold_dig INT, sign INT, label TEXT)";
		if(!ExcuteSQL(pDBConn, NULL, createSQL, NULL))
		{
			LogPrint("INFO", "Create table t_wallet_address failed\n");
			return FALSE;
		}
	}
	strCondition = "type='table' and name= 't_common_address'";
	if(!GetTableCountItem(strTableName, strCondition)) 
	{
		string createSQL="CREATE TABLE t_common_address(reg_id TEXT, app_id TEXT, keyid integer PRIMARY KEY autoincrement)";
		if(!ExcuteSQL(pDBConn, NULL, createSQL, NULL))
		{
			LogPrint("INFO", "Create table t_common_address failed\n");
			return FALSE;
		}
	}

	strCondition = "type='table' and name= 't_chain_tip'";
	if(!GetTableCountItem(strTableName, strCondition))
	{
		string createSQL="CREATE TABLE t_chain_tip(block_hash TEXT PRIMARY KEY,block_height INT,confired_time INT,fuelrate INT,lock INT)";
		if(!ExcuteSQL(pDBConn, NULL, createSQL, NULL))
		{
			LogPrint("INFO", "Create table t_chain_tip failed\n");
			return FALSE;
		}
	}else if (!IsExistField(_T("t_chain_tip"),_T("lock"),_T("1=1"))  )
	{
		DeleteTable(_T("t_chain_tip"));
		string createSQL="CREATE TABLE t_chain_tip(block_hash TEXT PRIMARY KEY,block_height INT,confired_time INT,fuelrate INT,lock INT)";
		if(!ExcuteSQL(pDBConn, NULL, createSQL, NULL))
		{
			LogPrint("INFO", "Create table t_p2p_quiz failed\n");
			return FALSE;
		}
	}

	strCondition = _T("type='table' and name='t_address_book'");
	if(!GetTableCountItem(strTableName, strCondition))
	{
		string createSQL="CREATE TABLE t_address_book(Label TEXT,address TEXT PRIMARY KEY)";
		if(!ExcuteSQL(pDBConn, NULL, createSQL, NULL))
		{
			LogPrint("INFO", "Create table t_address_book failed\n");
			return FALSE;
		}
	}

	strCondition = "type='table' and name='t_transaction'";
	if(!GetTableCountItem(strTableName, strCondition))
	{
		string createSQL= "CREATE TABLE t_transaction(hash TEXT PRIMARY KEY, tx_type TEXT,version INT, src_addr TEXT, pub_key TEXT, miner_pub_key TEXT, fees DOUBLE, height INT, des_addr TEXT, money DOUBLE, contract TEXT, confirm_height INT, confirmed_time INT, block_hash TEXT, state INT)";
		if(!ExcuteSQL(pDBConn, NULL, createSQL, NULL))
		{
			LogPrint("INFO", "Create table t_p2p_quiz failed\n");
			return FALSE;
		}
	}

	/// 创建与app所有相关的交易数据库
	strCondition = "type='table' and name='t_app_transaction'";
	if(!GetTableCountItem(strTableName, strCondition))
	{
		string createSQL= "CREATE TABLE t_app_transaction(hash TEXT PRIMARY KEY, tx_type TEXT,version INT, src_addr TEXT, src_regid TEXT, fees DOUBLE, height INT, desr_addr TEXT,desr_regid TEXT, money DOUBLE, contract TEXT, confirm_height INT, confirmed_time INT, block_hash TEXT)";
		if(!ExcuteSQL(pDBConn, NULL, createSQL, NULL))
		{
			LogPrint("INFO", "Create table t_app_transaction failed\n");
			return FALSE;
		}
	}

	///清空blcokchain
	ClearTableData(_T("t_chain_tip") );
	return TRUE ;
}

//获取数据库连接
sqlite3** CSqliteDeal::GetDBConnect() 
{
	LOCK(m_pCs);
	string strPath;
	strPath = strprintf("%s\\db\\data.db" , theApp.m_strInsPath.c_str() );
	//strPath.Format(_T("%s\\data.db") , theApp.dbpath); 
	if(NULL == m_pSqliteWrite)
	{
		int ret = sqlite3_open_v2( UiFun::MbcsToUtf8(strPath.c_str()), &m_pSqliteWrite, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, NULL);
		if ( 0 != ret) {   //打开指定的数据库文件,如果不存在将创建一个同名的数据库文件
			sqlite3_close(m_pSqliteWrite);
			m_pSqliteWrite = NULL ;
			LogPrint("INFO", "Open DataBase %s failed!\n", strPath.c_str());
			return FALSE ;
		}
		LogPrint("INFO", "Open DataBase %s succeed!\n", strPath.c_str());
	}
	
	return &m_pSqliteWrite;
}
//开启事务
BOOL CSqliteDeal::BeginDBTransaction()
{
	sqlite3** ppDb = GetDBConnect();
	LOCK(m_pCs);
	char *pzErrorMsg;
	int nResult = sqlite3_exec(*ppDb, "begin;", 0,0, &pzErrorMsg);
	if ( nResult != SQLITE_OK ){
		LogPrint("INFO", "nResult:%d error:%s\n", nResult, pzErrorMsg);
		sqlite3_close(*ppDb);  
		sqlite3_free(pzErrorMsg);  
		*ppDb = NULL ;
		return FALSE ;
	}
	return TRUE;
}

//提交事务
BOOL CSqliteDeal::CommitDbTransaction()
{
	sqlite3** ppDb = GetDBConnect();
	LOCK(m_pCs);
	char    *pzErrMsg;    //错误信息
	int nResult = sqlite3_exec(*ppDb, "commit;", 0,0, &pzErrMsg);
	if ( nResult != SQLITE_OK ){
		LogPrint("INFO", "nResult:%d error:%s\n", nResult, pzErrMsg);
		sqlite3_close(*ppDb);  
		sqlite3_free(pzErrMsg);  
		*ppDb = NULL ;
		return FALSE ;
	}
	LogPrint("INFO", "sync tx: begin database transaction\n")
	
	return TRUE;
}
//执行SQL语句
BOOL CSqliteDeal::ExcuteSQL(sqlite3** ppDb, CallBackFunc pFunc, string strSQL, void *pPara)
{
	LOCK(m_pCs);
	char *pzErrMsg = NULL;
	int nResult = sqlite3_exec(*ppDb, strSQL.c_str(), pFunc, pPara, &pzErrMsg);
	TRACE("%s\n", strSQL.c_str());
	if ( nResult != SQLITE_OK ){
		if(pzErrMsg != NULL) {
			LogPrint("INFO", "call ExcuteSQL retCode:%d, error:%s,SQL:%s\n", nResult, pzErrMsg, strSQL);
		}else {
			LogPrint("INFO", "call ExcuteSQL error retCode:%d, SQL:%s\n", nResult, strSQL);
		}
		
		sqlite3_close(*ppDb);
		sqlite3_free(pzErrMsg);
		*ppDb = NULL ;
		CheckFailedCode(nResult);
		return FALSE ;
	}
	return TRUE;
}

//获取某一个钱包地址
int CallGetWalletAddressItem(void *para, int n_column, char ** column_value, char ** column_name)
{
	uistruct::LISTADDR_t *pAddr = (uistruct::LISTADDR_t *)para;
	if(NULL == column_value[0])
		return -1;
	if(n_column != 6)
		return -1;

	pAddr->address=strprintf("%s",column_value[0]);
;
	pAddr->RegID=strprintf("%s",column_value[1]);

	pAddr->fMoney = strtod(column_value[2],NULL);

	pAddr->nColdDig = atoi(column_value[3]) ;

	pAddr->bSign = atoi(column_value[4]) ;

	pAddr->Label =strprintf("%s",column_value[5]);

	return 0;
}
//获取某一个钱包地址
int CallGetCommonWalletAddressItem(void *para, int n_column, char ** column_value, char ** column_name)
{
	uistruct::COMMONLISTADDR_t *pAddr = (uistruct::COMMONLISTADDR_t *)para;
	if(NULL == column_value[0])
		return -1;
	if(n_column != 3)
		return -1;

	pAddr->reg_id=strprintf("%s",column_value[0]);
;
	pAddr->app_id=strprintf("%s",column_value[1]);

	pAddr->keyid = atoi(column_value[2]) ;

	return 0;
}
//获取钱包地址列表
int CallGetWalletAddressList(void *para, int n_column, char ** column_value, char ** column_name)
{
	map<string,uistruct::LISTADDR_t> *pListInfo = (map<string,uistruct::LISTADDR_t> *)para;
	if(n_column != 6)
		return -1;
	
	uistruct::LISTADDR_t listdata;
	
	if(CallGetWalletAddressItem(&listdata, n_column, column_value, column_name) < 0 )
		return -1;
	
	(*pListInfo)[listdata.address] = listdata;
	return 0;
}
//获取常用钱包地址列表
int CallGetCommonWalletAddressList(void *para, int n_column, char ** column_value, char ** column_name)
{
	map<int,uistruct::COMMONLISTADDR_t> *pListInfo = (map<int,uistruct::COMMONLISTADDR_t> *)para;
	if(n_column != 3)
		return -1;

	uistruct::COMMONLISTADDR_t listdata;
	
	if(CallGetCommonWalletAddressItem(&listdata, n_column, column_value, column_name) < 0 )
		return -1;
	
	(*pListInfo)[listdata.keyid] = listdata;
	return 0;
}

//获取某个交易信息
int CallGetTransactionItem(void *para, int n_column, char ** column_value, char ** column_name)
{
	uistruct::REVTRANSACTION_t *pTxItem =  (uistruct::REVTRANSACTION_t*) para;
	if(NULL == column_value[0])
		return -1;
	
	if(n_column != 15)
		return -1;
	Json::Value root;
	string strValue;
	strValue = strprintf("%s" , column_value[0] ) ;
	root["hash"] = strValue;

	strValue = strprintf("%s" , column_value[1] ) ;
	root["txtype"] = strValue;

	root["ver"] = atoi(column_value[2]);

	strValue = strprintf("%s" , column_value[3] ) ;
	root["addr"] = strValue;

	strValue = strprintf("%s" , column_value[4] ) ;
	root["pubkey"] =  strValue;

	strValue= strprintf("%s" , column_value[5] ) ;
	root["miner_pubkey"] = strValue;

	root["fees"] = (INT64)(strtod (column_value[6], NULL)*100000000);

	root["height"] = atoi(column_value[7] );

	strValue =strprintf("%s" , column_value[8] ) ;
	root["desaddr"] = strValue;

	root["money"] = (INT64)(strtod ( column_value[9] , NULL)*100000000);

	strValue = strprintf("%s" , column_value[10] ) ;
	root["Contract"] =  strValue;

	root["confirmHeight"] = atoi(column_value[11]);

	root["confirmedtime"] = atoi( column_value[12]);

	strValue = strprintf("%s", column_value[13] ) ;
	root["blockhash"] = strValue;

	root["state"] = atoi(column_value[14]);

	pTxItem->JsonToStruct(root.toStyledString());
	
	return 0;

}
//获取交易列表
int CallGetTransactionList(void *para, int n_column, char ** column_value, char ** column_name)
{
	uistruct::TRANSRECORDLIST* pListInfo = (uistruct::TRANSRECORDLIST*) para;
	uistruct::REVTRANSACTION_t sTxInfo;
	if(CallGetTransactionItem((void *)(&sTxInfo), n_column, column_value, column_name) < 0)
	{
		return -1;
	}
	pListInfo->push_back(sTxInfo);
	return 0;
}
//获取表记录总条数
int CallCountTableItem(void *para, int n_column, char ** column_value, char ** column_name)
{
	if(NULL == column_value[0])
		return -1;
	if(1 == n_column && !strcmp(column_name[0],"num"))
		*(int *)para = atoi(column_value[0]);
	return 0;
}

//获取指定表某个字段的总和
int CallGetTableItemSum(void *para, int n_column, char ** column_value, char ** column_name)
{
	if(NULL == column_value[0])
		return 0;
	
	if(1 == n_column && !strcmp(column_name[0],"total"))
		*(double *)para = strtod (column_value[0], NULL);
	return 0;

}
//获取地址簿管理某一项
int CallGetAddressBookItem(void *para, int n_column, char ** column_value, char ** column_name)
{
	uistruct::ADDRBOOK_t *pAddrBook = (uistruct::ADDRBOOK_t *)para;
	if(n_column != 2)
		return -1;
	string strValue;
	strValue = strprintf("%s" , column_value[0] );
	pAddrBook->label = strValue;

	strValue = strprintf("%s", column_value[1] ) ;
	pAddrBook->address = strValue;
	return 0;
}
//获取地址簿管理列表
int CallGetAddressBookList(void *para, int n_column, char ** column_value, char ** column_name)
{
	map<string,uistruct::ADDRBOOK_t>* pAddrBookMap = (map<string, uistruct::ADDRBOOK_t> *)para;
	uistruct::ADDRBOOK_t sAddrBook;
	if(CallGetAddressBookItem(&sAddrBook, n_column, column_value, column_name) < 0)
		return -1;
	(*pAddrBookMap)[sAddrBook.address] = sAddrBook;
	return 0;
}

//获取表的记录条数
int CSqliteDeal::GetTableCountItem(const string &strTabName ,const string &strCondition)
{
	sqlite3 ** pDBConn = GetDBConnect(); //获取数据库连接
	int nCount(0);
	string strSQL="";
	strSQL = "SELECT count(*) as num FROM " + strTabName + _T(" WHERE ") + strCondition;
	//strSQL = "SELECT count(*) as num FROM ";
	//strSQL+ =strTabName;
	//strSQL+ =" WHERE ";
	//strSQL+ =strCondition;
	ExcuteSQL(pDBConn, &CallCountTableItem, strSQL, (void *)(&nCount));
	return nCount;
}

int CSqliteDeal::GetWalletAddressItem(const string &strCondition, uistruct::LISTADDR_t *pAddr)
{
	sqlite3 ** pDBConn = GetDBConnect(); //获取数据库连接
	
	string strSQL="";
	strSQL = "SELECT * FROM t_wallet_address WHERE " + strCondition;
	ExcuteSQL(pDBConn, &CallGetWalletAddressItem, strSQL, (void *)(pAddr));
	return 0;

}
int CSqliteDeal::GetCommonWalletAddressItem(const string &strCondition, uistruct::COMMONLISTADDR_t *pAddr)
{
	sqlite3 ** pDBConn = GetDBConnect(); //获取数据库连接
	
	string strSQL="";
	strSQL = "SELECT * FROM t_common_address WHERE " + strCondition;
	ExcuteSQL(pDBConn, &CallGetCommonWalletAddressItem, strSQL, (void *)(pAddr));
	return 0;
}
int CSqliteDeal::GetWalletAddressList(const string &strCondition, map<string,uistruct::LISTADDR_t> *pListInfo)
{
	sqlite3 ** pDBConn = GetDBConnect();
	string strSQL("");
	strSQL = "SELECT * FROM t_wallet_address WHERE " + strCondition;
	ExcuteSQL(pDBConn, &CallGetWalletAddressList, strSQL, (void *)pListInfo);
	return 0;
}
int CSqliteDeal::GetCommonWalletAddressList(const string &strCondition, map<int,uistruct::COMMONLISTADDR_t> *pListInfo)
{
	sqlite3 ** pDBConn = GetDBConnect();
	string strSQL("");
	strSQL = "SELECT * FROM t_common_address WHERE " + strCondition;
	ExcuteSQL(pDBConn, &CallGetCommonWalletAddressList, strSQL, (void *)pListInfo);
	return 0;
}

//删除数据库表记录
int CSqliteDeal::DeleteTableItem(const string &strTabName, const string &strCondition)
{
	sqlite3 **pDBConn = GetDBConnect(); //获取数据库连接
	string strSQL("");
	strSQL =_T("DELETE FROM ") + strTabName + _T(" WHERE ") + strCondition;
	if (!ExcuteSQL(pDBConn, NULL, strSQL, NULL))
	{
		LogPrint("INFO","DeleteTableItem Error:%s\r\n",strSQL);
	}
	
	return 0;
}


//插入一条数据到表中
BOOL CSqliteDeal::InsertTableItem(const string &strTabName ,const string &strSourceData)
{
	sqlite3 **pDBConn = GetDBConnect(); //获取数据库连接
	string strSQL("");
	strSQL = strprintf("INSERT INTO %s VALUES( %s )",(LPSTR)(LPCTSTR)strTabName.c_str(), (LPSTR)(LPCTSTR)strSourceData.c_str());
	return ExcuteSQL(pDBConn , NULL, strSQL, NULL);
}

//清空表数据
BOOL CSqliteDeal::ClearTableData(const string &strTabName)
{
	sqlite3 **pDBConn = GetDBConnect(); //获取数据库连接
	string strSQL = "DELETE FROM " + strTabName;
	return ExcuteSQL(pDBConn , NULL, strSQL, NULL);
}

//修改数据项
BOOL CSqliteDeal::UpdateTableItem(const string &strTabName , const string &strSourceData , const string &strW)
{
	sqlite3 **pDBConn = GetDBConnect(); //获取数据库连接
	string strSQL = "UPDATE " + strTabName + " SET " + strSourceData + " WHERE " + strW ;
	return ExcuteSQL(pDBConn , NULL, strSQL, NULL);
}

//判断blocktip是否还在主链上
BOOL CSqliteDeal::IsBlockTipInChain()
{
	return TRUE;
	char cTipBlockHash[35];
	memset(cTipBlockHash, 0, 35);
	uistruct::CHAIN_TIP_T blockchain;
	GetTipBlockHash(&blockchain);
	string strShowData, strCommand;
	if(blockchain.blockhash.length() == 0){
		strCommand = strprintf("%s %d","getblock" ,-1 );
	}else{
		strCommand = strprintf("%s %s",_T("getblock") ,blockchain.blockhash );
	}
	Json::Value root; 
	if(!CSoyPayHelp::getInstance()->SendRpc(strCommand,root))
	{
		TRACE("IsBlockTipInChain rpccmd listaddr error");
		return false;
	}


	int height = root["height"].asInt();
	strCommand = strprintf("%s %d","getblockhash" ,height );
	if(!CSoyPayHelp::getInstance()->SendRpc(strCommand,root))
	{
		TRACE("IsBlockTipInChain rpccmd getblockhash error");
		return false;
	}

	string newblock =root["hash"].asString(); 
	if (!strcmp(cTipBlockHash, newblock.c_str()))
	{
		return FALSE;
	}

	return TRUE ;
}

double CSqliteDeal::GetTableItemSum(const string &strTabName, const string &stdFieldName, const string &strCond)
{
	sqlite3 ** pDBConn = GetDBConnect(); //获取数据库连接
	string strSQL("");
	strSQL=strprintf("SELECT sum(%s) as total FROM %s WHERE %s",(LPSTR)(LPCTSTR)stdFieldName.c_str(), (LPSTR)(LPCTSTR)strTabName.c_str(), (LPSTR)(LPCTSTR)strCond.c_str());
	double dSum(0);
	if(!ExcuteSQL(pDBConn , &CallGetTableItemSum, strSQL, (void*)(&dSum)))
		return -1;
	 return dSum;
}

BOOL CSqliteDeal::GetAddressBookItem(const string &strCond, uistruct::ADDRBOOK_t *pAddrBook)
{
	sqlite3 ** pDBConn = GetDBConnect(); //获取数据库连接
	string strSQL("");
	strSQL= strprintf("SELECT * FROM t_address_book WHERE %s",(LPSTR)(LPCTSTR)strCond.c_str());
	return ExcuteSQL(pDBConn , &CallGetAddressBookItem, strSQL, (void*)(pAddrBook));
}

BOOL CSqliteDeal::GetAddressBookList(const string &strCond, map<string,uistruct::ADDRBOOK_t>* pAddrBookMap)
{
	sqlite3 ** pDBConn = GetDBConnect(); //获取数据库连接
	string strSQL("");
	strSQL =strprintf("SELECT * FROM t_address_book WHERE %s",(LPSTR)(LPCTSTR)strCond.c_str());
	return ExcuteSQL(pDBConn , &CallGetAddressBookList, strSQL, (void*)(pAddrBookMap));
}


BOOL CSqliteDeal::GetTransactionItem(const string &strCond, uistruct::REVTRANSACTION_t *pTxItem)
{
	sqlite3 ** pDBConn = GetDBConnect(); //获取数据库连接
	string strSQL("");
	strSQL=strprintf("SELECT * FROM t_transaction WHERE %s",(LPSTR)(LPCTSTR)strCond.c_str());
	return ExcuteSQL(pDBConn , &CallGetTransactionItem, strSQL, (void*)(pTxItem));
}

BOOL CSqliteDeal::GetTransactionList(const string &strCond, uistruct::TRANSRECORDLIST *pTxList)
{
	sqlite3 ** pDBConn = GetDBConnect(); //获取数据库连接
	string strSQL("");
	strSQL= strprintf("SELECT * FROM t_transaction WHERE %s",(LPSTR)(LPCTSTR)strCond.c_str());
	return ExcuteSQL(pDBConn , &CallGetTransactionList, strSQL, (void*)(pTxList));
}


BOOL CSqliteDeal::IsExistTx(string tablename,string filed ,string txhash){
	LOCK(m_pCs);
	string strCommand,strShowData;
	strCommand = strprintf("%s %s",_T("gettxdetail") ,txhash );
	CSoyPayHelp::getInstance()->SendRpc(strCommand,strShowData);
	BOOL flag = true;
	if (strShowData == _T("")){
		flag = false;
	}
	if(strShowData.find("hash") < 0){		
		flag = false;
	}
	if (!flag)
	{
		string strCond;
		strCond = strprintf(" %s='%s' ", filed, txhash);
		if (DeleteTableItem(tablename, strCond))
		{
			return flag;
		}
		
	}
	return flag;
}

void CSqliteDeal::CheckFailedCode(int retCode)
{
	if(retCode == 11)
	{
		if ( NULL != m_pSqliteWrite ) {
			sqlite3_close(m_pSqliteWrite);
			m_pSqliteWrite = NULL ;
		}
		if( NULL != m_pCs ) {
			delete m_pCs;
			m_pCs = NULL;
		}
		CString strFullPath(_T(""));
		strFullPath.Format(_T("%s\\db\\data.db") , theApp.m_strInsPath.c_str() );
		//strFullPath.Format(_T("%s\\data.db") , theApp.dbpath); 
		if(!DeleteFile((LPCTSTR)strFullPath))
			LogPrint("INFO", "删除数据库文件失败：%s\n", strFullPath);
		exit(-1);
	}

}

void  CSqliteDeal::UpdataAllTableData(){

	//LOCK(m_pCs);
	///// 不用更新所所有的数据库表

}


int CallTableItem(void *para, int n_column, char ** column_value, char ** column_name)
{

	if(NULL == column_value[0])
		return 0;

	for (int i = 0;i<n_column;i++)
	{
		if (strcmp((char*)para ,column_name[0]) == 0)
		{
			return 1;
		}
	}
	return 0;
}

BOOL CSqliteDeal::IsExistField(const string tablename,const string filed , const string &strCond)
{
	sqlite3 ** pDBConn = GetDBConnect(); //获取数据库连接
	string strSQL("");
	strSQL =strprintf("select * from sqlite_master where type='table'and name='%s'",tablename);


	BOOL isExist = false;
	char **pazResult;
	char *zErrMsg = NULL;
	int nRow, nCol;
	int rc = sqlite3_get_table(*pDBConn, strSQL.c_str(), &pazResult, &nRow, &nCol, &zErrMsg);
	if( rc!=SQLITE_OK )
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		return FALSE;
	}
	if (nCol<=0 || nRow<=0)
	{
		sqlite3_free_table(pazResult);
		return isExist;
	}
	string strValue ;
	strValue = strprintf("%s" , pazResult[nCol+nCol-1] ) ;
    
	int pos = strValue.find("(");
	strValue = strValue.substr(pos+1,strValue.length());

	pos = strValue.find(",");
	string keyValue="";
	while(pos >=0)
	{
		keyValue = strValue.substr(0,pos);
		UiFun::trimleft(keyValue);
		int index = keyValue.find(" ");
		if (index >=0)
		{
			keyValue=keyValue.substr(0,index);
			UiFun::trimleft(keyValue);
			UiFun::trimright(keyValue);
			if (strcmp(keyValue.c_str(),filed.c_str()) == 0)
			{
				isExist = TRUE;
				break;
			}
		}
		strValue=strValue.substr(pos+1,strValue.length());
		pos = strValue.find(",");
	}


	keyValue = strValue.substr(0,pos);
	UiFun::trimleft(keyValue);
	pos = keyValue.find(" ");
	if (!isExist &&pos >=0)
	{
		keyValue=keyValue.substr(0,pos);
		UiFun::trimright(keyValue);
		if (strcmp(keyValue.c_str(),filed.c_str()) == 0)
		{
			isExist = TRUE;
		}
	}

	sqlite3_free_table(pazResult);
	return isExist;
}

BOOL CSqliteDeal::DeleteTable(const string tablename){
	sqlite3 ** pDBConn = GetDBConnect(); //获取数据库连接
	string strSQL("");
	strSQL = strprintf("drop table %s",(LPSTR)(LPCTSTR)tablename.c_str());
	return ExcuteSQL(pDBConn , &CallTableItem, strSQL, NULL);
}

//获取tip block hash
int CallGetTipBlockHash(void *para, int n_column, char ** column_value, char ** column_name)
{
	uistruct::CHAIN_TIP_T * pitem =  (uistruct::CHAIN_TIP_T *)para;
	if(NULL == column_value[0])
		return -1;
	if(n_column != 5)
	{
		return -1;
	}
	pitem->blockhash =strprintf("%s",column_value[0]);
	pitem->block_height =atoi(column_value[1]);
	pitem->confired_time = atoi(column_value[2]);
	pitem->fuelrate = atoi(column_value[3]);
	pitem->lock = atoi(column_value[4]);
	return 0;
}

int CSqliteDeal::GetTipBlockHash(uistruct::CHAIN_TIP_T *pBlockHash)
{
	sqlite3 ** pDBConn = GetDBConnect(); //获取数据库连接

	string strSQL="";
	strSQL = "SELECT * FROM t_chain_tip  ";
	ExcuteSQL(pDBConn, &CallGetTipBlockHash, strSQL, (void *)(pBlockHash));
	return 0;
}