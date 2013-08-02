/* $RCSfile: JTXDataManager.java,v $
 * $Revision: 1.6 $
 * $Author: Gambineri $
 * $Date: 2009/02/27 10:35:51 $
 *
 * developed by Massimo Gambineri
 * ._..-_..-._-._..- ..-._-._- ._..-_..-._-._..- ..-._-._- ..-._-.
 */

package jtxlib.main.sql;


import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Calendar;
import java.util.GregorianCalendar;

import jtxlib.main.aux.Debug;
import jtxlib.main.aux.Tools;



/* ----------------------------------------------------- */
/*                    JTXDataManager                     */
/* ----------------------------------------------------- */


public class DataManager
{
  public interface StatementType
  {
    public final byte SELECT = 0;
    public final byte INSERT = 1;
    public final byte UPDATE = 2;
    public final byte DELETE = 3;
    public final byte OTHER  = 4;
  }

  Debug        m_Dbg = new Debug();
  StatementType   m_StmType;

  private Statement   m_SelectStatement;
  private ResultSet   m_Data;

//---------------------------------------------------------------
  public DataManager()
  {
  }

//---------------------------------------------------------------
  public DataManager(DBConnection conn)
  {
    try
    {
      m_SelectStatement = conn.m_Conn.createStatement();
    }
    catch(SQLException sqle)
    {
	    Debug.Dbg("JTXDataManager(JTXConnection conn): SQLException: " + sqle.getMessage());
	    if (conn.m_Conn == null)
	      Debug.Dbg("conn.m_Conn is null.");
	    sqle.printStackTrace();
		}
  }

//---------------------------------------------------------------
  public void close()
  {
    try
    {
      m_SelectStatement.getConnection().commit();
      m_SelectStatement.close();
    }
    catch(SQLException sqle)
    {
		  Debug.Dbg("JTXDataManager.close: SQLException: " + sqle.getMessage());
    }
  }

//---------------------------------------------------------------
  public ResultSet executeStatement(String selectStatement, byte queryType) throws SQLException
  {
    if ((queryType == StatementType.INSERT) || (queryType == StatementType.UPDATE) || (queryType == StatementType.DELETE))
    {
      try
      {
			  m_SelectStatement.executeUpdate(selectStatement);
      }
      catch(SQLException sqle)
      {
			  Debug.Dbg("JTXDataManager.executeUpdate: SQLException: " + sqle.getMessage() + "\r\nQuery:\r\n" + selectStatement);
  	    throw sqle;
		  }
    }
    else if (queryType == StatementType.SELECT)
    {
      try
      {
		    m_Data = m_SelectStatement.executeQuery(selectStatement);
      }
      catch(SQLException sqle)
      {
		    Debug.Dbg("JTXDataManager.executeQuery: SQLException: " + sqle.getMessage() + "\r\nQuery:\r\n" + selectStatement);
  	    throw sqle;
		  }
		}
    else if (queryType == StatementType.OTHER)
    {
      try
      {
		    m_SelectStatement.execute(selectStatement);
      }
      catch(SQLException sqle)
      {
		    Debug.Dbg("JTXDataManager.execute: SQLException: " + sqle.getMessage() + "\r\nQuery:\r\n" + selectStatement);
  	    throw sqle;
		  }
		}

	  return m_Data;
	}

//---------------------------------------------------------------
  public String getMaxID(String tableName, String idname) throws SQLException //RESTITUISCE MAXID + 1
  {
    String ret = "1";
    try
    {
      ResultSet rs = m_SelectStatement.executeQuery("SELECT (MAX(" + idname + ") + 1) AS maxid FROM " + tableName);
      if (rs != null)
      {
        rs.next();
        ret = rs.getString("maxid");
      }
    }
    catch(SQLException sqle)
    {
	    Debug.Dbg("JTXDataManager-getMaxID: SQLException: " + sqle.getMessage());
	    throw sqle;
	  }

    if (ret == null)
      ret = "1";

    return ret;
  }

//---------------------------------------------------------------
  public byte adjVal(String fieldName, byte defaultVal) throws Exception
  {
    //wasNull() method works on the last retrieved field value, so
    //it's better to fetch it here and not outside, to prevent side effects
    byte retVal = m_Data.getByte(fieldName);
    return (m_Data.wasNull() ? defaultVal : retVal);
  }

//---------------------------------------------------------------
  public String adjVal(String fieldName, String defaultVal) //throws Exception
  {
    //wasNull() method works on the last retrieved field value, so
    //it's better to fetch it here and not outside, to prevent side effects
    try
    {
      String retVal = m_Data.getString(fieldName);
      return (m_Data.wasNull() ? defaultVal : retVal);
    }
    catch(SQLException se)
    {
    		Debug.Dbg("Current field is: " + fieldName + "\n");
//  		m_Dbg.Dbg("SQLException: " + se.getMessage() + " - error code: " + se.getErrorCode() + " SQLState: " + se.getSQLState());
	  }

		return defaultVal;
  }

//---------------------------------------------------------------
  public String adjTxt(String fieldName, String defaultVal)// throws Exception
  {
    //wasNull() method works on the last retrieved field value, so
    //it's better to fetch it here and not outside, to prevent side effects
    try
    {
      String retVal = m_Data.getString(fieldName);
      return (m_Data.wasNull() ? defaultVal : adjQuotes(retVal));
    }
    catch(SQLException se)
    {
    		Debug.Dbg("Current field is: " + fieldName + "\n");
//  		m_Dbg.Dbg("SQLException: " + se.getMessage() + " - error code: " + se.getErrorCode() + " SQLState: " + se.getSQLState());
		}

		return defaultVal;
  }

//---------------------------------------------------------------
  public String adjQuotes(String in)
  {
    return Tools.SQLQuotesEnclose(in);
  }

//---------------------------------------------------------------
  public String defaultVal()
  {
    String defaultDate;
    java.util.Date today = new java.util.Date();

    Calendar calendar = new GregorianCalendar();
    calendar.setTime(today);

    defaultDate="'"+calendar.get(Calendar.YEAR)+"-"+calendar.get(Calendar.MONTH)+"-"+calendar.get(Calendar.DATE)+"'";
    return defaultDate;
  }

//---------------------------------------------------------------
  public String adjDate(String fieldName, String defaultVal) throws Exception
  {
     //wasNull() method works on the last retrieved field value, so
    //it's better to fetch it here and not outside, to prevent side effects
    String retVal = "'"+m_Data.getString(fieldName)+"'";
    return (m_Data.wasNull() ? defaultVal : retVal);
  }

//---------------------------------------------------------------
  public String adjDate(String fieldName) throws Exception
  {
     //wasNull() method works on the last retrieved field value, so
    //it's better to fetch it here and not outside, to prevent side effects
    String retVal = "'"+m_Data.getString(fieldName)+"'";
    return (m_Data.wasNull() ? "CONVERT(varchar(10), getdate(), 120)" : retVal);
  }
} //END OF class JTXDataManager

