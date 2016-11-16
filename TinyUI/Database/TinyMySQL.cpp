#include "../stdafx.h"
#include "TinyMySQL.h"
using namespace std;

namespace TinyUI
{
	//MySqlConnection::MySqlConnection()
	//	:m_mysql(NULL),
	//	m_connectionState(0),
	//	m_timeout(0)
	//{

	//}
	//MySqlConnection::~MySqlConnection()
	//{
	//	Close();
	//}

	//LPCSTR MySqlConnection::GetConnectionString() const
	//{
	//	return m_connectionString.c_str();
	//}

	//void MySqlConnection::SetConnectionString(LPCSTR pzText)
	//{
	//	m_connectionString = pzText;
	//}

	//LONG MySqlConnection::GetConnectionTimeout() const
	//{
	//	return m_timeout;
	//}

	//void MySqlConnection::SetConnectionTimeout(LONG timeout)
	//{
	//	m_timeout = timeout;
	//}

	//BOOL MySqlConnection::Open()
	//{
	//	m_mysql = mysql_init(NULL);
	//	if (!m_mysql)
	//		return FALSE;
	//	if (!Options(m_mysql, MYSQL_OPT_CONNECT_TIMEOUT, m_timeout))
	//		return FALSE;
	//	std::map<std::string, std::string> options;
	//	options["host"] = "localhost";
	//	options["port"] = "3306";
	//	options["user"] = "";
	//	options["password"] = "";
	//	options["db"] = "";
	//	options["compress"] = "";
	//	options["auto-reconnect"] = "";
	//	options["secure-auth"] = "";
	//	options["character-set"] = "utf8";
	//	const std::string& str = GetConnectionString();
	//	SplitString2(str, ";", "=", options);
	//	INT port = 0;
	//	istringstream(options["port"]) >> port;
	//	if (mysql_real_connect(m_mysql, options["host"], options["user"], options["password"], options["db"], port, NULL, 0) != 0)
	//		return FALSE;
	//	m_connectionState = 1;
	//	return TRUE;
	//}

	//BOOL MySqlConnection::Close()
	//{
	//	if (!m_mysql)
	//		return FALSE;
	//	mysql_close(m_mysql);
	//	m_connectionState = 0;
	//	return TRUE;
	//}

	//LONG MySqlConnection::GetConnectionState()
	//{
	//	return m_connectionState;
	//}

	//BOOL MySqlConnection::BeginTransaction(INT iIsolationLevel /*= 0*/)
	//{
	//	if (!m_mysql)
	//		return FALSE;
	//	INT iRes = mysql_autocommit(m_mysql, FALSE);
	//	if (iRes != 0)
	//	{
	//		iRes = mysql_errno(m_mysql);
	//		if (iRes == 2006 || iRes == 2013)
	//		{
	//			iRes = mysql_autocommit(m_mysql, FALSE);
	//		}
	//	}
	//	if (iRes != 0)
	//		return FALSE;
	//	return TRUE;
	//}

	//BOOL MySqlConnection::CommitTransaction()
	//{
	//	if (!m_mysql)
	//		return FALSE;
	//	return mysql_commit(m_mysql) == 0;
	//}

	//BOOL MySqlConnection::RollbackTransaction()
	//{
	//	if (!m_mysql)
	//		return FALSE;
	//	return mysql_rollback(m_mysql) == 0;
	//}

	//void MySqlConnection::GetLatestError(string& error)
	//{
	//	error = mysql_error();
	//}
	//BOOL MySqlConnection::Options(LPCSTR name, mysql_option opt, void* args)
	//{
	//	if (!m_mysql)
	//		return FALSE;
	//	return mysql_options(m_mysql, opt, args) == 0;
	//}
	////////////////////////////////////////////////////////////////////////////
	//MySqlCommand::MySqlCommand(MySqlConnection& connection)
	//	:m_timeout(0),
	//	m_commandType(0),
	//	m_connection(connection)
	//{

	//}
	//MySqlCommand::~MySqlCommand()
	//{

	//}
	//LPCSTR MySqlCommand::GetCommandText() const
	//{
	//	return m_commandText.c_str();
	//}

	//void MySqlCommand::SetCommandText(LPCSTR pzText)
	//{
	//	m_commandText = pzText;
	//}

	//LONG MySqlCommand::GetCommandTimeout() const
	//{
	//	return m_timeout;
	//}

	//void MySqlCommand::SetCommandTimeout(LONG time)
	//{
	//	m_timeout = time;
	//}

	//INT MySqlCommand::GetCommandType()
	//{
	//	return m_commandType;
	//}

	//void MySqlCommand::SetCommandType(INT commandType)
	//{
	//	m_commandType = commandType;
	//}

	//TinyUI::IDbConnection* MySqlCommand::GetConnection()
	//{
	//	return &m_connection;
	//}

	//BOOL MySqlCommand::Add(IDbDataParameter* ps)
	//{
	//	return m_parameters.Add(value);
	//}

	//BOOL MySqlCommand::Remove(INT i)
	//{
	//	return m_parameters.RemoveAt(i);
	//}

	//BOOL MySqlCommand::Remove(LPCSTR pzName)
	//{
	//	for (INT i = 0; i < m_parameters.GetSize(); i++)
	//	{
	//		if (!strcmp(m_parameters[i]->GetParameterName(), pzName))
	//		{
	//			return m_parameters.RemoveAt(i);
	//		}
	//	}
	//	return FALSE;
	//}

	//BOOL MySqlCommand::RemoveAll()
	//{
	//	m_parameters.RemoveAll();
	//	return TRUE;
	//}

	//INT MySqlCommand::ExecuteNonQuery()
	//{

	//}

	//BOOL MySqlCommand::ExecuteReader(IDbDataReader* ps)
	//{

	//}

	//BOOL MySqlCommand::Cancel()
	//{
	//	return TRUE;
	//}
	//BOOL MySqlCommand::BindParameter(MYSQL_STMT* statement, IDbDataParameter* param)
	//{
	//	MYSQL_BIND s = { 0 };
	//	s.buffer_type = (enum_field_types)param->GetDbType();
	//	s.buffer = const_cast<void*>(buffer);
	//	s.buffer_length = length;
	//	if (mysql_stmt_bind_param(statement, &s) != 0)
	//		return FALSE;
	//	return TRUE;
	//}
	//BOOL MySqlCommand::BindParameters(MYSQL_BIND* statement)
	//{
	//	TinyScopedArray<MYSQL_BIND>	binds(new MYSQL_BIND[m_parameters.GetSize()]);
	//	for (INT i = 0; i < m_parameters.GetSize(); i++)
	//	{
	//		IDbDataParameter* parameter = m_parameters[i];
	//		binds[i].buffer_type = (enum_field_types)parameter->GetDbType();

	//	}
	//	return TRUE;
	//}
	////////////////////////////////////////////////////////////////////////////
	//MySqlDataParameter::MySqlDataParameter()
	//{

	//}
	//MySqlDataParameter::~MySqlDataParameter()
	//{

	//}
	//INT MySqlDataParameter::GetDbType()
	//{
	//	return (INT)m_bind.buffer_type;
	//}

	//void MySqlDataParameter::SetDbType(INT dbTye)
	//{
	//	m_bind.buffer_type = (enum_field_types)dbTye;
	//}

	//INT MySqlDataParameter::GetDirection()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetDirection(INT direction)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//BOOL MySqlDataParameter::IsNullable()
	//{
	//	return m_bind.buffer_type == MYSQL_TYPE_NULL;
	//}

	//LPCSTR MySqlDataParameter::GetParameterName()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetParameterName(LPCSTR pzName)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//BYTE MySqlDataParameter::GetPrecision()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetPrecision(BYTE precision)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//BYTE MySqlDataParameter::GetScale()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetScale(BYTE scale)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//INT MySqlDataParameter::GetSize()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetSize(INT size)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetNull()
	//{

	//}

	//void MySqlDataParameter::SetBoolean(BOOL val)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetByte(BYTE val)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetChar(CHAR val)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetBlob(LPBYTE val, LONG size)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetDateTime(DATE val)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetDecimal(DECIMAL val)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetDouble(DOUBLE val)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetFloat(FLOAT val)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetInt16(SHORT val)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetInt32(INT val)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetInt64(LONG val)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//void MySqlDataParameter::SetString(LPCSTR val)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//BOOL MySqlDataParameter::GetBoolean()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//BYTE MySqlDataParameter::GetByte()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//CHAR MySqlDataParameter::GetChar()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//INT MySqlDataParameter::GetBlob(BYTE* ps)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//DATE MySqlDataParameter::GetDateTime()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//DECIMAL MySqlDataParameter::GetDecimal()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//DOUBLE MySqlDataParameter::GetDouble()
	//{
	//	if (m_bind.buffer_type == MYSQL_TYPE_DOUBLE)
	//		return reinterpret_cast<LPCSTR>(m_bind.buffer);
	//}

	//FLOAT MySqlDataParameter::GetFloat()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//SHORT MySqlDataParameter::GetInt16()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//INT MySqlDataParameter::GetInt32()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//LONG MySqlDataParameter::GetInt64()
	//{
	//	if (m_bind.buffer_type == MYSQL_TYPE_LONGLONG)
	//		return reinterpret_cast<LPCSTR>(m_bind.buffer);
	//}

	//LPCSTR MySqlDataParameter::GetString()
	//{
	//	if (m_bind.buffer_type == MYSQL_TYPE_STRING ||
	//		m_bind.buffer_type == MYSQL_TYPE_VAR_STRING)
	//		return reinterpret_cast<LPCSTR>(m_bind.buffer);
	//}
	////////////////////////////////////////////////////////////////////////////
	//BOOL MySqlDataReader::ReadNext()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//BOOL MySqlDataReader::ReadPrevious()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//BOOL MySqlDataReader::ReadFirst()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//BOOL MySqlDataReader::ReadLast()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//BOOL MySqlDataReader::Close()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//INT MySqlDataReader::GetColumnCount()
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//BOOL MySqlDataReader::GetBoolean(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//BYTE MySqlDataReader::GetByte(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//CHAR MySqlDataReader::GetChar(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//INT MySqlDataReader::GetBlob(INT i, BYTE* ps)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//LPCSTR MySqlDataReader::GetDataTypeName(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//DATE MySqlDataReader::GetDateTime(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//DECIMAL MySqlDataReader::GetDecimal(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//DOUBLE MySqlDataReader::GetDouble(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//FLOAT MySqlDataReader::GetFloat(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//SHORT MySqlDataReader::GetInt16(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//INT MySqlDataReader::GetInt32(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//LONG MySqlDataReader::GetInt64(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//LPCSTR MySqlDataReader::GetName(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//INT MySqlDataReader::GetOrdinal(LPCSTR pzName)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//LPCSTR MySqlDataReader::GetString(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

	//BOOL MySqlDataReader::IsDBNull(INT i)
	//{
	//	throw std::logic_error("The method or operation is not implemented.");
	//}

}