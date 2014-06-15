/* 
 * Copyright (c) 2013 Massimo Gambineri
 * See the file LICENSE for copying permission.
 * */

package jtxlib.main.sql;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

import jtxlib.main.aux.Debug;

/* ----------------------------------------------------- */
/* ***************** Auxiliary classes ***************** */
/* ----------------------------------------------------- */

public class DBConnection
{
  protected Connection  m_Conn = null;
  private   String      m_DSN  = "";
  private   String      m_USR  = "";
  private   String      m_PWD  = "";
    
  //----------------------------------------------------------------------------
  public Connection getConnection()
  {
    return m_Conn;
  }

  //----------------------------------------------------------------------------
  public boolean isValid()
  {
    try {return (m_Conn != null && (!m_Conn.isClosed()));} 
    catch (SQLException ex){ex.printStackTrace();}
    return false;
  }

  //----------------------------------------------------------------------------
  public void close()
  {
    try
    {
      if (m_Conn != null)
        m_Conn.close();
    }
    catch(SQLException sqle)
    {
      Debug.Dbg("JTXConnection: SQLException: " + sqle.getMessage());
    }
  }

  //---CONSTRUCTORS
  //----------------------------------------------------------------------------
  public DBConnection(String dsn, String user, String pwd)
  {
    this("sun.jdbc.odbc.JdbcOdbcDriver", "jdbc:odbc:" +  dsn, user, pwd);
    m_DSN  = dsn;
    m_USR  = user;
    m_PWD  = pwd;
  }

  //----------------------------------------------------------------------------
  public DBConnection(String driverclass, String drivername, String user, String passwd)
  {
    try
    {
  		Class.forName(driverclass);
  		m_Conn = DriverManager.getConnection(drivername, user, passwd);
//        m_Conn.setAutoCommit(false); //enables transaction mechanism
  	}
    catch(SQLException sqle)
    {
			Debug.Dbg("JTXConnection: SQLException: " + sqle.getMessage());
		}
    catch(ClassNotFoundException e)
    {
  		Debug.Dbg("JTXConnection: ClassNotFoundException: " + e.getMessage());
		}
  }

  //----------------------------------------------------------------------------
  public DBConnection(String driverclass, String drivername)
  {
    try
    {
  		Class.forName(driverclass);
  		m_Conn = DriverManager.getConnection(drivername);
  	}
    catch(SQLException sqle)
    {
			Debug.Dbg("JTXConnection: SQLException: " + sqle.getMessage());
		}
    catch(ClassNotFoundException e)
    {
  		Debug.Dbg("JTXConnection: ClassNotFoundException: " + e.getMessage());
		}
  }

  //GETTER----------------------------------------------------------------------
  public String getDSN() {return m_DSN;}
  public String getUser() {return m_USR;}
  public String getPwd() {return m_PWD;}
}


