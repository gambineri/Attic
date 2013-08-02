/* 
 * Copyright (c) 2013 Massimo Gambineri
 * See the file LICENSE for copying permission.
 * */

package jtxlib.main.web;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.StringTokenizer;

import jtxlib.main.aux.Tools;
import jtxlib.main.sql.DBConnection;
import jtxlib.main.sql.DataManager;
import jtxlib.main.sql.RecordSet;


/**
 * This class is the common class used to assign application 
 * level rights to the users. These rights are pushed in 
 * memory once per session.
 * Based on SYSSECU, SYSFUNC, SYSPRIVILEG tables: SYSSECU contains
 * the list of users/pwd; SYSPRIVILEG the associated functionalities
 * together with the authorization levels in the range [0, 100].
 * SYSFUNC is a lookup table to store the extend description
 * of the functionalities.
 */
public class UserAuth
{
  //Members
  private boolean                   m_IsLoaded = false;
  private boolean                   m_UserAuthorization = false;
  private Hashtable<String, String> m_AuthLevels = new Hashtable<String, String>();
  private String                    m_FAOAuthServiceURL = "";
  private String                    m_JDBCDriver = "";
  private String                    m_JDBCDatabase = "";
  private String                    m_Usr = "";
  
  // Constants
  private final String              RECORD_ACCESS_FUNC_TAG = "_RA_";

  /**
   * Basic constructor
   */
  public UserAuth()
  {
  }

  //----------------------------------------------------------------------------
  public void Init(String jdbcdriver, String jdbcdatabase, String faoauthserviceurl)
  {
    if (m_IsLoaded)
      return;
    
    m_FAOAuthServiceURL = (faoauthserviceurl == null ? "" : faoauthserviceurl);
    m_JDBCDriver        = (jdbcdriver == null ? "" : jdbcdriver);
    m_JDBCDatabase      = (jdbcdatabase == null ? "" : jdbcdatabase);
    
    if (m_JDBCDriver.length() > 0 && m_JDBCDatabase.length() > 0)
      m_IsLoaded = true;
  }
  
  //----------------------------------------------------------------------------
  /**
   * Main login function: the real login can be managed 
   * internally by the class itself or via the FAOAuth servlet
   * (needed by all FAO intranet secured pages).
   * @param UserIndex username
   * @param Pwd user password
   */
  public void LogIn(String UserIndex, String Pwd)
  {
    if (m_FAOAuthServiceURL.length() == 0)
      ApplicationManagedLogIn(UserIndex, Pwd);
    else
      FAOAuthLogIn(UserIndex, Pwd);
  }

  //----------------------------------------------------------------------------
  /* Login implementation to support Authenticated Session Inheritance
   */
  public void LogIn(String UserIndex, String Pwd, String sid, String client_ipaddr)
  {
    UserIndex = (UserIndex == null ? "" : UserIndex);
    Pwd       = (Pwd == null ? "" : Pwd);

    // If username is empty, this could be a call for ASI method
    if (UserIndex.length() == 0)
      GetASI(sid, client_ipaddr);
    else  // Go ahead with normal procedure, even possibly an ASI initalization
    {
      LogIn(UserIndex, Pwd);
      SetASI(UserIndex, sid, client_ipaddr);
    }
  }

  //----------------------------------------------------------------------------
  /**
   * Called whenever an authorization level is required in the code
   * @param modulename the functionality name for which the 
   * authorization is required.
   * @return true if the user is authorized, false otherwise
   */
  public boolean CheckAuth(String modulename)
  {
    if (modulename == null || modulename.length() == 0)
      return m_UserAuthorization;

    boolean auth = false;
    if (m_UserAuthorization)
    {
      //  Authorization levels:
      //  100 : full control
      //    0 : not authorized
      String authlevel = (String)m_AuthLevels.get(modulename);
      if (authlevel != null)
        auth = authlevel.equals("100");
    }

    return auth;
  }

  //----------------------------------------------------------------------------
  /**
   * Called whenever an authorization level for record access is required 
   * in the code: ALL FUNCTIONALITY NAMES MUST CONTAIN '_RA_'.
   * @param ra_privil_f the name of field used to store the specific 
   *        record access functionality value.
   * @return a string with a sql filter of OR-ed RA conditions:<br>
   * <code>(ra_privil = functionality01 OR ra_privil = functionality01 ...)</code>.
   */
  public String GetRecordAccessFilter(String ra_privil_f)
  {
    String ret = ra_privil_f + "='' OR " + ra_privil_f + " IS NULL OR";
    
    if (ra_privil_f == null || ra_privil_f.length() == 0)
      return "";

    if (m_UserAuthorization)
    {
      Enumeration<String> e = m_AuthLevels.keys();
      String funcname = "";
      while (e.hasMoreElements())
      {
        funcname = (String)e.nextElement();
        if (funcname.indexOf(RECORD_ACCESS_FUNC_TAG) != -1)
          if (((String)m_AuthLevels.get(funcname)).equals("100"))
            ret += " " + ra_privil_f + " = '" + funcname + "' OR";
      }
      
      ret = ret.substring(0, ret.length()-2);
      ret = ret.trim();
      ret = "(" + ret + ")";
    }
    
    return ret;
  }

  //----------------------------------------------------------------------------
  /**
   *  Resets the current instance thus invalidating any previous existing login.
   */
  public void Invalidate()
  {
    m_IsLoaded = false;
    m_UserAuthorization = false;
    m_Usr = "";
    m_AuthLevels.clear();
    m_FAOAuthServiceURL = "";
    m_JDBCDriver = "";
    m_JDBCDatabase = "";
  }

  /*****************************************************************************
   *                            PRIVATE METHODS
   *****************************************************************************/

  //----------------------------------------------------------------------------
  private void GetASI(String sid, String client_ipaddr)
  {
    sid = (sid == null ? "" : sid);
    sid = sid.replace("\r", "");
    sid = sid.replace("\n", "");
    
    if (m_UserAuthorization || sid.length() == 0)
      return;

//    System.out.println("GetASI()");
//    System.out.println("sid: " + sid);

    DBConnection  conn = null;
    RecordSet   rs   = null;
    
    try
    {
      conn = new DBConnection(m_JDBCDriver, m_JDBCDatabase);
      rs   = new RecordSet(conn);
      rs.select("SELECT * FROM sysasi WHERE expired = 0 AND sid = '" + sid + "' AND client_ipaddr = '" + client_ipaddr + "'");
      if (rs.first()) // a record relative to the current sid was actually found
      {
        m_UserAuthorization = true;
        String uid = rs.getField("uid");
        m_Usr = uid;
        
//        System.out.println("UserIndex = " + uid + "\nSid = " + sid);
    
        // Extract user auths and store them into memory through hashtable
        GetCurrentUserPrivileges(rs, uid);
      }      
    }
    catch(Exception e) {e.printStackTrace();}
    finally 
    {
      if (rs != null) rs.close();
      if (conn != null) conn.close();
    }
  }

  //----------------------------------------------------------------------------
  /* This function tells this UserAuth object to use the Authenticated Session
   * Inheritance method.
   */
  private void SetASI(String UserIndex, String sid, String client_ipaddr)
  {
    sid = (sid == null ? "" : sid);
    client_ipaddr = (client_ipaddr == null ? "" : client_ipaddr);
    
    if ((!m_UserAuthorization) || sid.length() == 0 || client_ipaddr.length() == 0)
      return;
    
    DBConnection  conn = null;
    DataManager dm   = null;
    
    try
    {
      conn = new DBConnection(m_JDBCDriver, m_JDBCDatabase);
      dm   = new DataManager(conn);
  
      SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd-HHmmss");
      
      dm.executeStatement("INSERT INTO sysasi (sid, uid, client_ipaddr, logon_time) VALUES (" +
                          "'" + sid + "', '" + UserIndex + "', '" + client_ipaddr + "', '" + sdf.format(new Date()) + "')", 
                          DataManager.StatementType.INSERT);
    }
    catch(Exception e) {e.printStackTrace();}
    finally 
    {
      if (dm != null) dm.close();
      if (conn != null) conn.close();
    }
  }
  
  //----------------------------------------------------------------------------
  /**
   * Log in via the internal method (query the db for privileges)
   * @param UserIndex
   * @param Pwd 
   */
  private void ApplicationManagedLogIn(String UserIndex, String Pwd)
  {
    DBConnection conn = null;
    RecordSet jtxrs = null;
    
    try
    {
      m_UserAuthorization = false;

      if (UserIndex == null || Pwd == null)
        return;

      // Verify if the user is application-enabled
      conn = new DBConnection(m_JDBCDriver, m_JDBCDatabase);

      jtxrs = new RecordSet(conn);
      jtxrs.select("SELECT COUNT(*) FROM syssecu WHERE indx_num = '" +
                   UserIndex + "' AND password = '" + Pwd + "'", 1);

      String usercount = jtxrs.getField(0, 0);
      if (usercount != null && usercount.equals("1"))
      {
        m_UserAuthorization = true;
        m_Usr = UserIndex;
        // Extract user auths and store them into memory through hashtable
        GetCurrentUserPrivileges(jtxrs, UserIndex);
        //Update LOGON_DATE for this user
        jtxrs.executeStatement("UPDATE syssecu SET logon_date = '" +
                               Tools.date2DB(new java.util.Date()) + "' " +
                               "WHERE indx_num = '" + UserIndex + "'",
                               DataManager.StatementType.UPDATE);
      }      
    }
    catch(Exception e) {e.printStackTrace();}
    finally 
    {
      if (jtxrs != null) jtxrs.close();
      if (conn != null) conn.close();
    }
  }

  //----------------------------------------------------------------------------
  /**
   * Manages login with the FAOAuth servlet.
   */
  private void FAOAuthLogIn(String UserIndex, String Pwd)
  {
    DBConnection conn = null;

    try
    {
      m_UserAuthorization = false;

      if (UserIndex == null || Pwd == null || m_FAOAuthServiceURL.length() == 0)
        return;

      // FAO user authentication
      boolean bOk = checkUserid(m_FAOAuthServiceURL, UserIndex, Pwd);
      if (!bOk)
        return;

      // Verify if the user is application-enabled
      conn = new DBConnection(m_JDBCDriver, m_JDBCDatabase);

      RecordSet jtxrs = new RecordSet(conn);
      jtxrs.select("SELECT COUNT(*) FROM syssecu WHERE indx_num = '" + UserIndex + "'", 1);

      String usercount = jtxrs.getField(0, 0);
      if (usercount != null && usercount.equals("1"))
      {
        m_UserAuthorization = true;
        m_Usr = UserIndex;        
        // Extract user auths and store them into memory through hashtable
        GetCurrentUserPrivileges(jtxrs, UserIndex);
      }

      jtxrs.close();
    }
    catch(Exception e) {e.printStackTrace();}
    finally {if (conn != null) conn.close();}
  }

  //----------------------------------------------------------------------------
  private void GetCurrentUserPrivileges(RecordSet jtxrs, String UserIndex)
  {
    // Extract user auths and store them into memory through hashtable
    jtxrs.select("SELECT function, authlevel FROM sysprivileg WHERE indx_num = '" +
                 UserIndex + "'", 0);

    String func = null;
    String auth = null;
    while (jtxrs.next())
    {
      func = jtxrs.getField("function");
      auth = jtxrs.getField("authlevel");
      if (func != null && auth != null)
        m_AuthLevels.put(func, auth); // put privileges in hash table
    }
  }

  //----------------------------------------------------------------------------
  // FAO AFI user authentication code
  private static boolean checkUserid(String auth_service_url, String domain, String userid, String passwd)
  {
    StringBuffer s = new StringBuffer();

    try
    {
      if ( userid==null || userid.length()==0 || passwd==null || passwd.length()==0 )
      {
        System.out.println("NT Security Check: Empty/null username/password!" );
        return false;
      }

/*
      String urltxt = "http://fao_auth_service_one:81/default.htm?userid="+userid+"&password="+passwd;
      String urltxt = "http://webrefs2.fao.org:2234/default.htm?userid="+userid+"&password="+passwd;
 */
      String urltxt = auth_service_url + "?userid="+userid+"&password="+passwd;
      if ( domain != null && domain.trim().length() > 0 )
        urltxt += "&domain="+domain;

      URL                 url   = new URL(urltxt);
      URLConnection       conn  = url.openConnection();
      BufferedInputStream bis   = new BufferedInputStream(conn.getInputStream());
      
      while (true)
      {
        int c = bis.read();
        if (c == -1)
          break;
        s.append((char)c);
      }

    }
    catch ( MalformedURLException e )
    {
      System.out.println("NT Security Check: Bad URL! : "+e.toString() );
      return false;
    }
    catch ( IOException e )
    {
      System.out.println("NT Security Check: "+e.toString() );
      return false;
    }

    return ( s.toString().indexOf("OK") >= 0 );
  }

  //----------------------------------------------------------------------------
  private static boolean checkUserid(String auth_service_url, String userid, String passwd)
  {
    String domain = null;

    StringTokenizer st = new StringTokenizer(userid, "/\\");
    if ( st.countTokens() == 2 )
    {
      domain = st.nextToken();
      userid = st.nextToken();
    }

    return checkUserid(auth_service_url, domain, userid, passwd);
  }
  
  //----------------------------------------------------------------------------
  public String GetCurrentUser() {return m_Usr;}
}
