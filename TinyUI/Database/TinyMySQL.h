#pragma once
#include "../Common/TinyCommon.h"
#include "../Common/TinyTime.h"
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "TinyDatabase.h"
#include <string>
#include <sstream>
#include <mysql.h>
#pragma comment(lib,"libmysql.lib");
using namespace std;

namespace TinyUI
{
	class MySqlConnection;
	class MySqlCommand;
	class MySqlDataReader;
	class MySqlParameter;

	typedef struct st_mysql	MYSQL;
	typedef struct st_mysql_stmt MYSQL_STMT;

	class MySqlConnection : public IDbConnection
	{
	public:
		MySqlConnection();
		virtual ~MySqlConnection();
	public:

	public:
		LPCSTR GetConnectionString() const OVERRIDE;
		void SetConnectionString(LPCSTR pzText) OVERRIDE;
		LONG GetConnectionTimeout() const OVERRIDE;
		void SetConnectionTimeout(LONG timeout) OVERRIDE;
		BOOL Open() OVERRIDE;
		BOOL Close() OVERRIDE;
		LONG GetConnectionState() OVERRIDE;
		BOOL BeginTransaction(INT iIsolationLevel = 0) OVERRIDE;
		BOOL CommitTransaction() OVERRIDE;
		BOOL RollbackTransaction() OVERRIDE;
		void GetLatestError(string& error) OVERRIDE;
	private:
		BOOL Options(LPCSTR name, mysql_option opt, void* args);
	private:
		string		m_connectionString;
		MYSQL*		m_mysql;
		LONG		m_connectionState;
		LONG		m_timeout;
	};
	//////////////////////////////////////////////////////////////////////////
	class MySqlCommand : public IDbCommand
	{
	public:
		MySqlCommand(MySqlConnection& connection);
		virtual ~MySqlCommand();
	public:
		LPCSTR GetCommandText() const OVERRIDE;
		void SetCommandText(LPCSTR pzText) OVERRIDE;
		LONG GetCommandTimeout() const OVERRIDE;
		void SetCommandTimeout(LONG time) OVERRIDE;
		INT GetCommandType() OVERRIDE;
		void SetCommandType(INT commandType) OVERRIDE;
		IDbConnection* GetConnection() OVERRIDE;
		BOOL Add(IDbDataParameter* ps) OVERRIDE;
		BOOL Remove(INT i) OVERRIDE;
		BOOL Remove(LPCSTR pzName) OVERRIDE;
		BOOL RemoveAll() OVERRIDE;
		INT ExecuteNonQuery() OVERRIDE;
		BOOL ExecuteReader(IDbDataReader* ps) OVERRIDE;
		BOOL Cancel() OVERRIDE;
	private:
		BOOL	BindParameter(MYSQL_STMT* statement, IDbDataParameter* param);
		BOOL	BindParameters(MYSQL_STMT* statement);
	private:
		string							m_commandText;
		INT								m_commandType;
		LONG							m_timeout;
		MySqlConnection&				m_connection;
		TinyArray<IDbDataParameter*>	m_parameters;
	};
	//////////////////////////////////////////////////////////////////////////
	class MySqlDataParameter : public IDbDataParameter
	{
	public:
		MySqlDataParameter();
		virtual ~MySqlDataParameter();
	public:
		INT GetDbType() OVERRIDE;
		void SetDbType(INT dbTye) OVERRIDE;
		INT GetDirection() OVERRIDE;
		void SetDirection(INT direction) OVERRIDE;
		BOOL IsNullable() OVERRIDE;
		LPCSTR GetParameterName() OVERRIDE;
		void SetParameterName(LPCSTR pzName) OVERRIDE;
		BYTE GetPrecision() OVERRIDE;
		void SetPrecision(BYTE precision) OVERRIDE;
		BYTE GetScale() OVERRIDE;
		void SetScale(BYTE scale) OVERRIDE;
		INT GetSize() OVERRIDE;
		void SetSize(INT size) OVERRIDE;
		void SetNull() OVERRIDE;
		void SetBoolean(BOOL val) OVERRIDE;
		void SetByte(BYTE val) OVERRIDE;
		void SetChar(CHAR val) OVERRIDE;
		void SetBlob(LPBYTE val, LONG size) OVERRIDE;
		void SetDateTime(DATE val) OVERRIDE;
		void SetDecimal(DECIMAL val) OVERRIDE;
		void SetDouble(DOUBLE val) OVERRIDE;
		void SetFloat(FLOAT val) OVERRIDE;
		void SetInt16(SHORT val) OVERRIDE;
		void SetInt32(INT val) OVERRIDE;
		void SetInt64(LONG val) OVERRIDE;
		void SetString(LPCSTR val) OVERRIDE;
		BOOL GetBoolean() OVERRIDE;
		BYTE GetByte() OVERRIDE;
		CHAR GetChar() OVERRIDE;
		INT GetBlob(BYTE* ps) OVERRIDE;
		DATE GetDateTime() OVERRIDE;
		DECIMAL GetDecimal() OVERRIDE;
		DOUBLE GetDouble() OVERRIDE;
		FLOAT GetFloat() OVERRIDE;
		SHORT GetInt16() OVERRIDE;
		INT GetInt32() OVERRIDE;
		LONG GetInt64() OVERRIDE;
		LPCSTR GetString() OVERRIDE;
	private:
		MYSQL_BIND	m_bind;
	};
	//////////////////////////////////////////////////////////////////////////
	class MySqlDataReader : public IDbDataReader
	{
	public:
		MySqlDataReader();
		virtual ~MySqlDataReader();
	public:
		BOOL ReadNext() OVERRIDE;
		BOOL ReadPrevious() OVERRIDE;
		BOOL ReadFirst() OVERRIDE;
		BOOL ReadLast() OVERRIDE;
		BOOL Close() OVERRIDE;
		INT GetColumnCount() OVERRIDE;
		BOOL GetBoolean(INT i) OVERRIDE;
		BYTE GetByte(INT i) OVERRIDE;
		CHAR GetChar(INT i) OVERRIDE;
		INT GetBlob(INT i, BYTE* ps) OVERRIDE;
		LPCSTR GetDataTypeName(INT i) OVERRIDE;
		DATE GetDateTime(INT i) OVERRIDE;
		DECIMAL GetDecimal(INT i) OVERRIDE;
		DOUBLE GetDouble(INT i) OVERRIDE;
		FLOAT GetFloat(INT i) OVERRIDE;
		SHORT GetInt16(INT i) OVERRIDE;
		INT GetInt32(INT i) OVERRIDE;
		LONG GetInt64(INT i) OVERRIDE;
		LPCSTR GetName(INT i) OVERRIDE;
		INT GetOrdinal(LPCSTR pzName) OVERRIDE;
		LPCSTR GetString(INT i) OVERRIDE;
		BOOL IsDBNull(INT i) OVERRIDE;
	private:

	};
}